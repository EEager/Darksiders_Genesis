#include "stdafx.h"
#include "..\public\Fork.h"

#include "GameInstance.h"

#ifdef USE_IMGUI
#include "imgui_Manager.h"
#endif

CFork::CFork(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CFork::CFork(const CFork& rhs)
	: CGameObject(rhs)
{
}

HRESULT CFork::NativeConstruct_Prototype()
{	

	// Material Init
	m_tMtrlDesc.vMtrlDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_tMtrlDesc.vMtrlAmbient = { 0.5f, 0.5f, 0.5f, 1.0f };
	m_tMtrlDesc.vMtrlSpecular = { 0.2f, 0.2f, 0.2f, 16.0f };
	m_tMtrlDesc.vMtrlEmissive = { 1.f, 1.f, 1.f, 1.f };
	m_tMtrlDesc.fMtrlPower = 20.f;

	return S_OK;
}

HRESULT CFork::NativeConstruct(void * pArg)
{
	if (SetUp_Component())
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(230.f + 2.f, 5.f, 430.f, 1.f));

	return S_OK;
}

_int CFork::Tick(_float fTimeDelta)
{
	if (__super::Tick(fTimeDelta) < 0)
		return -1;

	// Init
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	m_pTransformCom->Turn(XMVectorSet(1.f, 0.f, 0.f, 0.f), fTimeDelta*2);
	m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta*3);

	// Collider 
	__super::Update_Colliders(m_pTransformCom->Get_WorldMatrix());

	return _int();
}

_int CFork::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//// Height
	//_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_float curFloorHeight = m_pNaviCom->Compute_Height(vPosition);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, curFloorHeight));

	// AddRenderGroup
	bool AddRenderGroup = false;
	if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
		AddRenderGroup = true;

#ifdef USE_IMGUI
	// 디버깅이 필요하면 이거 넣어줘야한다
	if (m_bUseImGui)
		AddRenderGroup = true;
#endif

	if (AddRenderGroup)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			assert(0);
		if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
			assert(0);
	}

	// Collider 
	pGameInstance->Add_Collision(this);

	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

HRESULT CFork::Render(_uint iPassIndex)
{

	if (FAILED(SetUp_ConstantTable(iPassIndex)))
		return E_FAIL;

	/* 장치에 월드변환 행렬을 저장한다. */
	_uint	iNumMeshContainer = m_pModelCom->Get_NumMeshContainer();

	
	for (_uint i = 0; i < iNumMeshContainer; ++i)
	{
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pModelCom->Render(i, iPassIndex); // Deferred
	}

	// restore default states, as the SkyFX changes them in the effect file.
	m_pDeviceContext->RSSetState(0);
	m_pDeviceContext->OMSetDepthStencilState(0, 0);


	return S_OK;
}

HRESULT CFork::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
#ifdef _DEBUG
	// Collider 
	__super::Render_Colliders();
#endif // _DEBUG

#ifdef USE_IMGUI
	if (m_bUseImGui) 
	{
		CImguiManager::GetInstance()->Transform_Control(m_pTransformCom, m_CloneIdx, &m_bUseImGui);
	}
#endif

	return S_OK;
}

void CFork::OnCollision_Enter(CGameObject* pDst, float fTimeDelta)
{
	
}

void CFork::OnCollision_Stay(CGameObject* pDst, float fTimeDelta)
{
}

void CFork::OnCollision_Leave(CGameObject* pDst, float fTimeDelta)
{
}

HRESULT CFork::SetUp_Component()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fork"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Navi */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navi"), (CComponent**)m_pNaviCom.GetAddressOf())))
		return E_FAIL;


	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_Texture"), (CComponent**)m_pDissolveTextureCom.GetAddressOf())))
		return E_FAIL;


	/* For.Com_AABB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ColliderDesc.vPivot = _float3(0.f, 2.5f, 0.f);
	ColliderDesc.vSize = _float3(2.f, 5.0f, 5.0f);
	ColliderDesc.eColType = CCollider::COL_TYPE::COL_TYPE_AABB;
	__super::Add_Collider(&ColliderDesc, L"Fork1");

	/* For.Com_OBB */
	ColliderDesc.vPivot = static_cast<CModel*>(m_pModelCom)->Get_Center();
	ColliderDesc.vSize = static_cast<CModel*>(m_pModelCom)->Get_Extents();
	ColliderDesc.eColType = CCollider::COL_TYPE::COL_TYPE_OBB;
	__super::Add_Collider(&ColliderDesc, L"Fork2");

	return S_OK;
}

HRESULT CFork::SetUp_ConstantTable(_uint iPassIndex)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Bind Transform
	m_pTransformCom->Bind_OnShader(m_pModelCom, "g_WorldMatrix");
	if (iPassIndex == 3) // ShadowMap
	{
		m_pModelCom->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_View())), sizeof(_float4x4));
		m_pModelCom->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_Proj())), sizeof(_float4x4));
	}
	else
	{
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom, "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom, "g_ProjMatrix");
	}

	// Bind Dissolve 
	dissolvePower += 0.002f;
	if (dissolvePower >= 1.f) // 1이면 다 사라졌다
		dissolvePower = 0.f;
	m_pModelCom->Set_RawValue("g_DissolvePwr", &dissolvePower, sizeof(_float));
	if (FAILED(m_pDissolveTextureCom->SetUp_OnShader(m_pModelCom, "g_DissolveTexture")))
		return E_FAIL;

	// Emissive Map
	_bool falseTemp = false;
	m_pModelCom->Set_RawValue("g_UseEmissiveMap", &falseTemp, sizeof(_bool));
	m_pModelCom->Set_RawValue("g_UseNormalMap", &falseTemp, sizeof(_bool));


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CFork * CFork::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CFork*		pInstance = new CFork(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CFork");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CFork::Clone(void* pArg)
{
	CFork*		pInstance = new CFork(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CFork");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFork::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
