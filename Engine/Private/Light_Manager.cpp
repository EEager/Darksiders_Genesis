#include "..\Public\Light_Manager.h"
#include "Light.h"
#include "VIBuffer_Rect.h"
#include "Target_Manager.h"
#include "PipeLine.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"

// "../Bin/ShaderFiles/Shader_Deferred.hlsl"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
}

LIGHTDESC* CLight_Manager::Get_LightDesc(_uint iIndex)
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
	m_tMtrlDesc.vMtrlSpecular = { 1.f, 1.f, 1.f, 20.f }; // w is pow ������
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

HRESULT CLight_Manager::Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc)
{
	CLight* pLight = CLight::Create(pDevice, pDeviceContext, LightDesc);

	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	return S_OK;
}

HRESULT CLight_Manager::Update(_float fTimeDelta)
{
	// War Ÿ������ ����
	static bool targetingOnce = false;
	if (!targetingOnce)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		auto pLayer_War = pGameInstance->Get_GameObject_CloneList(TEXT("Layer_War"));
		if (pLayer_War)
		{
			m_pTarget = pLayer_War->front();
			m_pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_ComponentPtr(L"Com_Transform"));
			targetingOnce = true;
		}
		RELEASE_INSTANCE(CGameInstance)
	}


	// ù��° Directional ������ ��ġ�� ������Ʈ�Ѵ�.
	if (!m_Lights.empty())
	{
		XMVECTOR lightDir = XMVector3Normalize(XMLoadFloat3(&m_Lights.front()->m_LightDesc.vDirection));

		// ���� ������ ���� Matrics �� �����Ѵ�
		// ���� Enviroment�׸��ڸ� ��������� ��ĵ��� �����Ѵ�.
		{


			// ��Ÿ��������
			XMVECTOR targetPos = XMVectorSet(502.f, 32.f - 80.f, 461.f + 40.f, 1.0f);
			XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

			// ����ġ�� ��ü���� ȯ�� �������� ��� 
			_float SceneRadius = 470.f;
			XMVECTOR lightPos = -2 * SceneRadius * lightDir + targetPos;

			// ���� �佺���̽� ��ȯ ���
			XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

			// Transform bounding sphere to light space.
			XMFLOAT3 sphereCenterLS;
			XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

			// Ortho frustum in light space encloses scene.
			float l = sphereCenterLS.x - SceneRadius;
			float b = sphereCenterLS.y - SceneRadius;
			float n = sphereCenterLS.z - SceneRadius;
			float r = sphereCenterLS.x + SceneRadius;
			float t = sphereCenterLS.y + SceneRadius;
			float f = sphereCenterLS.z + SceneRadius;
			XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

			// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
			XMMATRIX T(
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, -0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.0f, 1.0f);

			XMMATRIX S = V * P * T;

			XMStoreFloat4x4(&m_LightView_Env, V);
			XMStoreFloat4x4(&m_LightProj_Env, P);
			XMStoreFloat4x4(&m_ShadowTransform_Env, S);
		}

		// Object �׸��ڵ�������ϱ����� Ȯ�밡 �ʿ��ϴ� 
		{
			// ��Ÿ��������
			// ī�޶� ��� �ִ� Ÿ���� �������� ����Ѵ�.
			XMVECTOR targetPos;

			auto pCameraList = CObject_Manager::GetInstance()->Get_GameObject_CloneList(L"Layer_Camera");

			// ī�޶� �ִ� ��� ī�޶��� Ÿ�� �������� ��������.
			if (pCameraList->empty() == false) 
			{
				// ī�޶� Ÿ���� ������ ����.
				if (auto pCameraTarget = static_cast<CCamera*>(pCameraList->front())->Get_Target())
				{
					CTransform* pCameraTargetTransform = static_cast<CTransform*>(pCameraTarget->Get_ComponentPtr(L"Com_Transform"));
					targetPos = pCameraTargetTransform->Get_State(CTransform::STATE_POSITION);
				}
			}
			else // ī�޶� ���� ��� �÷��̾ ��������.
			{
				targetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
			}

			XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

			// ����ġ��� 
			//_float SceneRadius = 18.f;
			//_float SceneRadius = 36.f;
			_float SceneRadius = m_fSceneRadius;
			XMVECTOR lightPos = -2 * SceneRadius * lightDir + targetPos;

			// ���� �佺���̽� ��ȯ ���
			XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

			// Transform bounding sphere to light space.
			XMFLOAT3 sphereCenterLS;
			XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

			// Ortho frustum in light space encloses scene.
			float l = sphereCenterLS.x - SceneRadius;
			float b = sphereCenterLS.y - SceneRadius;
			float n = sphereCenterLS.z - SceneRadius;
			float r = sphereCenterLS.x + SceneRadius;
			float t = sphereCenterLS.y + SceneRadius;
			float f = sphereCenterLS.z + SceneRadius;
			XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

			// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
			XMMATRIX T(
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, -0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.0f, 1.0f);

			XMMATRIX S = V * P * T;

			XMStoreFloat4x4(&m_LightView_Objects, V);
			XMStoreFloat4x4(&m_LightProj_Objects, P);
			XMStoreFloat4x4(&m_ShadowTransform_Objects, S);
		}
	}

	return S_OK;
}

