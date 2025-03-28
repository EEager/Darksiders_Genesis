#include "stdafx.h"
#include "..\public\MeshEffect.h"

#include "GameInstance.h"

#include "Trail.h"

#ifdef USE_IMGUI
#include "imgui_Manager.h"
#endif

// --------------------------
// CMeshEffect_ChaosEater
// --------------------------
CMeshEffect_ChaosEater::CMeshEffect_ChaosEater(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMeshEffect_ChaosEater::CMeshEffect_ChaosEater(const CMeshEffect_ChaosEater& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMeshEffect_ChaosEater::NativeConstruct_Prototype()
{	
	return S_OK;
}

HRESULT CMeshEffect_ChaosEater::NativeConstruct(void * pArg)
{
	// 게임 정보
	m_tGameInfo.iAtt = 10.f;

	if (!pArg) // 무조건 이펙트를 생성할 위치 + LookAt 할 방향이다.
		assert(0);

	if (SetUp_Component())
		return E_FAIL; 

	auto tagDesc = (CHAOSEATERDESC*)pArg;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, tagDesc->vPos);
	m_pTransformCom->LookAt(tagDesc->vLook);

	return S_OK;
}

_int CMeshEffect_ChaosEater::Tick(_float fTimeDelta)
{
	if (m_isDead)
		return -1;

	// 조금씩 앞으로 가자.
	m_pTransformCom->Go_Straight(fTimeDelta/10.f);

	// Collider 
	__super::Update_Colliders(m_pTransformCom->Get_WorldMatrix());


	return _int();
}

_int CMeshEffect_ChaosEater::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// AddRenderGroup
	bool AddRenderGroup = false;
	if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
		AddRenderGroup = true;

#ifdef USE_IMGUI
	// 디버깅이 필요하면 이거 넣어줘야한다
	if (m_bUseImGui)
	{
		AddRenderGroup = true;
	}
#endif

	if (AddRenderGroup)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			assert(0);
		//if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
		//	assert(0); 
	}

	// Collider 
	pGameInstance->Add_Collision(this);
	RELEASE_INSTANCE(CGameInstance);

	// 디졸브 증가 시키자
	dissolvePower += fTimeDelta / 1.3f;
	if (dissolvePower >= 1.f) // 1이면 다 사라졌다. 이때는 진짜로 죽이자.
	{
		m_isDead = true;
	}

	return _int();
}

HRESULT CMeshEffect_ChaosEater::Render(_uint iPassIndex)
{
	if (FAILED(SetUp_ConstantTable(iPassIndex)))
		return E_FAIL;

	/* 장치에 월드변환 행렬을 저장한다. */
	_uint	iNumMeshContainer = m_pModelCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainer; ++i)
	{
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
		m_pModelCom->Set_ShaderResourceView("g_EmissiveTexture", i, aiTextureType_EMISSIVE);

		m_pModelCom->Render(i, iPassIndex);
	}

	// restore default states, as the SkyFX changes them in the effect file.
	m_pDeviceContext->RSSetState(0);
	m_pDeviceContext->OMSetDepthStencilState(0, 0);

	return S_OK;
}

HRESULT CMeshEffect_ChaosEater::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
#ifdef _DEBUG
	// Collider 
	__super::Render_Colliders();
#endif // _DEBUG

	return S_OK;
}


