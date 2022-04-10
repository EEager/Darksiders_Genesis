#include "stdafx.h"
#include "..\public\PointEffect.h"

#include "GameInstance.h"


CPointEffect::CPointEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPointEffect::CPointEffect(const CPointEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPointEffect::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CPointEffect::NativeConstruct(void * pArg)
{
	if (SetUp_Component())
		return E_FAIL;
	
	return S_OK;
}

_int CPointEffect::Tick(_float fTimeDelta)
{


	//m_pModelCom->Update(fTimeDelta);
	
	return _int();
}

_int CPointEffect::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	//CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//CTransform*		pTarget = (CTransform*)pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, TEXT("Layer_War"), TEXT("Com_Transform"));

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, pTarget->Get_State(CTransform::STATE_POSITION));
	//	
	//if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHA, this)))
	//	return 0; 
	// 
	//RELEASE_INSTANCE(CGameInstance);	


	return _int();
}

HRESULT CPointEffect::Render(_uint iPassIndex)
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(1); // Alpha Blending

	m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);

	return S_OK;
}

HRESULT CPointEffect::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_PointInstance"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CPointEffect::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);	

	m_pTransformCom->Bind_OnShader(m_pModelCom, "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom, "g_ProjMatrix");
	m_pModelCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_vector));
	
	m_pTextureCom->SetUp_OnShader(m_pModelCom, "g_DiffuseTexture");

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CPointEffect * CPointEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPointEffect*		pInstance = new CPointEffect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CRectEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPointEffect::Clone(void* pArg)
{
	CPointEffect*		pInstance = new CPointEffect(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CRectEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPointEffect::Free()
{

	__super::Free();

	
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