HRESULT CLight_Manager::Render()
{
	CTarget_Manager* pTarget_Manager = GET_INSTANCE(CTarget_Manager);

	// view��Ʈ ���� ���
	m_pVIBuffer->Set_RawValue("g_TransformMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)), sizeof(_float4x4));
	// �������� ��ȯ ���
	m_pVIBuffer->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OrthoMatrix)), sizeof(_float4x4));
	// �� ������Ʈ���� ������ Normal ���� �Ѱ�����
	m_pVIBuffer->Set_ShaderResourceView("g_NormalTexture", pTarget_Manager->Get_SRV(TEXT("Target_Normal")));
	// �� ������Ʈ���� ������ ���� ���� �Ѱ�����
	m_pVIBuffer->Set_ShaderResourceView("g_DepthTexture", pTarget_Manager->Get_SRV(TEXT("Target_Depth_Cur")));


	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);


	_matrix		ViewMatrixInverse = XMMatrixInverse(nullptr, pPipeLine->Get_Transform(CPipeLine::TS_VIEW));
	_matrix		ProjMatrixInverse = XMMatrixInverse(nullptr, pPipeLine->Get_Transform(CPipeLine::TS_PROJ));

	// �佺���̽� ��ȯ ����� 
	m_pVIBuffer->Set_RawValue("g_ViewMatrixInverse", &XMMatrixTranspose(ViewMatrixInverse), sizeof(_float4x4));
	// �������� ��ȯ �����
	m_pVIBuffer->Set_RawValue("g_ProjMatrixInverse", &XMMatrixTranspose(ProjMatrixInverse), sizeof(_float4x4));
	// ī�޶� ��ġ 
	m_pVIBuffer->Set_RawValue("g_vCamPosition", &pPipeLine->Get_CamPosition(), sizeof(_float4));

	RELEASE_INSTANCE(CPipeLine);

	// Bind Material
	m_pVIBuffer->Set_RawValue("g_Material", &m_tMtrlDesc, sizeof(MTRLDESC));

	// Bind for ShadowMap
	// 1)�������� ������ �׸��� ���̸� �Ѱ�����
	m_pVIBuffer->Set_ShaderResourceView("g_ShadowMap_Env", pTarget_Manager->Get_SRV(TEXT("Target_Shadow_Env")));
	// 2)�� ������Ʈ���� ������ �׸��� ���̸� �Ѱ�����
	m_pVIBuffer->Set_ShaderResourceView("g_ShadowMap_Objects", pTarget_Manager->Get_SRV(TEXT("Target_Shadow_Obj")));
	// 3)�׸�������� �Ѱ��־�, ������Ʈ���� �׸��ڸ� ����ֵ�������
		// ���� ���� �׸��� ��� 
	m_pVIBuffer->Set_RawValue("g_ShadowTransform_Env", &XMMatrixTranspose(XMLoadFloat4x4(&m_ShadowTransform_Env)), sizeof(_matrix));
		// ������Ʈ���� �׸��� ���
	m_pVIBuffer->Set_RawValue("g_ShadowTransform_Objects", &XMMatrixTranspose(XMLoadFloat4x4(&m_ShadowTransform_Objects)), sizeof(_matrix));

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

