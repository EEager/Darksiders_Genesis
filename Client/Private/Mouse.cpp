#include "stdafx.h"
#include "..\public\Mouse.h"

#include "GameInstance.h"


CMouse::CMouse(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMouse::CMouse(const CMouse & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMouse::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CMouse::NativeConstruct(void * pArg)
{
	if (SetUp_Component())
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f)));	

	m_vSize.x = 256/3.2f;
	m_vSize.y = 256/3.2f;

	m_vPos.x = g_iWinCX >> 1;
	m_vPos.y = g_iWinCY >> 1;

	return S_OK;
}

_int CMouse::Tick(_float fTimeDelta)
{
	POINT pt;
	::GetCursorPos(&pt);
	::ScreenToClient(g_hWnd, &pt);
	m_vPos.x = (_float)pt.x;
	m_vPos.y = (_float)pt.y;

	return _int();
}

_int CMouse::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MOUSE, this)))
		return 0;

	return _int();
}

HRESULT CMouse::Render(_uint iPassIndex)
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pVIBufferCom->Render(iPassIndex);

	return S_OK;
}

HRESULT CMouse::SetUp_Component()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	
	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mouse"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CMouse::SetUp_ConstantTable()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4x4		WorldMatrix, ViewMatrix;
	
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_vSize.x;
	WorldMatrix._22 = m_vSize.y;
	WorldMatrix._41 = m_vPos.x - (g_iWinCX >> 1);
	WorldMatrix._42 = -m_vPos.y + (g_iWinCY >> 1);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());		
	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture")))
		return E_FAIL;	

	return S_OK;
}

CMouse * CMouse::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMouse*		pInstance = new CMouse(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMouse::Clone(void* pArg)
{
	CMouse*		pInstance = new CMouse(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMouse::Free()
{

	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
