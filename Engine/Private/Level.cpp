#include "..\public\Level.h"

CLevel::CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDeviceContext(pDeviceContext)
	, m_pDevice(pDevice)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

HRESULT CLevel::NativeConstruct()
{
	return S_OK;
}

_int CLevel::Tick(_float fTimeDelta)
{
	return _int();
}

HRESULT CLevel::Render()
{
	return S_OK;
}

HRESULT CLevel::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	return S_OK;
}

void CLevel::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
