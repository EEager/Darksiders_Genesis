#include "stdafx.h"
#include "..\public\Terrain.h"

#include "GameInstance.h"
#include "Light.h"


CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::NativeConstruct_Prototype()
{	
	// Material Init
	m_tMtrlDesc.vMtrlDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_tMtrlDesc.vMtrlAmbient = { 0.5f, 0.5f, 0.5f, 1.0f };
	m_tMtrlDesc.vMtrlSpecular = { 0.2f, 0.2f, 0.2f, 16.0f };
	m_tMtrlDesc.vMtrlEmissive = {1.f, 1.f, 1.f, 1.f};
	m_tMtrlDesc.fMtrlPower = 20.f;

	return S_OK;
}

HRESULT CTerrain::NativeConstruct(void * pArg)
{
	if (SetUp_Component())
		return E_FAIL;

	if (FAILED(Create_FilterTexture()))
		return E_FAIL;


	return S_OK;
}

_int CTerrain::Tick(_float fTimeDelta)
{
	return _int();
}

_int CTerrain::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA_TERRAIN, this)))
		return 0;

	return _int();
}

HRESULT CTerrain::Render()
{
	// m_pDeviceContext->RSSetState(RenderStates::WireframeRS.Get());

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	/* 장치에 월드변환 행렬을 저장한다. */

//	m_pVIBufferCom->Render(0);

	m_pRendererCom->ClearRenderStates();

	return S_OK;
}

