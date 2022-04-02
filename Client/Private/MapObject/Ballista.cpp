#include "stdafx.h"
#include "..\public\MapObject/Ballista.h"
#include "GameInstance.h"

#ifdef USE_IMGUI
#include "imgui_Manager.h"
#endif

CBallista::CBallista(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBallista::CBallista(const CBallista& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBallista::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBallista::NativeConstruct(void* pArg)
{
	if (SetUp_Component())
		return E_FAIL;

	m_pModelCom->SetUp_Animation("Ballista_A.ao|Balliista_A_Idle");

	return S_OK;
}

_int CBallista::Tick(_float fTimeDelta)
{
	// 키프레임 0일때 화살을 생성하자.
	if (boltOnce == false && m_pModelCom->Get_Current_KeyFrame_Index("Ballista_A.ao|Ballista_A_Full") == 0)
	{
		if (FAILED(CObject_Manager::GetInstance()->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Ballista_Bolt",
			L"Prototype_GameObject_Ballista_Bolt", this)))
			assert(0);

		boltOnce = true;
	}

	// 키프레임 대충 10보다 클때 다시 락 풀고 
	if (boltOnce)
	{
		if (m_pModelCom->Get_Current_KeyFrame_Index("Ballista_A.ao|Ballista_A_Full") > 10)
		{
			boltOnce = false;
		}
	}

	m_pModelCom->Update_Animation(fTimeDelta);

	// UpdateState
	{
		if (m_pCurState != m_pNextState)
		{
			_bool isLoop = false;

			if (m_pNextState == "Ballista_A.ao|Balliista_A_Idle" ||
				m_pNextState == "Ballista_A.ao|Ballista_A_Full"
				)
			{
				isLoop = true;
			}

			m_pModelCom->SetUp_Animation(m_pNextState, isLoop);
			m_pCurState = m_pNextState;
		}
	}

	return _int();
}

_int CBallista::LateTick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// AddRenderGroup
	bool AddRenderGroup = false;
	if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 6.f))
		AddRenderGroup = true;

#ifdef USE_IMGUI
	if (m_bUseImGui) // IMGUI 툴로 배치할거다
		AddRenderGroup = true;
#endif

	if (AddRenderGroup)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			assert(0);
		if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
			assert(0);
	}

	RELEASE_INSTANCE(CGameInstance);
	return 0;
}

HRESULT CBallista::Render(_uint iPassIndex)
{
	if (FAILED(SetUp_ConstantTable(iPassIndex)))
		return E_FAIL;

	// Render
	_uint	iNumMeshContainer = m_pModelCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainer; ++i)
	{
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
		m_pModelCom->Set_ShaderResourceView("g_EmissiveTexture", i, aiTextureType_EMISSIVE);

		m_pModelCom->Render(i, iPassIndex);
	}

	return S_OK;
}

HRESULT CBallista::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	CGameObject::Render_Colliders();

#ifdef USE_IMGUI
	if (m_bUseImGui) // IMGUI 툴로 배치할거다
	{
		CImguiManager::GetInstance()->Transform_Control(m_pTransformCom, m_CloneIdx, &m_bUseImGui);
	}
#endif

	return S_OK;
}

HRESULT CBallista::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 7.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(10.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Ballista"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBallista::SetUp_ConstantTable(_uint iPassIndex)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Bind Transform
	m_pTransformCom->Bind_OnShader(m_pModelCom, "g_WorldMatrix");
	if (iPassIndex == 3) // shadow map
	{
		m_pModelCom->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_View())), sizeof(_float4x4));
		m_pModelCom->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_Proj())), sizeof(_float4x4));
	}
	else
	{
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom, "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom, "g_ProjMatrix");
	}

	// Branch to Use Normal Mapping
	m_pModelCom->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));

	// Branch to Use Emissive Mapping
	m_pModelCom->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}


