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
	//m_pModelCom_RectInstance->Update(fTimeDelta);

	m_fMyTimeDelta += fTimeDelta;
	
	return _int();
}

_int CFireEffect::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	// War를 따라다니도록하자.
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform*		pTarget = (CTransform*)pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, TEXT("Layer_War"), TEXT("Com_Transform"));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pTarget->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 5.5f, 0.f, 0.f));

	// 빌보딩
	//m_pTransformCom->Set_BillBoard();
		
	// Blending을 하기 위해 알파그룹에 넣는다.
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHA, this)))
		return 0; 

	RELEASE_INSTANCE(CGameInstance);	


	return _int();
}

HRESULT CFireEffect::Render(_uint iPassIndex)
{
	if (FAILED(SetUp_ConstantTable(m_pModelCom_RectInstance)))
		return E_FAIL;

	//m_pModelCom_Rect->Render(3); // Distortion_Alpha
	m_pModelCom_RectInstance->Render(1); // Distortion_Alpha Blending 
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

	/* For.Com_Texture_Diffuse */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_fire01"), TEXT("Com_Texture_Diffuse"), (CComponent**)&m_pTextureDiffuse)))
		return E_FAIL;
	/* For.Com_Texture_Noise */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_noise"), TEXT("Com_Texture_Noise"), (CComponent**)&m_pTextureNoise)))
		return E_FAIL;

	/* For.Com_Texture_Alpha */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_alpha"), TEXT("Com_Texture_Alpha"), (CComponent**)&m_pTextureAlpha)))
		return E_FAIL;

	/* For.Com_Model1 */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_RectInstance"), TEXT("Com_Model1"), (CComponent**)&m_pModelCom_RectInstance)))
		return E_FAIL;

	/* For.Com_Model2 */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model2"), (CComponent**)&m_pModelCom_Rect)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CFireEffect::SetUp_ConstantTable(CVIBuffer* pVIBuffer)
{
	if (nullptr == pVIBuffer)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);	

	m_pTransformCom->Bind_OnShader(pVIBuffer, "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, pVIBuffer, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, pVIBuffer, "g_ProjMatrix");

	//// For.소프트 렌더링
	//pVIBuffer->Set_ShaderResourceView("g_DepthTexture", pGameInstance->Get_RenderTarget_SRV(TEXT("Target_Depth_Cur")));

	m_pTextureDiffuse->SetUp_OnShader(pVIBuffer, "g_DiffuseTexture");

	// Noise
	m_pTextureNoise->SetUp_OnShader(pVIBuffer, "g_NoiseTexture", 1);
	m_pTextureNoise->SetUp_OnShader(pVIBuffer, "g_NoiseTexture_HeatHaze", 2); 

	// 알파
	m_pTextureAlpha->SetUp_OnShader(pVIBuffer, "g_AlphaTexture", 0);
	m_pTextureAlpha->SetUp_OnShader(pVIBuffer, "g_AlphaTexture_HeatHaze", 1); 

	// From Dx11Demo_33
	// Distortion 효과를 적용하자.
	{
		// 세 가지 다른 노이즈 텍스처에 대해 세 가지 스크롤 속도를 설정합니다.
		_float3 scrollSpeeds = _float3(1.3f, 2.1f, 2.3f);
		// 세 개의 서로 다른 노이즈 옥타브 텍스처를 만드는 데 사용할 세 개의 스케일을 설정합니다.
		_float3 scales = _float3(1.0f, 2.0f, 3.0f);
		// Bind NosiseBuffer
		m_fMyTimeDelta;
		if (m_fMyTimeDelta > 1000.0f)
		{
			m_fMyTimeDelta = 0.0f;
		}
		pVIBuffer->Set_RawValue("frameTime", &m_fMyTimeDelta, sizeof(_float));
		pVIBuffer->Set_RawValue("scrollSpeeds", &scrollSpeeds, sizeof(_float3));
		pVIBuffer->Set_RawValue("scales", &scales, sizeof(_float3));

		// 세 가지 다른 노이즈 텍스처에 대해 세 가지 다른 x 및 y 왜곡 인수를 설정합니다.
		_float2 distortion1 = _float2(0.1f, 0.2f);
		_float2 distortion2 = _float2(0.1f, 0.3f);
		_float2 distortion3 = _float2(0.1f, 0.1f);
		// 텍스처 좌표 샘플링 섭동의 스케일과 바이어스.
		float distortionScale = 0.2f; // 크면 위로 길어진다.
		float distortionBias = 0.3f; // fire01의 어느 지점부터 찍을지
		// Bind DistortionBuffer
		pVIBuffer->Set_RawValue("distortion1", &distortion1, sizeof(_float2));
		pVIBuffer->Set_RawValue("distortion2", &distortion2, sizeof(_float2));
		pVIBuffer->Set_RawValue("distortion3", &distortion3, sizeof(_float2));
		pVIBuffer->Set_RawValue("distortionScale", &distortionScale, sizeof(_float));
		pVIBuffer->Set_RawValue("distortionBias", &distortionBias, sizeof(_float));
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

	
	Safe_Release(m_pTextureDiffuse);
	Safe_Release(m_pTextureNoise);
	Safe_Release(m_pTextureAlpha);
	Safe_Release(m_pTransformCom);	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom_RectInstance);
	Safe_Release(m_pModelCom_Rect);
}
