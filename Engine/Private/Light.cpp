#include "..\Public\Light.h"


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
