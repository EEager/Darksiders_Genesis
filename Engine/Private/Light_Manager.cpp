#include "..\Public\Light_Manager.h"
#include "Light.h"
#include "VIBuffer_Rect.h"
#include "Target_Manager.h"

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

HRESULT CLight_Manager::Render()
{
	CTarget_Manager* pTarget_Manager = GET_INSTANCE(CTarget_Manager);

	m_pVIBuffer->Set_RawValue("g_TransformMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OrthoMatrix)), sizeof(_float4x4));
	m_pVIBuffer->Set_ShaderResourceView("g_NormalTexture", pTarget_Manager->Get_SRV(TEXT("Target_Normal")));

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