HRESULT CMeshEffect_ChaosEater::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 7.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(10.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)m_pTransformCom.GetAddressOf(), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)m_pRendererCom.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MeshEffect_ChaosEater"), TEXT("Com_Model"), (CComponent**)m_pModelCom.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Texture_Disolve */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_Texture"), (CComponent**)m_pDissolveTextureCom.GetAddressOf())))
		return E_FAIL;

	/* For.Com_AABB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ColliderDesc.fRadius = 3.0f;
	ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
	__super::Add_Collider(&ColliderDesc, COL_WAR_WEAPON);

	return S_OK;
}

HRESULT CMeshEffect_ChaosEater::SetUp_ConstantTable(_uint iPassIndex)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Bind Transform
	m_pTransformCom->Bind_OnShader(m_pModelCom.Get(), "g_WorldMatrix");
	if (iPassIndex == 3) // ShadowMap
	{
		m_pModelCom->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_View())), sizeof(_float4x4));
		m_pModelCom->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_Proj())), sizeof(_float4x4));
	}
	else
	{
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom.Get(), "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom.Get(), "g_ProjMatrix");
	}

	// Bind Dissolve 
	m_pModelCom->Set_RawValue("g_DissolvePwr", &dissolvePower, sizeof(_float));
	if (FAILED(m_pDissolveTextureCom->SetUp_OnShader(m_pModelCom.Get(), "g_DissolveTexture")))
		return E_FAIL;

	// Emissive Map
	_bool falseTemp = false;
	_bool trueTemp = true;
	m_pModelCom->Set_RawValue("g_UseEmissiveMap", &trueTemp, sizeof(_bool));
	m_pModelCom->Set_RawValue("g_UseNormalMap", &trueTemp, sizeof(_bool));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CMeshEffect_ChaosEater * CMeshEffect_ChaosEater::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMeshEffect_ChaosEater*		pInstance = new CMeshEffect_ChaosEater(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CMesh_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMeshEffect_ChaosEater::Clone(void* pArg)
{
	CMeshEffect_ChaosEater*		pInstance = new CMeshEffect_ChaosEater(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CMesh_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshEffect_ChaosEater::Free()
{
	__super::Free();
}


// ---------------------------------------------------
// CMeshEffect_Sphere
// ---------------------------------------------------
CMeshEffect_Sphere::CMeshEffect_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMeshEffect_Sphere::CMeshEffect_Sphere(const CMeshEffect_Sphere& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMeshEffect_Sphere::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CMeshEffect_Sphere::NativeConstruct(void* pArg)
{
	if (!pArg) // 무조건 이펙트를 생성할 위치
		assert(0);

	if (SetUp_Component())
		return E_FAIL;
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vector*)pArg);

	m_vSize = _float3(0.01f, 0.01f, 0.01f);
	return S_OK;
}

_int CMeshEffect_Sphere::Tick(_float fTimeDelta)
{
	if (m_isDead)
		return -1;

	// 공기팡 사이즈를 서서히 증가시키자
	fTimeDelta *= 20.f;
	XMStoreFloat3(&m_vSize, XMLoadFloat3(&m_vSize) + XMVectorSet(fTimeDelta, fTimeDelta, fTimeDelta, 0.f));
	m_pTransformCom->Set_Scale(m_vSize);

	return _int();
}

_int CMeshEffect_Sphere::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// AddRenderGroup
	bool AddRenderGroup = false;
	if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
		AddRenderGroup = true;

#ifdef USE_IMGUI
	// 디버깅이 필요하면 이거 넣어줘야한다
	if (m_bUseImGui)
	{
		AddRenderGroup = true;
	}
#endif

	if (AddRenderGroup)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHA, this)))
			assert(0);
	/*	if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
			assert(0); */
	}

	// 공기팡 사이즈가 5.0f 이상이 되면 죽자.
	if (m_vSize.x >= 6.f)
	{
		m_isDead = true;
	}

	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

HRESULT CMeshEffect_Sphere::Render(_uint iPassIndex)
{
	if (FAILED(SetUp_ConstantTable(iPassIndex)))
		return E_FAIL;

	/* 장치에 월드변환 행렬을 저장한다. */
	_uint	iNumMeshContainer = m_pModelCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainer; ++i)
	{
		//m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pModelCom->Render(i, 5); // 그냥 Distortion 만 하자.
	}

	// restore default states, as the SkyFX changes them in the effect file.
	m_pDeviceContext->RSSetState(0);
	m_pDeviceContext->OMSetDepthStencilState(0, 0);

	return S_OK;
}

HRESULT CMeshEffect_Sphere::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
#ifdef _DEBUG
	// Collider 
	__super::Render_Colliders();
#endif // _DEBUG

#ifdef USE_IMGUI
	if (m_bUseImGui)
	{
		CImguiManager::GetInstance()->Transform_Control(m_pTransformCom.Get(), m_CloneIdx, &m_bUseImGui);

		char buf[32];
		sprintf_s(buf, "Sphere##%d", m_CloneIdx);
		ImGui::Begin(buf);
		{

			if (ImGui::DragFloat("Size", &m_vSize.x, 0.01f))
			{
				m_vSize = _float3(m_vSize.x, m_vSize.x, m_vSize.x);
			}
		}
		ImGui::End();
	}
#endif

	return S_OK;
}


HRESULT CMeshEffect_Sphere::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 7.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(10.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)m_pTransformCom.GetAddressOf(), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)m_pRendererCom.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MeshEffect_Sphere"), TEXT("Com_Model"), (CComponent**)m_pModelCom.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Texture_Noise */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_noise"), TEXT("Com_Texture_Noise"), (CComponent**)m_pDistortionNoiseTextureCom.GetAddressOf())))
		return E_FAIL;
	return S_OK;
}

