#include "stdafx.h"
#include "..\public\Effect\FireEffect.h"

#include "GameInstance.h"


CFireEffect::CFireEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CFireEffect::CFireEffect(const CFireEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFireEffect::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CFireEffect::NativeConstruct(void * pArg)
{
	if (SetUp_Component())
		return E_FAIL;
	
	return S_OK;
}

_int CFireEffect::Tick(_float fTimeDelta)
{
	m_pModelCom->Update(fTimeDelta);
	
	return _int();
}

_int CFireEffect::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	// War를 따라다니도록하자.
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform*		pTarget = (CTransform*)pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, TEXT("Layer_War"), TEXT("Com_Transform"));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pTarget->Get_State(CTransform::STATE_POSITION));

	// 빌보딩
	m_pTransformCom->Set_BillBoard();
		
	// Blending을 하기 위해 알파그룹에 넣는다.
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHA, this)))
		return 0; 

	RELEASE_INSTANCE(CGameInstance);	


	return _int();
}

HRESULT CFireEffect::Render(_uint iPassIndex)
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(1); // Alpha Blending 
	m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);

	return S_OK;
}

HRESULT CFireEffect::SetUp_Component()
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

	///* For.Com_Texture */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	//	return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_RectInstance"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CFireEffect::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);	

	m_pTransformCom->Bind_OnShader(m_pModelCom, "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom, "g_ProjMatrix");

	m_pModelCom->Set_ShaderResourceView("g_DepthTexture", pGameInstance->Get_RenderTarget_SRV(TEXT("Target_Depth_Cur")));
	m_pTextureCom->SetUp_OnShader(m_pModelCom, "g_DiffuseTexture"); 

	// From Dx11Demo_33
	// Distortion 효과를 적용하자.
	{
		// 세 가지 다른 노이즈 텍스처에 대해 세 가지 스크롤 속도를 설정합니다.
		XMFLOAT3 scrollSpeeds = XMFLOAT3(1.3f, 2.1f, 2.3f);

		// 세 개의 서로 다른 노이즈 옥타브 텍스처를 만드는 데 사용할 세 개의 스케일을 설정합니다.
		XMFLOAT3 scales = XMFLOAT3(1.0f, 2.0f, 3.0f);

		// 세 가지 다른 노이즈 텍스처에 대해 세 가지 다른 x 및 y 왜곡 인수를 설정합니다.
		XMFLOAT2 distortion1 = XMFLOAT2(0.1f, 0.2f);
		XMFLOAT2 distortion2 = XMFLOAT2(0.1f, 0.3f);
		XMFLOAT2 distortion3 = XMFLOAT2(0.1f, 0.1f);

		// 텍스처 좌표 샘플링 섭동의 스케일과 바이어스.
		float distortionScale = 0.8f;
		float distortionBias = 0.5f;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CFireEffect * CFireEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CFireEffect*		pInstance = new CFireEffect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CRectEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CFireEffect::Clone(void* pArg)
{
	CFireEffect*		pInstance = new CFireEffect(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CRectEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFireEffect::Free()
{

	__super::Free();

	
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
