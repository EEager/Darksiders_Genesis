#include "stdafx.h"
#include "..\public\BackGround.h"

#include "GameInstance.h"


CBackGround::CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBackGround::CBackGround(const CBackGround & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBackGround::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CBackGround::NativeConstruct(void * pArg)
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

_int CBackGround::Tick(_float fTimeDelta)
{
	return _int();
}

_int CBackGround::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return 0;

	// D3D11_SAMPLER_DESC

	return _int();
}

HRESULT CBackGround::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	/* 장치에 월드변환 행렬을 저장한다. */

	m_pVIBufferCom->Render(0);

	return S_OK;
}

HRESULT CBackGround::SetUp_Component()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	
	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Default"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	

	return S_OK;
}

HRESULT CBackGround::SetUp_ConstantTable()
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

CBackGround * CBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBackGround*		pInstance = new CBackGround(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBackGround::Clone(void* pArg)
{
	CBackGround*		pInstance = new CBackGround(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGround::Free()
{

	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
