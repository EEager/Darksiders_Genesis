#include "..\Public\Light.h"
#include "VIBuffer_Rect.h"


CLight::CLight(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CLight::NativeConstruct(const LIGHTDESC & LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

HRESULT CLight::Render(CVIBuffer_Rect* pVIBuffer)
{
	/*_uint			iPassIndex = 0;

	if (LIGHTDESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		iPassIndex = 1;
		pVIBuffer->Set_RawValue("g_vLightDir", &_float4(m_LightDesc.vDirection, 0.f), sizeof(_float4));
	}
	else
	{
		iPassIndex = 2;
		pVIBuffer->Set_RawValue("g_vLightPos", &_float4(m_LightDesc.vPosition, 1.f), sizeof(_float4));
		pVIBuffer->Set_RawValue("g_fRange", &m_LightDesc.fRadiuse, sizeof(_float));

	}

	pVIBuffer->Set_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4));
	pVIBuffer->Set_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4));
	pVIBuffer->Set_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4));

	pVIBuffer->Render(iPassIndex);*/

	return S_OK;
}


CLight * CLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	CLight*	pInstance = new CLight(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(LightDesc)))
	{
		MSG_BOX("Failed To Creating CLight");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
