#include "stdafx.h"
#include "..\public\Loading_BackGround.h"

#include "GameInstance.h"


CLoading_BackGround::CLoading_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CLoading_BackGround::CLoading_BackGround(const CLoading_BackGround & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLoading_BackGround::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CLoading_BackGround::NativeConstruct(void * pArg)
{
	if (SetUp_Component())
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f)));	

	// 로딩 배경 크기, 위치
	m_fBGSizeX = g_iWinCX;
	m_fBGSizeY = g_iWinCY;

	m_fBGX = g_iWinCX >> 1;
	m_fBGY = g_iWinCY >> 1;

	// 로딩 원 크기, 위치
	m_fCircleSizeX = 128.f;
	m_fCircleSizeY = 128.f;

	m_fCircleX = 648.f;
	m_fCircleY = 761.f;

	XMStoreFloat4x4(&m_matCircle, XMMatrixIdentity());
	m_matCircle._11 = m_fCircleSizeX;
	m_matCircle._22 = m_fCircleSizeY;
	m_matCircle._41 = m_fCircleX - (g_iWinCX >> 1);
	m_matCircle._42 = -m_fCircleY + (g_iWinCY >> 1);

	return S_OK;
}

_int CLoading_BackGround::Tick(_float fTimeDelta)
{
	Rotate_CircleZ(fTimeDelta);
	return _int();
}

_int CLoading_BackGround::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return 0;

	return _int();
}

HRESULT CLoading_BackGround::Render()
{
	// 1. Loading_Background 출력
	if (FAILED(SetUp_ConstantTable_BackGround()))
		return E_FAIL;

	m_pVIBufferCom->Render(0);

	// 2. Loading_Circle 출력
	if (FAILED(SetUp_ConstantTable_LoadingCircle()))
		return E_FAIL;

	m_pVIBufferCom->Render(0);

	return S_OK;
}

HRESULT CLoading_BackGround::SetUp_Component()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	
	/* For.Com_Texture_BackGround*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, 
		TEXT("Prototype_Component_Texture_Loading_BackGround"), TEXT("Com_Texture_BackGround"), (CComponent**)&m_pTextureCom_BackGround)))
		return E_FAIL;

	/* For.Com_Texture_LoadingCircle*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, 
		TEXT("Prototype_Component_Texture_Loading_Circle"), TEXT("Com_Texture_LoadingCircle"), (CComponent**)&m_pTextureCom_LoadingCircle)))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CLoading_BackGround::SetUp_ConstantTable_BackGround()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4x4		WorldMatrix, ViewMatrix;
	
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = 1024;
	WorldMatrix._22 = 1024;

	// x : ~(g_iWinCX/2) ~ (g_iWinCX/2)
	// y : ~(-g_iWinCY/2) ~ -(g_iWinCY/2)
	WorldMatrix._41 = m_fBGX - (g_iWinCX >> 1); 
	WorldMatrix._42 = -m_fBGY + (g_iWinCY >> 1); 


	/* Float4x4 -> XMMatrix  : Load */
	/* XMMatrix -> Float4x4 : Store */
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));

	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());		

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom_BackGround->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture")))
		return E_FAIL;	

	return S_OK;
}



void CLoading_BackGround::Rotate_CircleZ(_float fTimeDelta)
{
	_matrix		RotationMatrix;

	RotationMatrix = XMMatrixRotationAxis(XMLoadFloat4(&_float4(0.f, 0.f, 1.f,0.f)), .4f * fTimeDelta);

	_vector		vRight = XMLoadFloat4((_float4*)&m_matCircle.m[CTransform::STATE_RIGHT]); // right
	_vector		vUp = XMLoadFloat4((_float4*)&m_matCircle.m[CTransform::STATE_UP]); // up 
	_vector		vLook = XMLoadFloat4((_float4*)&m_matCircle.m[CTransform::STATE_LOOK]); // look

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_Circle_State(CTransform::STATE_RIGHT, vRight);
	Set_Circle_State(CTransform::STATE_UP, vUp);
	Set_Circle_State(CTransform::STATE_LOOK, vLook);
}

HRESULT CLoading_BackGround::SetUp_ConstantTable_LoadingCircle()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	/* Float4x4 -> XMMatrix  : Load */
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_matCircle)));

	_float4x4		ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom_LoadingCircle->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

void CLoading_BackGround::Set_Circle_State(int state, _fvector vState)
{
	_float4		vTmp;
	XMStoreFloat4(&vTmp, vState);
	memcpy(&m_matCircle.m[state], &vTmp, sizeof(_float4));
}

CLoading_BackGround * CLoading_BackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLoading_BackGround*		pInstance = new CLoading_BackGround(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CLoading_BackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CLoading_BackGround::Clone(void* pArg)
{
	CLoading_BackGround*		pInstance = new CLoading_BackGround(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CLoading_BackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading_BackGround::Free()
{

	__super::Free();

	Safe_Release(m_pTextureCom_BackGround);
	Safe_Release(m_pTextureCom_LoadingCircle);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
