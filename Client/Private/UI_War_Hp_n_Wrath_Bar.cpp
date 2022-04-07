#include "stdafx.h"
#include "..\public\UI_War_Hp_n_Wrath_Bar.h"

#include "GameInstance.h"


CUI_War_Hp_n_Wrath_Bar::CUI_War_Hp_n_Wrath_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CUI_War_Hp_n_Wrath_Bar::CUI_War_Hp_n_Wrath_Bar(const CUI_War_Hp_n_Wrath_Bar & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_War_Hp_n_Wrath_Bar::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CUI_War_Hp_n_Wrath_Bar::NativeConstruct(void * pArg)
{
	if (SetUp_Component())
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f)));	
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	return S_OK;
}

_int CUI_War_Hp_n_Wrath_Bar::Tick(_float fTimeDelta)
{
	return _int();
}

_int CUI_War_Hp_n_Wrath_Bar::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return 0;

	if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
		return 0;

	return _int();
}

HRESULT CUI_War_Hp_n_Wrath_Bar::Render(_uint iPassIndex)
{

	if (m_bNotRender)
		return S_OK;

	// 1. UI_War_Base 출력
	if (FAILED(SetUp_ConstantTable_Base()))
		return E_FAIL;
	m_pVIBufferCom->Render(1);

	// 2. UI_War_HpBar 출력
	if (FAILED(SetUp_ConstantTable_HpBar()))
		return E_FAIL;
	m_pVIBufferCom->Render(1);

	// 3. UI_War_WrathBar 출력
	if (FAILED(SetUp_ConstantTable_WrathBar()))
		return E_FAIL;
	m_pVIBufferCom->Render(1);

	return S_OK;
}

HRESULT CUI_War_Hp_n_Wrath_Bar::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	//ImSI
	return S_OK;

	wstring str = DXString::Format(L"100");

	const wchar_t* output = str.c_str();

	auto origin = DirectX::g_XMZero;

	_float2 tmpPos; 
	tmpPos = _float2(131.f, 45.f); // 처음 위치
	XMVECTOR m_fontPos = XMLoadFloat2(&tmpPos);

	// Outline Effect
	tmpPos = _float2(1.f, 1.f);
	m_spriteFont->DrawString(m_spriteBatch.get(), output,
		m_fontPos + XMLoadFloat2(&tmpPos), Colors::Black, 0.f, origin);
	tmpPos = _float2(-1.f, 1.f);
	m_spriteFont->DrawString(m_spriteBatch.get(), output,
		m_fontPos + XMLoadFloat2(&tmpPos), Colors::Black, 0.f, origin);
	tmpPos = _float2(-1.f, -1.f);
	m_spriteFont->DrawString(m_spriteBatch.get(), output,
		m_fontPos + XMLoadFloat2(&tmpPos), Colors::Black, 0.f, origin);
	tmpPos = _float2(1.f, -1.f);
	m_spriteFont->DrawString(m_spriteBatch.get(), output,
		m_fontPos + XMLoadFloat2(&tmpPos), Colors::Black, 0.f, origin);

	// Origin Text
	m_spriteFont->DrawString(m_spriteBatch.get(), output,
		m_fontPos, Colors::White, 0.f, origin);

	return S_OK;
}


HRESULT CUI_War_Hp_n_Wrath_Bar::SetUp_Component()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture_Base*/ 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_War_Base"), TEXT("Com_Texture_Base"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Texture_HpBar*/ 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_War_HpBar"), TEXT("Com_Texture_HpBar"), (CComponent**)m_pTextureCom_HpBar.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Texture_WrathBar*/ 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_War_WrathBar"), TEXT("Com_Texture_WrathBar"), (CComponent**)m_pTextureCom_WrathBar.GetAddressOf())))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_War_Hp_n_Wrath_Bar::SetUp_ConstantTable_Base()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = 874.f / 1.5f;
	WorldMatrix._22 = 228.f / 1.5f;
	WorldMatrix._41 = 250.f - (g_iWinCX >> 1);
	WorldMatrix._42 = -90.f + (g_iWinCY >> 1);
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture")))
		return E_FAIL;	

	return S_OK;
}

HRESULT CUI_War_Hp_n_Wrath_Bar::SetUp_ConstantTable_HpBar()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4x4		WorldMatrix;

	// same position as SetUp_ConstantTable_Base
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = 874.f / 1.5f;
	WorldMatrix._22 = 228.f / 1.5f;
	WorldMatrix._41 = 250.f - (g_iWinCX >> 1);
	WorldMatrix._42 = -90.f + (g_iWinCY >> 1);
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom_HpBar->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_War_Hp_n_Wrath_Bar::SetUp_ConstantTable_WrathBar()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	_float fSizeX = 114.f/ 1.5f;
	_float fSizeY = 39.f / 1.5f;
	_float fPosX = 158.f;
	_float fPosY = 97.f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = fSizeX;
	WorldMatrix._22 = fSizeY;
	WorldMatrix._41 = fPosX - (g_iWinCX >> 1);
	WorldMatrix._42 = -fPosY + (g_iWinCY >> 1);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom_WrathBar->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

CUI_War_Hp_n_Wrath_Bar * CUI_War_Hp_n_Wrath_Bar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUI_War_Hp_n_Wrath_Bar*		pInstance = new CUI_War_Hp_n_Wrath_Bar(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_War_Hp_n_Wrath_Bar");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CUI_War_Hp_n_Wrath_Bar::Clone(void* pArg)
{
	CUI_War_Hp_n_Wrath_Bar*		pInstance = new CUI_War_Hp_n_Wrath_Bar(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CUI_War_Hp_n_Wrath_Bar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_War_Hp_n_Wrath_Bar::Free()
{

	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
