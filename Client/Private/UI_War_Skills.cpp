#include "stdafx.h"
#include "..\public\UI_War_Skills.h"

#include "GameInstance.h"


CUI_War_Skills::CUI_War_Skills(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CUI_War_Skills::CUI_War_Skills(const CUI_War_Skills & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_War_Skills::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CUI_War_Skills::NativeConstruct(void * pArg)
{
	if (SetUp_Component())
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f)));	
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	return S_OK;
}

_int CUI_War_Skills::Tick(_float fTimeDelta)
{
	return _int();
}

_int CUI_War_Skills::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return 0;

	if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
		return 0;

	return _int();
}

HRESULT CUI_War_Skills::Render()
{
	// 1. UI_Skill_Base 출력
	if (FAILED(SetUp_ConstantTable_Base()))
		return E_FAIL;
	m_pVIBufferCom->Render(0);

	// 2. WrathPower 출력
	if (FAILED(SetUp_ConstantTable_WrathPower()))
		return E_FAIL;
	m_pVIBufferCom->Render(0);

	// 버튼1
	if (FAILED(SetUp_ConstantTable_Bnt1()))
		return E_FAIL;
	m_pVIBufferCom->Render(0);

	// 3. Enhancement 출력
	if (FAILED(SetUp_ConstantTable_Enhancement(m_pTextureCom_Fire.Get())))
		return E_FAIL;
	m_pVIBufferCom->Render(0);

	// 버튼G
	if (FAILED(SetUp_ConstantTable_BntG()))
		return E_FAIL;
	m_pVIBufferCom->Render(0);

	return S_OK;
}

HRESULT CUI_War_Skills::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	//wstring str = DXString::Format(L"100");

	//const wchar_t* output = str.c_str();

	//auto origin = DirectX::g_XMZero;

	//_float2 tmpPos;
	//// Font Position
	//tmpPos = _float2(148, 57.f);
	//XMVECTOR m_fontPos = XMLoadFloat2(&tmpPos);

	//// Outline Effect
	//tmpPos = _float2(1.f, 1.f);
	//m_spriteFont->DrawString(m_spriteBatch.get(), output,
	//	m_fontPos + XMLoadFloat2(&tmpPos), Colors::Black, 0.f, origin);
	//tmpPos = _float2(-1.f, 1.f);
	//m_spriteFont->DrawString(m_spriteBatch.get(), output,
	//	m_fontPos + XMLoadFloat2(&tmpPos), Colors::Black, 0.f, origin);
	//tmpPos = _float2(-1.f, -1.f);
	//m_spriteFont->DrawString(m_spriteBatch.get(), output,
	//	m_fontPos + XMLoadFloat2(&tmpPos), Colors::Black, 0.f, origin);
	//tmpPos = _float2(1.f, -1.f);
	//m_spriteFont->DrawString(m_spriteBatch.get(), output,
	//	m_fontPos + XMLoadFloat2(&tmpPos), Colors::Black, 0.f, origin);

	//// Origin Text
	//m_spriteFont->DrawString(m_spriteBatch.get(), output,
	//	m_fontPos, Colors::White, 0.f, origin);

	return S_OK;
}


HRESULT CUI_War_Skills::SetUp_Component()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	// Notice : LEVEL_GAMEPLAY
	/* For.Com_Texture_Base*/ 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Skill_Base"), TEXT("Com_Texture_Base"), (CComponent**)m_pTextureCom_SkillBase.GetAddressOf())))
		return E_FAIL;
	/* For.Com_Texture_WrathPower*/	
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_WrathPower_War"), TEXT("Com_Texture_WrathPower"), (CComponent**)m_pTextureCom_WrathPower.GetAddressOf())))
		return E_FAIL;
	/* For.Com_Texture_Fire*/ 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Enhancement_War_Fire"), TEXT("Com_Texture_Fire"), (CComponent**)m_pTextureCom_Fire.GetAddressOf())))
		return E_FAIL;
	/* For.Com_Texture_Leaf*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Enhancement_War_Leaf"), TEXT("Com_Texture_Leaf"), (CComponent**)m_pTextureCom_Leaf.GetAddressOf())))
		return E_FAIL;
	/* For.Com_Texture_Bnt_1*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UIButtonBase_1"), TEXT("Com_Texture_Bnt_1"), (CComponent**)m_pTextureCom_Button_1.GetAddressOf())))
		return E_FAIL;
	/* For.Com_Texture_Bnt_G*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UIButtonBase_G"), TEXT("Com_Texture_Bnt_G"), (CComponent**)m_pTextureCom_Button_G.GetAddressOf())))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_War_Skills::SetUp_ConstantTable_Base()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = 465.f/2.f;
	WorldMatrix._22 = 288.f/2.f;
	WorldMatrix._41 = 197.f - (g_iWinCX >> 1);
	WorldMatrix._42 = -663.f + (g_iWinCY >> 1);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom_SkillBase->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture")))
		return E_FAIL;	

	return S_OK;
}

HRESULT CUI_War_Skills::SetUp_ConstantTable_WrathPower()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = 256.f/2.25f;
	WorldMatrix._22 = 256.f/2.25f;
	WorldMatrix._41 = 134.f - (g_iWinCX >> 1);
	WorldMatrix._42 = -664.f + (g_iWinCY >> 1);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom_WrathPower->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_War_Skills::SetUp_ConstantTable_Enhancement(CTexture* pEnhance_Texture)
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = 512.f/4.5f;
	WorldMatrix._22 = 512.f/4.5f;
	WorldMatrix._41 = 134.f+134.f - (g_iWinCX >> 1);
	WorldMatrix._42 = -664.f + (g_iWinCY >> 1);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(pEnhance_Texture->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}


HRESULT CUI_War_Skills::SetUp_ConstantTable_Bnt1()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = 128.f / 2.f;
	WorldMatrix._22 = 128.f / 2.f;
	WorldMatrix._41 = 134.f - (g_iWinCX >> 1);
	WorldMatrix._42 = -(664.f + 60.f) + (g_iWinCY >> 1);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom_Button_1->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_War_Skills::SetUp_ConstantTable_BntG()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = 128.f / 2.f;
	WorldMatrix._22 = 128.f / 2.f;
	WorldMatrix._41 = 134.f + 134.f - (g_iWinCX >> 1);
	WorldMatrix._42 = -(664.f + 60.f) + (g_iWinCY >> 1);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom_Button_G->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

CUI_War_Skills * CUI_War_Skills::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUI_War_Skills*		pInstance = new CUI_War_Skills(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_War_Skills");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CUI_War_Skills::Clone(void* pArg)
{
	CUI_War_Skills*		pInstance = new CUI_War_Skills(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CUI_War_Skills");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_War_Skills::Free()
{

	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
