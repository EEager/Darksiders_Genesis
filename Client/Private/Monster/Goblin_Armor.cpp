#include "stdafx.h"
#include "..\public\Monster\Goblin_Armor.h"
#include "GameInstance.h"


CGoblin_Armor::CGoblin_Armor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{
}

CGoblin_Armor::CGoblin_Armor(const CGoblin_Armor & rhs)
	: CMonster(rhs)
{
}

HRESULT CGoblin_Armor::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CGoblin_Armor::NativeConstruct(void * pArg)
{
	// 모든 몬스터는 m_pTransformCom, m_pRendererCom, m_pNaviCom를 가진다
	if (CMonster::NativeConstruct(pArg))
		return E_FAIL;	

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Goblin_Armor"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Collider */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vPivot = _float3(0.f, 0.75f, 0.f);
	ColliderDesc.vSize = _float3(0.5f, 1.5f, 0.5f);
	ColliderDesc.eColType = CCollider::COL_TYPE::COL_TYPE_AABB;
	__super::Add_Collider(&ColliderDesc, L"GoblinBody");

	// For Weapon
	{
		/* For.Com_Model_Goblin_Spear */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Goblin_Spear"), TEXT("Com_Model_Goblin_Spear"), (CComponent**)&m_pModelSpearCom)))
			return E_FAIL;
		/* For.Com_Model_Goblin_Quiver */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Goblin_Quiver"), TEXT("Com_Model_Goblin_Quiver"), (CComponent**)&m_pModelQuiverCom)))
			return E_FAIL;

		/* For.GoblinSpear */
		ColliderDesc.vPivot = _float3(0.f, -0.5f, 0.f); 
		ColliderDesc.fRadius = 0.25f;
		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, L"GoblinSpear");
		ZeroMemory(&m_spearDesc, sizeof(SPEARDESC));
		m_spearDesc.pBoneMatrix = m_pModelCom->Get_CombinedMatrixPtr("Bone_Goblin_Weapon_Fleamag_Sword");
		m_spearDesc.OffsetMatrix = m_pModelCom->Get_OffsetMatrix("Bone_Goblin_Weapon_Fleamag_Sword");
		m_spearDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix_Bones();
		m_spearDesc.pTargetWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();

		/* For.GoblinQuiver */
		ZeroMemory(&m_quiverDesc, sizeof(QUIVERDESC));
		m_quiverDesc.pBoneMatrix = m_pModelCom->Get_CombinedMatrixPtr("Bone_Goblin_Quiver");
		m_quiverDesc.OffsetMatrix = m_pModelCom->Get_OffsetMatrix("Bone_Goblin_Quiver");
		m_quiverDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix_Bones();
		m_quiverDesc.pTargetWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();
	}


	// Init test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(85.f + rand()%10, 0.f, 431.f + rand() % 10, 1.f));
	m_pModelCom->SetUp_Animation("Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_Spear");


	// 모든 몬스터는 Navigation 초기 인덱스를 잡아줘야한다
	m_pNaviCom->SetUp_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));

	return S_OK;
}

_int CGoblin_Armor::Tick(_float fTimeDelta)
{
	// For Weapon Collider
	{
		for (auto pCollider : m_ColliderList)
		{
			if (pCollider->Get_ColliderTag() == L"GoblinSpear")
			{
				_matrix		OffsetMatrix = XMLoadFloat4x4(&m_spearDesc.OffsetMatrix); 
				_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_spearDesc.pBoneMatrix); 
				_matrix		PivotMatrix = XMLoadFloat4x4(&m_spearDesc.PivotMatrix);
				_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_spearDesc.pTargetWorldMatrix); 
				_matrix		TransformationMatrix =
					(OffsetMatrix * CombinedTransformationMatrix * PivotMatrix) *
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
	m_pModelCom->Update_Animation(fTimeDelta, static_cast<CTransform*>(m_pTransformCom)->Get_WorldMatrix_4x4(), "Bone_Goblin_Root", m_pNaviCom);

	// for test
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.001f);

	return _int();
}

_int CGoblin_Armor::LateTick(_float fTimeDelta)
{
	// 모든 몬스터는 Height, Renderer, Add_Collider
	if (CMonster::LateTick(fTimeDelta) < 0)
		return -1;

	return _int();
}

