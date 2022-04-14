#include "stdafx.h"
#include "..\public\UI_HollowLord_HpBar.h"

#include "GameInstance.h"

#ifdef USE_IMGUI
#include "imgui_Manager.h"
#endif


CUI_HollowLord_HpBar::CUI_HollowLord_HpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CUI_HollowLord_HpBar::CUI_HollowLord_HpBar(const CUI_HollowLord_HpBar & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_HollowLord_HpBar::NativeConstruct_Prototype()
{	
	if (SetUp_Component())
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f)));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CUI_HollowLord_HpBar::NativeConstruct(void * pArg)
{
	return S_OK;
}

_int CUI_HollowLord_HpBar::Tick(_float fTimeDelta)
{
	if (m_bInit == false)
		return 0;

	m_fAlpha += 0.005f;
	if (m_fAlpha >= 1.f)
		m_fAlpha = 1.f;

	return _int();
}

_int CUI_HollowLord_HpBar::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	
	
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return 0;

	// Hp 바 위에 이름 출력하자.
	if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
		return 0;

	return _int();
}

HRESULT CUI_HollowLord_HpBar::Render(_uint iPassIndex)
{
	// 초상화 출력 
	if (FAILED(SetUp_ConstantTable_Portrait()))
		return E_FAIL;
	m_pVIBufferCom->Render(2); // ZIgnoreNAlphablending_AlphaControl
	// Hp Bar 출력
	if (FAILED(SetUp_ConstantTable_HpBar()))
		return E_FAIL;
	m_pVIBufferCom->Render(4); // ZIgnoreNAlphablending_HollowLord



	return S_OK;
}

HRESULT CUI_HollowLord_HpBar::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	if (m_bInit == false)
		return 0;

	//ImSI
	wstring str = DXString::Format(L"Hollow Lord");

	const wchar_t* output = str.c_str();

	auto origin = DirectX::g_XMZero;

	_float2 tmpPos; 
	tmpPos = _float2(m_fText_X, m_fText_Y); //  위치
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

#ifdef USE_IMGUI
	if (m_pHollowLord->m_bUseImGui) // IMGUI 툴로 배치할거다
	{
		ImGui::Begin("Hollow Lord UI");
		{
			ImGui::DragFloat("Hp_PosX", &m_fHpBar_X);
			ImGui::DragFloat("Hp_PosY", &m_fHpBar_Y);
			ImGui::DragFloat("Portrait_PosX", &m_fPortrait_X);
			ImGui::DragFloat("Portrait_PosY", &m_fPortrait_Y);
			ImGui::DragFloat("m_fText_X", &m_fText_X);
			ImGui::DragFloat("m_fText_Y", &m_fText_Y);
		}
		ImGui::End();
	}
#endif

	return S_OK;
}


HRESULT CUI_HollowLord_HpBar::SetUp_Component()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_CreatureCore_Portraits_hollowLord_major*/ 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_CreatureCore_Portraits_hollowLord_major"), TEXT("Com_Texture_WrathBar"), (CComponent**)m_pTextureCom_Portrait.GetAddressOf())))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_HUD_BossHealthFrame1*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_HUD_BossHealthFrame1"), TEXT("Com_Texture_HpBar"), (CComponent**)m_pTextureCom_HpBar.GetAddressOf())))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_HollowLord_HpBar::SetUp_ConstantTable_HpBar()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;
	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = 256.f *1.7f;
	WorldMatrix._22 = 32.f * 1.7f;
	WorldMatrix._41 = m_fHpBar_X - (g_iWinCX >> 1);
	WorldMatrix._42 = -m_fHpBar_Y + (g_iWinCY >> 1);
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom_HpBar->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture")))
		return E_FAIL;

	// 알파
	m_pVIBufferCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(float));

	// 0~1 UV X좌표중 현재 체력의 UV X좌표를 넘겨주자.
	_float curUVX = (_float)m_pHollowLord->m_tGameInfo.iHp / (_float)m_pHollowLord->m_tGameInfo.iMaxHp;
	m_pVIBufferCom->Set_RawValue("g_HollowLordCurHpUVX", &curUVX, sizeof(float));

	return S_OK;
}

HRESULT CUI_HollowLord_HpBar::SetUp_ConstantTable_Portrait()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._22 = 256.f/1.2f;
	WorldMatrix._11 = 256.f/1.2f;
	WorldMatrix._41 = m_fPortrait_X - (g_iWinCX >> 1);
	WorldMatrix._42 = -m_fPortrait_Y + (g_iWinCY >> 1);
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom_Portrait->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture")))
		return E_FAIL;

	// 알파
	m_pVIBufferCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(float));

	return S_OK;
}

CUI_HollowLord_HpBar * CUI_HollowLord_HpBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUI_HollowLord_HpBar*		pInstance = new CUI_HollowLord_HpBar(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_HollowLord_HpBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CUI_HollowLord_HpBar::Clone(void* pArg)
{
	return nullptr;
}

void CUI_HollowLord_HpBar::Free()
{

	__super::Free();
	Safe_Release(m_pHollowLord);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
