#include "stdafx.h"
#include "..\public\Sky.h"	

#include "GameInstance.h"
#include "Light.h"

CSky::CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSky::CSky(const CSky & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSky::NativeConstruct_Prototype()
{	
	return S_OK;
}

HRESULT CSky::NativeConstruct(void * pArg)
{
	if (SetUp_Component())
		return E_FAIL;

	return S_OK;
}

_int CSky::Tick(_float fTimeDelta)
{
	return _int();
}

_int CSky::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix			ViewMatrix;
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pGameInstance->Get_CamPosition());

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this)))
		return 0;

	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

HRESULT CSky::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	/* 장치에 월드변환 행렬을 저장한다. */
	m_pVIBufferCom->Render(0);

	return S_OK;
}

HRESULT CSky::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(TransformDesc));

	TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	
	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CSky::SetUp_ConstantTable()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);	

	// Bind Transform
	m_pTransformCom->Bind_OnShader(m_pVIBufferCom, "g_WorldMatrix"); 
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pVIBufferCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pVIBufferCom, "g_ProjMatrix"); 

	// Bind Texture 
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pVIBufferCom, "g_CubeMapTexture", 0)))
		return E_FAIL;	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CSky * CSky::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CSky*		pInstance = new CSky(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CSky::Clone(void* pArg)
{
	CSky*		pInstance = new CSky(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSky::Free()
{

	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