HRESULT CGoblin_Armor::Render(_uint iPassIndex)
{
	// 모든 몬스터는 SetUp_ConstantTable, RenderColliders
	if (CMonster::Render(iPassIndex) < 0)
		return -1;


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	LIGHTDESC		dirLightDesc = *pGameInstance->Get_LightDesc(0);
	DirectionalLight mDirLight;
	mDirLight.Ambient = dirLightDesc.vAmbient;
	mDirLight.Diffuse = dirLightDesc.vDiffuse;
	mDirLight.Specular = dirLightDesc.vSpecular;
	mDirLight.Direction = dirLightDesc.vDirection;

	_float4			vCamPosition;
	XMStoreFloat4(&vCamPosition, pGameInstance->Get_CamPosition());

	// Render Sphere
	{
		// Bind Directional Light

		m_pModelSpearCom->Set_RawValue("g_DirLight", &mDirLight, sizeof(DirectionalLight));
		// Bind Material
		m_pModelSpearCom->Set_RawValue("g_Material", &m_tMtrlDesc, sizeof(MTRLDESC));

		_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_spearDesc.pBoneMatrix);
		_matrix		OffsetMatrix = XMLoadFloat4x4(&m_spearDesc.OffsetMatrix);
		_matrix		PivotMatrix = XMLoadFloat4x4(&m_spearDesc.PivotMatrix);
		_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_spearDesc.pTargetWorldMatrix);

		// 이게 정답인가 보네...
		_matrix		TransformationMatrix = XMMatrixRotationX(XMConvertToRadians(-90)) /** OffsetMatrix*/ * CombinedTransformationMatrix * PivotMatrix * TargetWorldMatrix;
		_float4x4	modelWorldMat;
		XMStoreFloat4x4(&modelWorldMat, XMMatrixTranspose(TransformationMatrix));
		m_pModelSpearCom->Set_RawValue("g_WorldMatrix", &modelWorldMat, sizeof(_float4x4));

		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelSpearCom, "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelSpearCom, "g_ProjMatrix");

		// Bind Position
		m_pModelSpearCom->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));

		// Branch to Use Normal Mapping 
		m_pModelSpearCom->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));
		m_pModelSpearCom->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));

		_uint	iNumMaterials = m_pModelSpearCom->Get_NumMaterials();
		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelSpearCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
			m_pModelSpearCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
			m_pModelSpearCom->Render(i, 0);
		}
	}


	// Render Quiver
	{
		m_pModelQuiverCom->Set_RawValue("g_DirLight", &mDirLight, sizeof(DirectionalLight));
		// Bind Material
		m_pModelQuiverCom->Set_RawValue("g_Material", &m_tMtrlDesc, sizeof(MTRLDESC));

		_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_quiverDesc.pBoneMatrix);
		_matrix		OffsetMatrix = XMLoadFloat4x4(&m_quiverDesc.OffsetMatrix);
		_matrix		PivotMatrix = XMLoadFloat4x4(&m_quiverDesc.PivotMatrix);
		_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_quiverDesc.pTargetWorldMatrix);

		_matrix		TransformationMatrix =  XMMatrixRotationX(XMConvertToRadians(90)) * XMMatrixRotationY(XMConvertToRadians(180)) * CombinedTransformationMatrix * PivotMatrix * TargetWorldMatrix;
		_float4x4	modelWorldMat;
		XMStoreFloat4x4(&modelWorldMat, XMMatrixTranspose(TransformationMatrix));
		m_pModelQuiverCom->Set_RawValue("g_WorldMatrix", &modelWorldMat, sizeof(_float4x4));

		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelQuiverCom, "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelQuiverCom, "g_ProjMatrix");

		// Bind Position
		m_pModelQuiverCom->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));

		// Branch to Use Normal Mapping 
		m_pModelQuiverCom->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));
		m_pModelQuiverCom->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));

		_uint	iNumMaterials = m_pModelQuiverCom->Get_NumMaterials();
		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelQuiverCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
			m_pModelQuiverCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
			m_pModelQuiverCom->Render(i, 0);
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CGoblin_Armor * CGoblin_Armor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGoblin_Armor*		pInstance = new CGoblin_Armor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CGoblin_Armor");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CGoblin_Armor::Clone(void* pArg)
{
	CGoblin_Armor*		pInstance = new CGoblin_Armor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CGoblin_Armor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGoblin_Armor::Free()
{
	CMonster::Free();

	Safe_Release(m_pModelSpearCom);
	Safe_Release(m_pModelQuiverCom);
}
