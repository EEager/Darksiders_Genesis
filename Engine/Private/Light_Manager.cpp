#include "..\Public\Light_Manager.h"
#include "Light.h"

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

HRESULT CLight_Manager::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	CLight*			pLight = CLight::Create(pDevice, pDeviceContext, LightDesc);

	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	return S_OK;
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}

