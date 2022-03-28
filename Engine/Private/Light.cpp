#include "..\Public\Light.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"


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
	if (LIGHTDESC::TYPE_DIRECTIONAL != m_LightDesc.eType)
		return S_OK;

	_uint			iPassIndex = 0;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (LIGHTDESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		LIGHTDESC		dirLightDesc = *pGameInstance->Get_LightDesc(0);
		DirectionalLight mDirLight;
		mDirLight.Ambient = dirLightDesc.vAmbient;
		mDirLight.Diffuse = dirLightDesc.vDiffuse;
		mDirLight.Specular = dirLightDesc.vSpecular;
		mDirLight.Direction = dirLightDesc.vDirection;
		pVIBuffer->Set_RawValue("g_DirLight", &mDirLight, sizeof(DirectionalLight));
		iPassIndex = 1; // Light_Direction
	}
	else
	{
		iPassIndex = 2; // Light_Point
		pVIBuffer->Set_RawValue("g_vLightPos", &_float4(m_LightDesc.vPosition, 1.f), sizeof(_float4));
		pVIBuffer->Set_RawValue("g_fRange", &m_LightDesc.fRadiuse, sizeof(_float));

	}
	RELEASE_INSTANCE(CGameInstance);
	pVIBuffer->Render(iPassIndex);
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
