#include "stdafx.h"
#include "..\public\Fork.h"

#include "GameInstance.h"


CFork::CFork(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CFork::CFork(const CFork & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFork::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CFork::NativeConstruct(void * pArg)
{
	if (SetUp_Component())
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet((float)(rand() % 10), 2.f, (float)(rand() % 10), 1.f));

	return S_OK;
}

_int CFork::Tick(_float fTimeDelta)
{
	return _int();
}

_int CFork::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		return 0;

	return _int();
}

HRESULT CFork::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	/* 장치에 월드변환 행렬을 저장한다. */
	_uint	iNumMeshContainer = m_pModelCom->Get_NumMeshContainer();

	

	for (_uint i = 0; i < iNumMeshContainer; ++i)
	{
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		

		m_pModelCom->Render(i, 0);
	}

	// restore default states, as the SkyFX changes them in the effect file.
	m_pDeviceContext->RSSetState(0);
	m_pDeviceContext->OMSetDepthStencilState(0, 0);

	

	return S_OK;
}

HRESULT CFork::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fork"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CFork::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);	

	m_pTransformCom->Bind_OnShader(m_pModelCom, "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom, "g_ProjMatrix");

	/*LIGHTDESC		LightDesc = *pGameInstance->Get_LightDesc(0); 
	* m_pModelCom->Set_RawValue 만들어야함 시방...
	m_pVIBufferCom->Set_RawValue("g_vLightDir", &_float4(LightDesc.vDirection, 0.f), sizeof(_float4));
	m_pVIBufferCom->Set_RawValue("g_vLightDiffuse", &LightDesc.vDiffuse, sizeof(_float4));
	m_pVIBufferCom->Set_RawValue("g_vLightAmbient", &LightDesc.vAmbient, sizeof(_float4));
	m_pVIBufferCom->Set_RawValue("g_vLightSpecular", &LightDesc.vSpecular, sizeof(_float4));

	_float4			vCamPosition;	
	XMStoreFloat4(&vCamPosition, pGameInstance->Get_CamPosition());
	m_pVIBufferCom->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));*/
	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CFork * CFork::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CFork*		pInstance = new CFork(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CFork");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CFork::Clone(void* pArg)
{
	CFork*		pInstance = new CFork(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CFork");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFork::Free()
{

	__super::Free();


	Safe_Release(m_pTransformCom);	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