HRESULT CMeshEffect_Sphere::SetUp_ConstantTable(_uint iPassIndex)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Bind Transform
	m_pTransformCom->Bind_OnShader(m_pModelCom.Get(), "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom.Get(), "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom.Get(), "g_ProjMatrix");

	// Bind Noise Texture
	if (FAILED(m_pDistortionNoiseTextureCom->SetUp_OnShader(m_pModelCom.Get(), "g_NoiseTexture", 5))) // m_noiseTextureIdx
		return E_FAIL;

	_float tempAlpha = .0f;
	m_pModelCom->Set_RawValue("g_NoiseAlphaPwr", &tempAlpha, sizeof(_float));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CMeshEffect_Sphere* CMeshEffect_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMeshEffect_Sphere* pInstance = new CMeshEffect_Sphere(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CMesh_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CMeshEffect_Sphere::Clone(void* pArg)
{
	CMeshEffect_Sphere* pInstance = new CMeshEffect_Sphere(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CMeshEffect_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshEffect_Sphere::Free()
{
	__super::Free();
}


// ---------------------------------------------------
// CMeshEffect_Ring
// ---------------------------------------------------
CMeshEffect_Ring::CMeshEffect_Ring(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMeshEffect_Ring::CMeshEffect_Ring(const CMeshEffect_Ring& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMeshEffect_Ring::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CMeshEffect_Ring::NativeConstruct(void* pArg)
{
	if (!pArg) // 무조건 이펙트를 생성할 위치
		assert(0);

	if (SetUp_Component())
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vector*)pArg);
	m_vSize = _float3(1.f, 1.f, 1.f);
	return S_OK;
}

_int CMeshEffect_Ring::Tick(_float fTimeDelta)
{
	if (m_isDead)
		return -1;

	// 공기팡 사이즈를 서서히 증가시키자
	fTimeDelta *= 15.f;
	XMStoreFloat3(&m_vSize, XMLoadFloat3(&m_vSize) + XMVectorSet(fTimeDelta, fTimeDelta, fTimeDelta, 0.f));
	// 그러나 y축은 최대 제한크기를 두자. 
	if (m_vSize.y >= 1.5f)
		m_vSize.y = 1.5f;


	m_pTransformCom->Set_Scale(m_vSize);

	return _int();
}

_int CMeshEffect_Ring::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// AddRenderGroup
	bool AddRenderGroup = false;
	if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
		AddRenderGroup = true;

#ifdef USE_IMGUI
	// 디버깅이 필요하면 이거 넣어줘야한다
	if (m_bUseImGui)
	{
		AddRenderGroup = true;
	}
#endif

	if (AddRenderGroup)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHA, this)))
			assert(0);
		if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
			assert(0); 
	}

	// 공기팡 사이즈가 이상이 되면 죽자.
	if (m_vSize.x >= 30.f)
	{
		m_isDead = true;
	}

	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

HRESULT CMeshEffect_Ring::Render(_uint iPassIndex)
{
	if (FAILED(SetUp_ConstantTable(iPassIndex)))
		return E_FAIL;

	/* 장치에 월드변환 행렬을 저장한다. */
	_uint	iNumMeshContainer = m_pModelCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainer; ++i)
	{
		m_pModelCom->Render(i, 5); // 그냥 Distortion 만 하자.
	}

	// restore default states, as the SkyFX changes them in the effect file.
	m_pDeviceContext->RSSetState(0);
	m_pDeviceContext->OMSetDepthStencilState(0, 0);

	return S_OK;
}

HRESULT CMeshEffect_Ring::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
#ifdef _DEBUG
	// Collider 
	__super::Render_Colliders();
#endif // _DEBUG

#ifdef USE_IMGUI
	if (m_bUseImGui)
	{
		CImguiManager::GetInstance()->Transform_Control(m_pTransformCom.Get(), m_CloneIdx, &m_bUseImGui);

		char buf[32];
		sprintf_s(buf, "Ring##%d", m_CloneIdx);
		ImGui::Begin(buf);
		{
			if (ImGui::DragFloat("Size", &m_vSize.x, 0.01f))
			{
				m_vSize = _float3(m_vSize.x, m_vSize.x, m_vSize.x);
			}
		}
		ImGui::End();
	}
#endif

	return S_OK;
}


HRESULT CMeshEffect_Ring::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 7.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(10.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)m_pTransformCom.GetAddressOf(), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)m_pRendererCom.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MeshEffect_Ring"), TEXT("Com_Model"), (CComponent**)m_pModelCom.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Texture_Noise */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_noise"), TEXT("Com_Texture_Noise"), (CComponent**)m_pDistortionNoiseTextureCom.GetAddressOf())))
		return E_FAIL;

	/* For.Com_AABB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ColliderDesc.fRadius = 3.0f;
	ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
	__super::Add_Collider(&ColliderDesc, COL_WAR_WEAPON);

	return S_OK;
}

HRESULT CMeshEffect_Ring::SetUp_ConstantTable(_uint iPassIndex)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Bind Transform
	m_pTransformCom->Bind_OnShader(m_pModelCom.Get(), "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom.Get(), "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom.Get(), "g_ProjMatrix");

	// Bind Noise Texture
	if (FAILED(m_pDistortionNoiseTextureCom->SetUp_OnShader(m_pModelCom.Get(), "g_NoiseTexture", 5))) 
		return E_FAIL;

	_float tempAlpha = .0f;
	m_pModelCom->Set_RawValue("g_NoiseAlphaPwr", &tempAlpha, sizeof(_float));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CMeshEffect_Ring* CMeshEffect_Ring::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMeshEffect_Ring* pInstance = new CMeshEffect_Ring(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CMesh_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CMeshEffect_Ring::Clone(void* pArg)
{
	CMeshEffect_Ring* pInstance = new CMeshEffect_Ring(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CMeshEffect_Ring");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshEffect_Ring::Free()
{
	__super::Free();
}