HRESULT CTerrain::SetUp_Component()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	
	///* For.Com_Texture*/
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
	//	return E_FAIL;

	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Black"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
		return E_FAIL;

	/* For.Com_Filter */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Filter"), TEXT("Com_Filter"), (CComponent**)&m_pTextureCom[TYPE_FILTER])))
		return E_FAIL;

	/* For.Com_Brush*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"), TEXT("Com_Brush"), (CComponent**)&m_pTextureCom[TYPE_BRUSH])))
		return E_FAIL;

	/* For.Com_Navi */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navi"), (CComponent**)&m_pNaviCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::SetUp_ConstantTable()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);	

	// Bind Directional Light
	LIGHTDESC		dirLightDesc = *pGameInstance->Get_LightDesc(0);
	DirectionalLight mDirLight;
	mDirLight.Ambient = dirLightDesc.vAmbient;
	mDirLight.Diffuse = dirLightDesc.vDiffuse;
	mDirLight.Specular = dirLightDesc.vSpecular;
	mDirLight.Direction = dirLightDesc.vDirection;
	m_pVIBufferCom->Set_RawValue("g_DirLight", &mDirLight, sizeof(DirectionalLight));

	// Bind Point Light
	LIGHTDESC		pointLightDesc = *pGameInstance->Get_LightDesc(1);
	PointLight mPointLight;
	mPointLight.Ambient = pointLightDesc.vAmbient;
	mPointLight.Diffuse = pointLightDesc.vDiffuse;
	mPointLight.Specular = pointLightDesc.vSpecular;
	mPointLight.Position = pointLightDesc.vPosition;
	mPointLight.Att = _float3(0.f, 0.1f, 0.f);
	mPointLight.Range = pointLightDesc.fRadiuse;
	m_pVIBufferCom->Set_RawValue("g_PointLight", &mPointLight, sizeof(PointLight));

	// Bind Spot Light
	LIGHTDESC*	pSpotLightDesc = pGameInstance->Get_LightDesc(2);
	SpotLight mSpotLight;
	mSpotLight.Ambient = pSpotLightDesc->vAmbient;
	mSpotLight.Diffuse = pSpotLightDesc->vDiffuse;
	mSpotLight.Specular = pSpotLightDesc->vSpecular;
	// The spotlight takes on the camera position and is aimed in the
	// same direction the camera is looking.  In this way, it looks
	// like we are holding a flashlight
	XMStoreFloat3(&mSpotLight.Position, pGameInstance->Get_CamPosition());
	XMStoreFloat3(&pSpotLightDesc->vPosition, pGameInstance->Get_CamPosition());

	XMStoreFloat3(&mSpotLight.Direction, pGameInstance->Get_CamLook());
	XMStoreFloat3(&pSpotLightDesc->vDirection, pGameInstance->Get_CamLook());
	
	mSpotLight.Att = _float3(1.f, 0.f, 0.f);
	mSpotLight.Spot = 96.f;
	mSpotLight.Range = pSpotLightDesc->fRadiuse;
	m_pVIBufferCom->Set_RawValue("g_SpotLight", &mSpotLight, sizeof(SpotLight));

	// Bind Material
	m_pVIBufferCom->Set_RawValue("g_Material", &m_tMtrlDesc, sizeof(MTRLDESC));

	// Bind Transform
	m_pTransformCom->Bind_OnShader(m_pVIBufferCom, "g_WorldMatrix"); 
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pVIBufferCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pVIBufferCom, "g_ProjMatrix"); 

	// Bind Position
	_float4			vCamPosition;	
	XMStoreFloat4(&vCamPosition, pGameInstance->Get_CamPosition());
	m_pVIBufferCom->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));

	// Bind Texture 
	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->SetUp_OnShader(m_pVIBufferCom, "g_SourTexture", 0)))
		return E_FAIL;	
	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->SetUp_OnShader(m_pVIBufferCom, "g_DestTexture", 1)))
		return E_FAIL;
	if (FAILED(m_pTextureCom[TYPE_FILTER]->SetUp_OnShader(m_pVIBufferCom, "g_FilterTexture")))
		return E_FAIL;
	if (FAILED(m_pTextureCom[TYPE_BRUSH]->SetUp_OnShader(m_pVIBufferCom, "g_BrushTexture")))
		return E_FAIL;
	// m_pVIBufferCom->Set_ShaderResourceView("g_FilterTexture", pSRV);
	if (FAILED(m_pVIBufferCom->Set_RawValue("g_vBrushPosition", &_float4(10.f, 0.f, 10.f, 1.f), sizeof(_float4))))
		return E_FAIL;
	_float		fRange = 5.f;
	if (FAILED(m_pVIBufferCom->Set_RawValue("g_fRange", &fRange, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CTerrain::Create_FilterTexture()
{
	DirectX::ScratchImage		ScratchImage;

	if (FAILED(ScratchImage.Initialize2D(DXGI_FORMAT_B8G8R8A8_UNORM, 128, 128, 1, 1)))
		return E_FAIL;	


	const DirectX::Image*	pImage = ScratchImage.GetImage(0, 0, 0);

	for (_uint i = 0; i < 128; ++i)
	{
		for (_uint j = 0; j < 128; ++j)
		{
			_uint iIndex = i * 128 + j;			

			if(j < 64)
				((_uint*)pImage->pixels)[iIndex] = D3D11COLOR_ARGB(255, 0, 0, 0);
			else
				((_uint*)pImage->pixels)[iIndex] = D3D11COLOR_ARGB(255, 255, 255, 255);
		}
	}
	

	ComPtr<ID3D11Resource>			pTextureResource;

	if (FAILED(DirectX::CreateTexture(m_pDevice, ScratchImage.GetImages(), ScratchImage.GetImageCount(), ScratchImage.GetMetadata(), pTextureResource.GetAddressOf())))
		return E_FAIL;	


	if (FAILED(m_pDevice->CreateShaderResourceView(pTextureResource.Get(), nullptr, &m_pFilter_SRV)))
		return E_FAIL;

	if (FAILED(DirectX::SaveToTGAFile(*ScratchImage.GetImage(0, 0, 0), TEXT("../Bin/Test.tga"), nullptr)))
		return E_FAIL;

	//Safe_Release(pTextureResource);
	
	

	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTerrain*		pInstance = new CTerrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CTerrain::Clone(void* pArg)
{
	CTerrain*		pInstance = new CTerrain(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{

	__super::Free();

	for (_uint i = 0; i < TYPE_END; ++i)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pNaviCom);
	Safe_Release(m_pFilter_SRV);
	Safe_Release(m_pTransformCom);	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
