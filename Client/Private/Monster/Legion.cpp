#include "stdafx.h"
#include "..\public\Monster\Legion.h"
#include "GameInstance.h"


CLegion::CLegion(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{
}

CLegion::CLegion(const CLegion & rhs)
	: CMonster(rhs)
{
}

HRESULT CLegion::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CLegion::NativeConstruct(void * pArg)
{
	// 모든 몬스터는 m_pTransformCom, m_pRendererCom, m_pNaviCom를 가진다
	if (CMonster::NativeConstruct(pArg))
		return E_FAIL;	

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legion"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Collider */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vPivot = _float3(0.f, 1.0f, 0.f);
	ColliderDesc.vSize = _float3(1.5f, 2.0f, 1.5f);
	ColliderDesc.eColType = CCollider::COL_TYPE::COL_TYPE_AABB;
	__super::Add_Collider(&ColliderDesc, L"LegionBody");


	// For Weapon
	{ 
		/* For.Com_Model_Legion_WeaponL */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legion_Axe_L"), TEXT("Com_Model_Legion_WeaponL"), (CComponent**)&m_pModelWeaponLCom)))
			return E_FAIL;
		/* For.Com_Model_Legion_WeaponR */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legion_Axe_R"), TEXT("Com_Model_Legion_WeaponR"), (CComponent**)&m_pModelWeaponRCom)))
			return E_FAIL;

		/* For.LegionWeapon */
		ColliderDesc.vPivot = _float3(0.f, 0.0f, -.75f); // For Test
		ColliderDesc.fRadius = 0.5f;

		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, L"LegionWeapon");

		ZeroMemory(&m_WarSwordDesc, sizeof(SWORDDESC));
		m_WarSwordDesc.pBoneMatrix = m_pModelCom->Get_CombinedMatrixPtr("Bone_LE_Weapon");
		m_WarSwordDesc.OffsetMatrix = m_pModelCom->Get_OffsetMatrix("Bone_LE_Weapon");
		m_WarSwordDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix_Bones();
		m_WarSwordDesc.pTargetWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();
	}


	// Init test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(85.f + rand()%10, 0.f, 431.f + rand() % 10, 1.f));
	//m_pModelCom->SetUp_Animation("Legion_Mesh.ao|Legion_Idle");
	m_pModelCom->SetUp_Animation((_uint)0);


	// 모든 몬스터는 Navigation 초기 인덱스를 잡아줘야한다
	m_pNaviCom->SetUp_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));
	
	return S_OK;
}

_int CLegion::Tick(_float fTimeDelta)
{
	// For Weapon Collider
	{
		for (auto pCollider : m_ColliderList)
		{
			if (pCollider->Get_ColliderTag() == L"LegionWeapon")
			{
				_matrix		OffsetMatrix = XMLoadFloat4x4(&m_WarSwordDesc.OffsetMatrix); // 뼈->정점
				_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_WarSwordDesc.pBoneMatrix); // Root->뼈 
				_matrix		PivotMatrix = XMLoadFloat4x4(&m_WarSwordDesc.PivotMatrix);
				_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_WarSwordDesc.pTargetWorldMatrix); // just legion's world matrix
				_matrix		TransformationMatrix =
					(CombinedTransformationMatrix * PivotMatrix) * //OffsetMatrix를 안곱하니간 뭔가 되는거 같다... 왜?
					TargetWorldMatrix;
				pCollider->Update(TransformationMatrix);
			}
			else
			{
				pCollider->Update(m_pTransformCom->Get_WorldMatrix());
			}
		}
	}

	// 로컬이동값 -> 월드이동반영
	m_pModelCom->Update_Animation(fTimeDelta, static_cast<CTransform*>(m_pTransformCom)->Get_WorldMatrix_4x4(), "_Ctrl_World", m_pNaviCom);

	// for test
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.001f);

	return _int();
}

_int CLegion::LateTick(_float fTimeDelta)
{
	// 모든 몬스터는 Height, Renderer, Add_Collider
	if (CMonster::LateTick(fTimeDelta) < 0)
		return -1;

	return _int();
}

HRESULT CLegion::Render()
{
	// 모든 몬스터는 SetUp_ConstantTable, RenderColliders
	if (CMonster::Render() < 0)
		return -1;

	// Weapon Render : ToDo 최적화
	Render_Weapon(m_pModelWeaponLCom, XMConvertToRadians(-90));
	Render_Weapon(m_pModelWeaponRCom, XMConvertToRadians(-90));

	return S_OK;
}


HRESULT CLegion::Render_Weapon(CModel* pModel, _float fRadian)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Bind Directional Light
	LIGHTDESC		dirLightDesc = *pGameInstance->Get_LightDesc(0);
	DirectionalLight mDirLight;
	mDirLight.Ambient = dirLightDesc.vAmbient;
	mDirLight.Diffuse = dirLightDesc.vDiffuse;
	mDirLight.Specular = dirLightDesc.vSpecular;
	mDirLight.Direction = dirLightDesc.vDirection;
	pModel->Set_RawValue("g_DirLight", &mDirLight, sizeof(DirectionalLight));
	// Bind Material
	pModel->Set_RawValue("g_Material", &m_tMtrlDesc, sizeof(MTRLDESC));

	_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_WarSwordDesc.pBoneMatrix);
	_matrix		PivotMatrix = XMLoadFloat4x4(&m_WarSwordDesc.PivotMatrix);
	_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_WarSwordDesc.pTargetWorldMatrix);
	_matrix		TransformationMatrix = XMMatrixRotationX(fRadian) * CombinedTransformationMatrix * PivotMatrix * TargetWorldMatrix;
	_float4x4 modelWeaponWorldMat;
	XMStoreFloat4x4(&modelWeaponWorldMat, XMMatrixTranspose(TransformationMatrix));
	pModel->Set_RawValue("g_WorldMatrix", &modelWeaponWorldMat, sizeof(_float4x4));

	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, pModel, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, pModel, "g_ProjMatrix");

	// Bind Position
	_float4			vCamPosition;
	XMStoreFloat4(&vCamPosition, pGameInstance->Get_CamPosition());
	pModel->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));

	// Branch to Use Normal Mapping 
	pModel->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));
	pModel->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));

	RELEASE_INSTANCE(CGameInstance);

	_uint	iNumMaterials = pModel->Get_NumMaterials();
	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		pModel->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		pModel->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
		pModel->Render(i, 0);
	}

	return S_OK;
}


CLegion * CLegion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLegion*		pInstance = new CLegion(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CLegion");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CLegion::Clone(void* pArg)
{
	CLegion*		pInstance = new CLegion(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CLegion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLegion::Free()
{
	CMonster::Free();

	Safe_Release(m_pModelWeaponLCom);
	Safe_Release(m_pModelWeaponRCom);
}
