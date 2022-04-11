#include "stdafx.h"
#include "..\public\MapObject/Trees.h"
#include "GameInstance.h"

#ifdef USE_IMGUI
#include "imgui_Manager.h"
#endif

CTree::CTree(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTree::CTree(const CTree& rhs)
	: CGameObject(rhs),
	m_pRendererCom(rhs.m_pRendererCom),
	m_pModelCom(rhs.m_pModelCom),
	m_bIsCloned(true)
{
}

HRESULT CTree::NativeConstruct_Prototype()
{
	//SetUp_Component(TEXT("Prototype_Component_Model_TreeA"));

	return S_OK;
}

HRESULT CTree::NativeConstruct(void* pArg)
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 7.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(10.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	// Test - War 위치
	auto pWarTransform = static_cast<CTransform*>(CObject_Manager::GetInstance()->Get_GameObject_CloneList(L"Layer_War")->front()->Get_ComponentPtr(L"Com_Transform"));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pWarTransform->Get_State(CTransform::STATE_POSITION));

	return S_OK;
}

_int CTree::Tick(_float fTimeDelta)
{
	if (m_isDead)
		return -1;
	return _int();
}

_int CTree::LateTick(_float fTimeDelta)
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

HRESULT CTree::Render(_uint iPassIndex)
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

HRESULT CTree::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
#ifdef USE_IMGUI
	if (m_bUseImGui) // IMGUI 툴로 배치할거다
	{
		CImguiManager::GetInstance()->Transform_Control(m_pTransformCom, m_CloneIdx, &m_bUseImGui);

		// 죽는 버튼 하나 만들자.
		char TagTmp[32];
		sprintf_s(TagTmp, "Edit##%d", m_CloneIdx);
		ImGui::Begin(TagTmp);
		{
			if (ImGui::Button("Die"))
			{
				m_isDead = true;
			}
		}
		ImGui::End();
	}
#endif

	return S_OK;
}

HRESULT CTree::SetUp_Component(const _tchar* pModelTag)
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, pModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTree::SetUp_ConstantTable(_uint iPassIndex)
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

CTree* CTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTree* pInstance = new CTree(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CTree");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CTree::Clone(void* pArg)
{
	CTree* pInstance = new CTree(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CTree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTree::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);

	if (m_bIsCloned == false)
	{
		Safe_Release(m_pRendererCom);
		Safe_Release(m_pModelCom);
	}
}


// ------------------------------------
// CTreeA

CTreeA::CTreeA(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CTree(pDevice, pDeviceContext)
{
}

CTreeA::CTreeA(const CTreeA& rhs)
	: CTree(rhs)
{
}

HRESULT CTreeA::NativeConstruct_Prototype()
{
	CTree::SetUp_Component(TEXT("Prototype_Component_Model_TreeA"));

	return S_OK;
}

HRESULT CTreeA::NativeConstruct(void* pArg)
{
	CTree::NativeConstruct(pArg);
	return S_OK;
}

_int CTreeA::Tick(_float fTimeDelta)
{
	return CTree::Tick(fTimeDelta);
}

_int CTreeA::LateTick(_float fTimeDelta)
{
	CTree::LateTick(fTimeDelta);
	return 0;

}

HRESULT CTreeA::Render(_uint iPassIndex)
{
	CTree::Render(iPassIndex);
	return S_OK;

}

HRESULT CTreeA::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	CTree::PostRender(m_spriteBatch, m_spriteFont);
	return S_OK;
}


CTreeA* CTreeA::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTreeA* pInstance = new CTreeA(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CTreeA");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CTreeA::Clone(void* pArg)
{
	CTreeA* pInstance = new CTreeA(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CTreeA");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTreeA::Free()
{
	CTree::Free();
}


// ------------------------------------
// CTreeB
CTreeB::CTreeB(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CTree(pDevice, pDeviceContext)
{
}

CTreeB::CTreeB(const CTreeB& rhs)
	: CTree(rhs)
{
}

HRESULT CTreeB::NativeConstruct_Prototype()
{
	CTree::SetUp_Component(TEXT("Prototype_Component_Model_TreeB"));

	return S_OK;
}

HRESULT CTreeB::NativeConstruct(void* pArg)
{
	CTree::NativeConstruct(pArg);
	return S_OK;
}

_int CTreeB::Tick(_float fTimeDelta)
{
	return CTree::Tick(fTimeDelta);
}

_int CTreeB::LateTick(_float fTimeDelta)
{
	CTree::LateTick(fTimeDelta);
	return 0;

}

HRESULT CTreeB::Render(_uint iPassIndex)
{
	CTree::Render(iPassIndex);
	return S_OK;

}

HRESULT CTreeB::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	CTree::PostRender(m_spriteBatch, m_spriteFont);
	return S_OK;
}


CTreeB* CTreeB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTreeB* pInstance = new CTreeB(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CTreeB");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CTreeB::Clone(void* pArg)
{
	CTreeB* pInstance = new CTreeB(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CTreeB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTreeB::Free()
{
	CTree::Free();
}


// ------------------------------------
// CTreeC
CTreeC::CTreeC(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CTree(pDevice, pDeviceContext)
{
}

CTreeC::CTreeC(const CTreeC& rhs)
	: CTree(rhs)
{
}

HRESULT CTreeC::NativeConstruct_Prototype()
{
	CTree::SetUp_Component(TEXT("Prototype_Component_Model_TreeC"));

	return S_OK;
}

HRESULT CTreeC::NativeConstruct(void* pArg)
{
	CTree::NativeConstruct(pArg);
	return S_OK;
}

_int CTreeC::Tick(_float fTimeDelta)
{
	return CTree::Tick(fTimeDelta);
}

_int CTreeC::LateTick(_float fTimeDelta)
{
	CTree::LateTick(fTimeDelta);
	return 0;

}

HRESULT CTreeC::Render(_uint iPassIndex)
{
	CTree::Render(iPassIndex);
	return S_OK;

}

HRESULT CTreeC::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	CTree::PostRender(m_spriteBatch, m_spriteFont);
	return S_OK;
}

CTreeC* CTreeC::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTreeC* pInstance = new CTreeC(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CTreeC");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTreeC::Clone(void* pArg)
{
	CTreeC* pInstance = new CTreeC(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CTreeC");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTreeC::Free()
{
	CTree::Free();
}