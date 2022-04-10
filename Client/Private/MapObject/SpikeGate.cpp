#include "stdafx.h"
#include "..\public\MapObject/SpikeGate.h"
#include "GameInstance.h"

#ifdef USE_IMGUI
#include "imgui_Manager.h"
#endif

CSpikeGate::CSpikeGate(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSpikeGate::CSpikeGate(const CSpikeGate& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSpikeGate::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CSpikeGate::NativeConstruct(void* pArg)
{
	if (SetUp_Component())
		return E_FAIL;

	// Test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(589, 10.5f, 83, 1.f));

	m_pModelCom->SetUp_Animation("Dn_HL_SpikeGate_A.ao|Dn_HL_SpikeGate_Out", false, false);
	//m_pModelCom->SetUp_Animation("Dn_HL_SpikeGate_A.ao|Dn_HL_SpikeGate_Out");

	return S_OK;
}

_int CSpikeGate::Tick(_float fTimeDelta)
{
	if (m_isDead)
	{
		return -1;
	}

	// Update Animation
	if (m_bAnimFinished == false)
	{
		m_pModelCom->Update_Animation(fTimeDelta);

		if (m_pModelCom->Get_Animation_isFinished("Dn_HL_SpikeGate_A.ao|Dn_HL_SpikeGate_Out"))
			m_bAnimFinished = true;
	}

	return _int();
}

_int CSpikeGate::LateTick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// AddRenderGroup
	{
		bool AddRenderGroup = false;
		if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 7.f))
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
	}

	RELEASE_INSTANCE(CGameInstance);
	return 0;
}

HRESULT CSpikeGate::Render(_uint iPassIndex)
{
	if (FAILED(SetUp_ConstantTable(iPassIndex)))
		return E_FAIL;

	_uint	iNumMeshContainer = m_pModelCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainer; ++i)
	{
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);

		m_pModelCom->Render(i, iPassIndex);
	}

	return S_OK;
}

HRESULT CSpikeGate::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
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

HRESULT CSpikeGate::SetUp_Component()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpikeGate"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpikeGate::SetUp_ConstantTable(_uint iPassIndex)
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

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CSpikeGate* CSpikeGate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CSpikeGate* pInstance = new CSpikeGate(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CSpikeGate");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CSpikeGate::Clone(void* pArg)
{
	CSpikeGate* pInstance = new CSpikeGate(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CSpikeGate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpikeGate::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

