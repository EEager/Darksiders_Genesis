#include "stdafx.h"
#include "..\public\Logo_BackGround.h"

#include "GameInstance.h"


CLogo_BackGround::CLogo_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CLogo_BackGround::CLogo_BackGround(const CLogo_BackGround & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLogo_BackGround::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CLogo_BackGround::NativeConstruct(void * pArg)
{
	if (SetUp_Component())
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f)));	

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;

	m_fX = g_iWinCX >> 1;
	m_fY = g_iWinCY >> 1;

	return S_OK;
}

_int CLogo_BackGround::Tick(_float fTimeDelta)
{
	return _int();
}

_int CLogo_BackGround::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return 0;

	if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
		return 0;

	return _int();
}

HRESULT CLogo_BackGround::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	/* 장치에 월드변환 행렬을 저장한다. */

	m_pVIBufferCom->Render(0);

	return S_OK;
}

HRESULT CLogo_BackGround::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	//#ifdef USE_IMGUI
	//	wstring str = DXString::Format(L"FPS : %.0f", ImGui::GetIO().Framerate);
	//#else
	//	wstring str = DXString::Format(L"Font Test");
	//#endif
	wstring str = DXString::Format(L"Press Any Key To Start");

	const wchar_t* output = str.c_str();

	//auto origin = m_spriteFont->MeasureString(output) / 2.f;
	auto origin = DirectX::g_XMZero;

	_float2 tmpPos;
	// Font Position
	tmpPos = _float2(1070.f, 600.f);
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


HRESULT CLogo_BackGround::SetUp_Component()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	
	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Logo"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	

	return S_OK;
}

HRESULT CLogo_BackGround::SetUp_ConstantTable()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4x4		WorldMatrix, ViewMatrix;
	
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fSizeX;
	WorldMatrix._22 = m_fSizeY;
	WorldMatrix._41 = m_fX - (g_iWinCX >> 1);
	WorldMatrix._42 = -m_fY + (g_iWinCY >> 1);

	/* Float4x4 -> XMMatrix  : Load */
	/* XMMatrix -> Float4x4 : Store */
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));

	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());		

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture")))
		return E_FAIL;	

	return S_OK;
}

CLogo_BackGround * CLogo_BackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLogo_BackGround*		pInstance = new CLogo_BackGround(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CLogo_BackGround::Clone(void* pArg)
{
	CLogo_BackGround*		pInstance = new CLogo_BackGround(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogo_BackGround::Free()
{

	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
