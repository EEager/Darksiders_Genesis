#include "..\Public\Light_Manager.h"
#include "Light.h"
#include "VIBuffer_Rect.h"
#include "Target_Manager.h"
#include "PipeLine.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Transform.h"

// "../Bin/ShaderFiles/Shader_Deferred.hlsl"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
}

LIGHTDESC * CLight_Manager::Get_LightDesc(_uint iIndex)
{
	auto	iter = m_Lights.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;	
	if (iter == m_Lights.end())
		return nullptr;

	return (*iter)->Get_LightDesc();
}

void CLight_Manager::Set_LightDesc(_uint iIndex, const LIGHTDESC& LightDesc)
{
	auto	iter = m_Lights.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Set_LightDesc(LightDesc);
}

HRESULT CLight_Manager::NativeConstruct(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	// Material Init
	m_tMtrlDesc.vMtrlDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_tMtrlDesc.vMtrlAmbient = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_tMtrlDesc.vMtrlSpecular = { 1.f, 1.f, 1.f, 20.f }; // w is pow 지수값
	m_tMtrlDesc.vMtrlEmissive = { 1.f, 1.f, 1.f, 1.f };
	m_tMtrlDesc.fMtrlPower = 20.f;


	m_pVIBuffer = CVIBuffer_Rect::Create(pDevice, pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"));
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	D3D11_VIEWPORT		ViewportDesc;

	_uint			iNumViewport = 1;

	pDeviceContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());
	m_TransformMatrix._11 = ViewportDesc.Width;
	m_TransformMatrix._22 = ViewportDesc.Height;
	m_TransformMatrix._41 = 0.0f;
	m_TransformMatrix._42 = 0.0f;

	XMStoreFloat4x4(&m_OrthoMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f)));

	return S_OK;
}

HRESULT CLight_Manager::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	CLight*			pLight = CLight::Create(pDevice, pDeviceContext, LightDesc);

	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	return S_OK;
}
CGameObject* m_pTarget;
CTransform* m_pTargetTransform;
HRESULT CLight_Manager::Render()
{
	if (!m_Lights.empty())
	{
		// 타겟팅 설정하자
		static bool targetingOnce = false;
		if (!targetingOnce)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			auto pLayer_War = pGameInstance->Get_GameObject_CloneList(TEXT("Layer_War"));
			if (pLayer_War)
			{
				m_pTarget = pLayer_War->front();
				//Safe_AddRef(m_pTarget);
				m_pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_ComponentPtr(L"Com_Transform"));
				targetingOnce = true;
			}
			RELEASE_INSTANCE(CGameInstance)
		}

		// Only the first "main" light casts a shadow.
		XMVECTOR lightDir = XMVector3Normalize(XMLoadFloat3(&m_Lights.front()->m_LightDesc.vDirection));

		// 아래 정보는 전체 Scene 기준으로 변수를 설정하였음
		// 빛타겟포지션
		//XMVECTOR targetPos = XMVectorSet(502.f, 32.f-80.f, 461.f+40.f, 1.0f);
		XMVECTOR targetPos = m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION) + XMVectorSet(0.f, 0.f, 0.f, 1.f);
		XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		// 빛위치
		_float SceneRadius = 20.f;
		XMVECTOR lightPos = SceneRadius * -lightDir + targetPos;
		  
		// 빛의 뷰스페이스 변환 행렬
		XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

		// Transform bounding sphere to light space.
		XMFLOAT3 sphereCenterLS;
		XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));
		
		
		//SceneRadius = 10.f;

		// Ortho frustum in light space encloses scene.
		float l = sphereCenterLS.x - SceneRadius;
		float r = sphereCenterLS.x + SceneRadius;

		float b = sphereCenterLS.y - SceneRadius;
		float t = sphereCenterLS.y + SceneRadius;

		float n = sphereCenterLS.z - SceneRadius;
		//sphereCenterLS.z - SceneRadius;
		float f = sphereCenterLS.z + SceneRadius;
		XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

		// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
		XMMATRIX T(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f);

		XMMATRIX S = V * P * T;

		XMStoreFloat4x4(&m_LightView, V);
		XMStoreFloat4x4(&m_LightProj, P);
		XMStoreFloat4x4(&m_ShadowTransform, S);
	}


	CTarget_Manager* pTarget_Manager = GET_INSTANCE(CTarget_Manager);

	// view포트 정보 행렬
	m_pVIBuffer->Set_RawValue("g_TransformMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)), sizeof(_float4x4));
	// 프로젝션 변환 행렬
	m_pVIBuffer->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OrthoMatrix)), sizeof(_float4x4));
	// 각 오브젝트들이 저장한 Normal 값을 넘겨주자
	m_pVIBuffer->Set_ShaderResourceView("g_NormalTexture", pTarget_Manager->Get_SRV(TEXT("Target_Normal")));
	// 각 오브젝트들이 저장한 깊이 값을 넘겨주자
	m_pVIBuffer->Set_ShaderResourceView("g_DepthTexture", pTarget_Manager->Get_SRV(TEXT("Target_Depth_Cur")));


	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);


	_matrix		ViewMatrixInverse = XMMatrixInverse(nullptr, pPipeLine->Get_Transform(CPipeLine::TS_VIEW));
	_matrix		ProjMatrixInverse = XMMatrixInverse(nullptr, pPipeLine->Get_Transform(CPipeLine::TS_PROJ));

	// 뷰스페이스 변환 역행렬 
	m_pVIBuffer->Set_RawValue("g_ViewMatrixInverse", &XMMatrixTranspose(ViewMatrixInverse), sizeof(_float4x4));
	// 프로젝션 변환 역행렬
	m_pVIBuffer->Set_RawValue("g_ProjMatrixInverse", &XMMatrixTranspose(ProjMatrixInverse), sizeof(_float4x4));
	// 카메라 위치 
	m_pVIBuffer->Set_RawValue("g_vCamPosition", &pPipeLine->Get_CamPosition(), sizeof(_float4));

	RELEASE_INSTANCE(CPipeLine);

	// Bind Material
	m_pVIBuffer->Set_RawValue("g_Material", &m_tMtrlDesc, sizeof(MTRLDESC));

	// Bind for ShadowMap
	// 각 오브젝트들이 저장한 그림자 깊이를 넘겨주자
	m_pVIBuffer->Set_ShaderResourceView("g_ShadowMap", pTarget_Manager->Get_SRV(TEXT("Target_Shadow")));
	// 그림자행렬을 넘겨주어, 오브젝트들의 그림자를 찍어주도록하자
	m_pVIBuffer->Set_RawValue("g_ShadowTransform", &XMMatrixTranspose(XMLoadFloat4x4(&m_ShadowTransform)), sizeof(_matrix));

	for (auto& pLight : m_Lights)
	{
		pLight->Render(m_pVIBuffer);
	}

	RELEASE_INSTANCE(CTarget_Manager);

	return S_OK;
}


void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
	Safe_Release(m_pVIBuffer);
}

