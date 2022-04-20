#include "stdafx.h"
#include "..\public\Effect\Explosion.h"

#include "GameInstance.h"


CExplosion::CExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CExplosion::CExplosion(const CExplosion & rhs)
	: CGameObject(rhs)
{
}

HRESULT CExplosion::NativeConstruct_Prototype()
{	
	return S_OK;
}

HRESULT CExplosion::NativeConstruct(void * pArg)
{
	// pArg는 위치일것이다.

	if (SetUp_Component())
		return E_FAIL;
	_vector vPos = *(_vector*)pArg + XMVectorSet(0.f, 1.5f, 0.f, 0.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pTransformCom->Set_Scale(_float3(7.f, 7.f, 7.f));
	
	return S_OK;
}

_int CExplosion::Tick(_float fTimeDelta)
{
	if (m_isDead)
		return -1;

	m_fAnimIdx += fTimeDelta*30.f;

	return _int();
}

_int CExplosion::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	
	if (m_fAnimIdx >= 48)
	{
		m_isDead = true;
	}

	m_pTransformCom->Set_BillBoard();

	if (m_isDead == false)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHA, this);

	return _int();
}

HRESULT CExplosion::Render(_uint iPassIndex) 
{
	iPassIndex = 5;  
	if (FAILED(SetUp_ConstantTable())) 
		return E_FAIL;

	m_pModelCom->Render(iPassIndex);

	return S_OK;
}

HRESULT CExplosion::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CExplosion::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);	

	m_pTransformCom->Bind_OnShader(m_pModelCom, "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom, "g_ProjMatrix");
	m_pTextureCom->SetUp_OnShader(m_pModelCom, "g_DiffuseTexture", (_uint)m_fAnimIdx);

	// For.소프트 렌더링
	m_pModelCom->Set_ShaderResourceView("g_DepthTexture", pGameInstance->Get_RenderTarget_SRV(TEXT("Target_Depth_Cur")));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CExplosion * CExplosion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CExplosion*		pInstance = new CExplosion(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CExplosion");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CExplosion::Clone(void* pArg)
{
	CExplosion*		pInstance = new CExplosion(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CExplosion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExplosion::Free()
{

	__super::Free();

	
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
