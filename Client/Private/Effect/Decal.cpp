#include "stdafx.h"
#include "..\public\Effect\Decal.h"

#include "GameInstance.h"


CDecal::CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CDecal::CDecal(const CDecal & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDecal::NativeConstruct_Prototype()
{	
	return S_OK;
}

HRESULT CDecal::NativeConstruct(void * pArg)
{
	// pArg는 위치일것이다.
	if (SetUp_Component())
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vector*)pArg);
	m_pTransformCom->Set_Scale(_float3(6.f, 6.f, 6.f));

	// xy로 서있는 렉트이므로 눕혀주자.
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	
	return S_OK;
}

_int CDecal::Tick(_float fTimeDelta)
{
	if (m_isDead)
		return -1;

	return _int();
}

_int CDecal::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	m_fDissolvePower += fTimeDelta / 2.f;
	if (m_fDissolvePower >= 1.f) // 1이면 다 사라졌다. 이때는 진짜로 죽이자.
	{
		m_isDead = true;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHA, this);

	return _int();
}

HRESULT CDecal::Render(_uint iPassIndex) 
{
	iPassIndex = 7;  
	if (FAILED(SetUp_ConstantTable())) 
		return E_FAIL;

	m_pModelCom->Render(iPassIndex);

	return S_OK;
}

HRESULT CDecal::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)m_pRendererCom.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Decal"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Texture_Disolve */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTextureCom)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CDecal::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);	

	m_pTransformCom->Bind_OnShader(m_pModelCom, "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom, "g_ProjMatrix");
	m_pTextureCom->SetUp_OnShader(m_pModelCom, "g_DiffuseTexture");
	m_pModelCom->Set_RawValue("g_DissolvePwr", &m_fDissolvePower, sizeof(_float));
	if (FAILED(m_pDissolveTextureCom->SetUp_OnShader(m_pModelCom, "g_DissolveTexture")))
		return E_FAIL;

	_float3 g_RGB = _float3(1.f, 1.f, 1.f);
	m_pModelCom->Set_RawValue("g_RGB", &g_RGB, sizeof(_float3));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CDecal * CDecal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CDecal*		pInstance = new CDecal(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CDecal");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CDecal::Clone(void* pArg)
{
	CDecal*		pInstance = new CDecal(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CDecal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecal::Free()
{

	__super::Free();

	
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);	
	Safe_Release(m_pModelCom);
	Safe_Release(m_pDissolveTextureCom);
}







// -------------------------------------------
// -------------------------------------------
// -------------------------------------------
// -------------------------------------------
// -------------------------------------------


CDecal2::CDecal2(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CDecal2::CDecal2(const CDecal2& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDecal2::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CDecal2::NativeConstruct(void* pArg)
{
	// pArg는 위치일것이다.
	if (SetUp_Component())
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vector*)pArg);

	// xy로 서있는 렉트이므로 눕혀주자.
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));

	return S_OK;
}

_int CDecal2::Tick(_float fTimeDelta)
{
	if (m_isDead)
		return -1;

	m_TimeAcc += fTimeDelta;
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	m_fScale += fTimeDelta * 10.f;
	if (m_fScale >= 4.f)
		m_fScale = 4.f;
	m_pTransformCom->Set_Scale(_float3(m_fScale, m_fScale, m_fScale));


	return _int();
}

_int CDecal2::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (m_TimeAcc >= 3.f)
		m_isDead = true;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHA, this);

	return _int();
}

HRESULT CDecal2::Render(_uint iPassIndex)
{
	iPassIndex = 6;
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(iPassIndex);

	return S_OK;
}

HRESULT CDecal2::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 7.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(10.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)m_pRendererCom.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Decal"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDecal2::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pTransformCom->Bind_OnShader(m_pModelCom, "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom, "g_ProjMatrix");
	m_pTextureCom->SetUp_OnShader(m_pModelCom, "g_DiffuseTexture", 1);
	_float tempZero = 0.f;
	m_pModelCom->Set_RawValue("g_DissolvePwr", &tempZero, sizeof(_float));
	_float3 g_RGB = _float3(1.f, 0.f, 0.f);
	m_pModelCom->Set_RawValue("g_RGB", &g_RGB, sizeof(_float3));
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CDecal2* CDecal2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CDecal2* pInstance = new CDecal2(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CDecal2");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CDecal2::Clone(void* pArg)
{
	CDecal2* pInstance = new CDecal2(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CDecal2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecal2::Free()
{

	__super::Free();


	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
}