CBallista* CBallista::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBallista* pInstance = new CBallista(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CBallista");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CBallista::Clone(void* pArg)
{
	CBallista* pInstance = new CBallista(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CBallista");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBallista::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}


// --------------------------------------------------------------------------------------
// Ballista_Bolt


CBallista_Bolt::CBallista_Bolt(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBallista_Bolt::CBallista_Bolt(const CBallista& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBallista_Bolt::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBallista_Bolt::NativeConstruct(void* pArg)
{
	if (SetUp_Component())
		return E_FAIL;

	// pArg는 pOwner가 될것이다. 
	if (pArg)
	{
		m_pOwner = static_cast<CGameObject*>(pArg);
	}

	// SetUp_BoneMatrix
	{
		CModel* pBallistaModel = (CModel*)m_pOwner->Get_ComponentPtr(L"Com_Model");
		if (nullptr == pBallistaModel)
			assert(0);

		// 바리스타 볼트 충돌체
		CCollider::COLLIDERDESC		ColliderDesc;
		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vPivot = _float3(0.f, 0.f, 0.f);
		ColliderDesc.fRadius = 2.0f;
		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, COL_MONSTER_WEAPON, true);

		ZeroMemory(&m_spearDesc, sizeof(SPEARDESC));
		m_spearDesc.pBoneMatrix = pBallistaModel->Get_CombinedMatrixPtr("Bone_BB_Bolt");
		m_spearDesc.OffsetMatrix = pBallistaModel->Get_OffsetMatrix("Bone_BB_Bolt");
		m_spearDesc.PivotMatrix = pBallistaModel->Get_PivotMatrix_Bones();

		CTransform* pBallistaTransform = (CTransform*)m_pOwner->Get_ComponentPtr(L"Com_Transform");
		if (nullptr == pBallistaTransform)
			assert(0);
		m_spearDesc.pTargetWorldMatrix = pBallistaTransform->Get_WorldFloat4x4Ptr();
	}

	return S_OK;
}

_int CBallista_Bolt::Tick(_float fTimeDelta)
{
	if (m_isDead)
		return -1;

	m_fLifeTime += fTimeDelta;
	// 충돌체 위치 Update
	{
		// For Weapon Collider
		for (auto pCollider : m_ColliderList)
		{
			if (pCollider->Get_ColliderTag() == COL_MONSTER_WEAPON)
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

	return _int();
}

_int CBallista_Bolt::LateTick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_fLifeTime > 15) // 대충 3초 이상 살았으면 죽이자 
		m_isDead = true;


	if (1)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			assert(0);
		if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
			assert(0);
	}

	RELEASE_INSTANCE(CGameInstance);
	return 0;
}

HRESULT CBallista_Bolt::Render(_uint iPassIndex)
{
	if (FAILED(SetUp_ConstantTable(iPassIndex)))
		return E_FAIL;

	// Render
	_uint	iNumMeshContainer = m_pModelCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainer; ++i)
	{
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);

		m_pModelCom->Render(i, iPassIndex);
	}

	return S_OK;
}

HRESULT CBallista_Bolt::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	CGameObject::Render_Colliders();

	return S_OK;
}

HRESULT CBallista_Bolt::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 20.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(10.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Ballista_Bolt"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBallista_Bolt::SetUp_ConstantTable(_uint iPassIndex)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Bind Matrix
	_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_spearDesc.pBoneMatrix);
	_matrix		OffsetMatrix = XMLoadFloat4x4(&m_spearDesc.OffsetMatrix);
	_matrix		PivotMatrix = XMLoadFloat4x4(&m_spearDesc.PivotMatrix);
	_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_spearDesc.pTargetWorldMatrix);

	// 이게 정답인가 보네...
	_matrix		TransformationMatrix = XMMatrixRotationX(XMConvertToRadians(-90)) /** OffsetMatrix*/ * CombinedTransformationMatrix * PivotMatrix * TargetWorldMatrix;
	_float4x4	modelWorldMat;
	XMStoreFloat4x4(&modelWorldMat, XMMatrixTranspose(TransformationMatrix));
	m_pModelCom->Set_RawValue("g_WorldMatrix", &modelWorldMat, sizeof(_float4x4));

	if (iPassIndex == 3) // shadow map
	{
		m_pModelCom->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_View())), sizeof(_float4x4));
		m_pModelCom->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_Proj())), sizeof(_float4x4));
	}
	else
	{
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom, "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom, "g_ProjMatrix");
	}

	// Branch to Use Normal Mapping 
	m_pModelCom->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));
	m_pModelCom->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}


CBallista_Bolt* CBallista_Bolt::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBallista_Bolt* pInstance = new CBallista_Bolt(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CBallista_Bolt");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CBallista_Bolt::Clone(void* pArg)
{
	CBallista_Bolt* pInstance = new CBallista_Bolt(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CBallista_Bolt");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBallista_Bolt::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}