#include "..\public\Renderer.h"
#include "RendererStates.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "VIBuffer_Rect.h"


#define SHADOW_MAP_TEST

#ifdef SHADOW_MAP_TEST

#pragma region Effect
class Effect
{
public:
	Effect(ID3D11Device* device, const std::wstring& filename);
	virtual ~Effect();

private:
	Effect(const Effect& rhs);
	Effect& operator=(const Effect& rhs);

protected:
	ID3DX11Effect* mFX;
};

Effect::Effect(ID3D11Device* device, const std::wstring& filename)
	: mFX(0)
{
	std::ifstream fin(filename.c_str(), std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size,
		0, device, &mFX));
}

Effect::~Effect()
{
	ReleaseCOM(mFX);
}

#pragma endregion

#pragma region BuildShadowMapEffect
class BuildShadowMapEffect : public Effect
{
public:
	BuildShadowMapEffect(ID3D11Device* device, const std::wstring& filename);
	~BuildShadowMapEffect();

	void SetViewProj(CXMMATRIX M) { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProj(CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M) { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M) { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M) { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }

	void SetHeightScale(float f) { HeightScale->SetFloat(f); }
	void SetMaxTessDistance(float f) { MaxTessDistance->SetFloat(f); }
	void SetMinTessDistance(float f) { MinTessDistance->SetFloat(f); }
	void SetMinTessFactor(float f) { MinTessFactor->SetFloat(f); }
	void SetMaxTessFactor(float f) { MaxTessFactor->SetFloat(f); }

	void SetDiffuseMap(ID3D11ShaderResourceView* tex) { DiffuseMap->SetResource(tex); }
	void SetNormalMap(ID3D11ShaderResourceView* tex) { NormalMap->SetResource(tex); }

	ID3DX11EffectTechnique* BuildShadowMapTech;
	ID3DX11EffectTechnique* BuildShadowMapAlphaClipTech;
	ID3DX11EffectTechnique* TessBuildShadowMapTech;
	ID3DX11EffectTechnique* TessBuildShadowMapAlphaClipTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectScalarVariable* HeightScale;
	ID3DX11EffectScalarVariable* MaxTessDistance;
	ID3DX11EffectScalarVariable* MinTessDistance;
	ID3DX11EffectScalarVariable* MinTessFactor;
	ID3DX11EffectScalarVariable* MaxTessFactor;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;
};

BuildShadowMapEffect::BuildShadowMapEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	BuildShadowMapTech = mFX->GetTechniqueByName("BuildShadowMapTech");
	BuildShadowMapAlphaClipTech = mFX->GetTechniqueByName("BuildShadowMapAlphaClipTech");

	TessBuildShadowMapTech = mFX->GetTechniqueByName("TessBuildShadowMapTech");
	TessBuildShadowMapAlphaClipTech = mFX->GetTechniqueByName("TessBuildShadowMapAlphaClipTech");

	ViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	HeightScale = mFX->GetVariableByName("gHeightScale")->AsScalar();
	MaxTessDistance = mFX->GetVariableByName("gMaxTessDistance")->AsScalar();
	MinTessDistance = mFX->GetVariableByName("gMinTessDistance")->AsScalar();
	MinTessFactor = mFX->GetVariableByName("gMinTessFactor")->AsScalar();
	MaxTessFactor = mFX->GetVariableByName("gMaxTessFactor")->AsScalar();
	DiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	NormalMap = mFX->GetVariableByName("gNormalMap")->AsShaderResource();
}

BuildShadowMapEffect::~BuildShadowMapEffect()
{
}

#pragma endregion

#pragma region Effects
class Effects
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static BuildShadowMapEffect* BuildShadowMapFX;
};

BuildShadowMapEffect* Effects::BuildShadowMapFX = 0;
void Effects::InitAll(ID3D11Device* device)
{
	BuildShadowMapFX = new BuildShadowMapEffect(device, L"BuildShadowMap.fxo");
}

void Effects::DestroyAll()
{
	SafeDelete(BuildShadowMapFX);
}

#pragma endregion

#pragma region ShadowsDemo
#include "Graphic_Device.h"

class ShadowMap
{
public:
	ShadowMap(ID3D11Device* device, UINT width, UINT height);
	~ShadowMap();

	ID3D11ShaderResourceView* DepthMapSRV();

	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc);

private:
	ShadowMap(const ShadowMap& rhs);
	ShadowMap& operator=(const ShadowMap& rhs);

private:
	UINT mWidth;
	UINT mHeight;

	ID3D11ShaderResourceView* mDepthMapSRV;
	ID3D11DepthStencilView* mDepthMapDSV;

	D3D11_VIEWPORT mViewport;
};

static const int SMapSize = 2048;
ShadowMap* mSmap;
XMFLOAT4X4 mLightView;
XMFLOAT4X4 mLightProj;
XMFLOAT4X4 mShadowTransform;

ShadowMap::ShadowMap(ID3D11Device* device, UINT width, UINT height)
	: mWidth(width), mHeight(height), mDepthMapSRV(0), mDepthMapDSV(0)
{
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = static_cast<float>(width);
	mViewport.Height = static_cast<float>(height);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	// Use typeless format because the DSV is going to interpret
	// the bits as DXGI_FORMAT_D24_UNORM_S8_UINT, whereas the SRV is going to interpret
	// the bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* depthMap = 0;
	HR(device->CreateTexture2D(&texDesc, 0, &depthMap));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	HR(device->CreateDepthStencilView(depthMap, &dsvDesc, &mDepthMapDSV));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	HR(device->CreateShaderResourceView(depthMap, &srvDesc, &mDepthMapSRV));

	// View saves a reference to the texture so we can release our reference.
	ReleaseCOM(depthMap);
}

ShadowMap::~ShadowMap()
{
	ReleaseCOM(mDepthMapSRV);
	ReleaseCOM(mDepthMapDSV);
}

ID3D11ShaderResourceView* ShadowMap::DepthMapSRV()
{
	return mDepthMapSRV;
}

void ShadowMap::BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc)
{
	dc->RSSetViewports(1, &mViewport);

	// Set null render target because we are only going to draw to depth buffer.
	// Setting a null render target will disable color writes.
	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	dc->OMSetRenderTargets(1, renderTargets, mDepthMapDSV);

	dc->ClearDepthStencilView(mDepthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
#pragma endregion

#endif



CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);
}

HRESULT CRenderer::NativeConstruct_Prototype()
{
	if (m_pTarget_Manager == nullptr)
		return E_FAIL;

#ifdef SHADOW_MAP_TEST
	mSmap = new ShadowMap(m_pDevice, (_uint)1600, (_uint)900);
#endif

	_uint iNumViewports = 1;
	D3D11_VIEWPORT ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof(D3D11_VIEWPORT));

	m_pDeviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	/* RT */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Diffuse"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Normal"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Depth"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Shade"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Specular"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

#ifdef _DEBUG
	/* Debug */
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Diffuse"), 0, 0, 200, 200)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Normal"), 0, 200, 200, 200)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Depth"), 0, 400, 200, 200)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Shade"), 200, 0, 200, 200)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Specular"), 200, 200, 200, 200)))
		return E_FAIL;
#endif // _DEBUG

	/* MRT */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	/* Lighting */
	if (FAILED(m_pLight_Manager->NativeConstruct(m_pDevice, m_pDeviceContext)))
		return E_FAIL;


	/* Final Render(Render Blending) */
	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"));
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());
	m_TransformMatrix._11 = ViewportDesc.Width;
	m_TransformMatrix._22 = ViewportDesc.Height;
	m_TransformMatrix._41 = 0.0f;
	m_TransformMatrix._42 = 0.0f;

	XMStoreFloat4x4(&m_OrthoMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f)));

	if (FAILED(RenderStates::CreateRenderStates(m_pDevice)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::NativeConstruct(void * pArg)
{
	return S_OK;
}

/* 객체생성시에 추가한다(x) */
/* 매 프레임마다 그려져야할 객체를 판명하여 추가하낟. */
HRESULT CRenderer::Add_RenderGroup(RENDER eRenderGroup, CGameObject * pGameObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;	
}

HRESULT CRenderer::Add_PostRenderGroup(CGameObject* pGameObject)
{
	m_PostRenderObjects.push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

#ifdef _DEBUG
bool bDraw_Debug = true;
#endif

HRESULT CRenderer::Draw()
{
	// SkyBox
	if (FAILED(Render_Priority()))
		return E_FAIL;
	// Terrain (Enviroment)
	if (FAILED(Render_Priority_Terrain()))
		return E_FAIL;
	// War 제외 : diffuse normal depth를 넣자.
	if (FAILED(Render_NonAlpha()))
		return E_FAIL;
	// 빛연산을 여기서하자 : shade와 spec에 넣자.
	if (FAILED(Render_LightAcc()))
		return E_FAIL;
	// [Deffered End] 최종 백버퍼에 합치자
	if (FAILED(Render_Blend())) 
		return E_FAIL;

	// War는 따로하자..
	if (FAILED(Render_NonAlpha_War()))
		return E_FAIL;
	
	// 빛처리 필요없는 애들의 경우 바로 백버퍼에 그리자
	if (FAILED(Render_NonLight())) 
		return E_FAIL;

	// 알파블렌딩은 Deffered로 말고 바로 백버퍼에 그리자
	if (FAILED(Render_Alpha())) 
		return E_FAIL;
	// UI도 바로 백버퍼에 그리자
	if (FAILED(Render_UI())) 
		return E_FAIL;		
	if (FAILED(Render_Mouse()))
		return E_FAIL;
	
	return S_OK;
}

// 오브젝트들이 각자 출력할 Text나 Debugging용 Line은 여기서 출력하자
HRESULT CRenderer::PostDraw(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	//ClearRenderStates();

	/* 리스트 순회 */
	for (auto& pGameObject : m_PostRenderObjects)
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->PostRender(m_spriteBatch, m_spriteFont)))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_PostRenderObjects.clear();

#ifdef _DEBUG
	if (CInput_Device::GetInstance()->Key_Down(DIK_F3))
	{
		bDraw_Debug = !bDraw_Debug;
	}

	if (bDraw_Debug)
	{
		m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_Deferred"), 0);
		m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_LightAcc"), 0);
		m_pTarget_Manager->PostRender_DebugBuffer(TEXT("MRT_Deferred"), 0, m_spriteBatch, m_spriteFont);
		m_pTarget_Manager->PostRender_DebugBuffer(TEXT("MRT_LightAcc"), 0, m_spriteBatch, m_spriteFont);
	}
#endif // _DEBUG

	return S_OK;
}

HRESULT CRenderer::ClearRenderStates()
{
	m_pDeviceContext->RSSetState(0);
	m_pDeviceContext->OMSetDepthStencilState(0, 0);
	m_pDeviceContext->OMSetBlendState(0, 0, 0xffffffff);
	m_pDeviceContext->PSSetSamplers(0, 0, 0);

	return S_OK;
}

// SkyBox
HRESULT CRenderer::Render_Priority()
{
	/* 리스트 순회 */
	for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_RenderObjects[RENDER_PRIORITY].clear();

	// Restore default states
	ClearRenderStates();

	return S_OK;
}

HRESULT CRenderer::Render_Priority_Terrain()
{
	/* 리스트 순회 */
	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHA_TERRAIN])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_RenderObjects[RENDER_NONALPHA_TERRAIN].clear();

	// Restore default states
	ClearRenderStates();

	return S_OK;
}

HRESULT CRenderer::Render_NonAlpha()
{
#ifdef SHADOW_MAP_TEST
	//mSmap->BindDsvAndSetNullRenderTarget(m_pDeviceContext);

	//// DrawSceneToShadowMap(); 동작을 여기서 수행
	//for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHA])
	//{
	//	if (nullptr != pGameObject)
	//	{
	//		if (FAILED(pGameObject->Render()))
	//			return E_FAIL;

	//		Safe_Release(pGameObject);
	//	}
	//}

	////
	//// Restore the back and depth buffer to the OM stage.
	////
	//m_pDeviceContext->RSSetState(0);
	//ID3D11RenderTargetView* renderTargets[1] = { CGraphic_Device::GetInstance()->Get_BackBufferRTV() };
	//m_pDeviceContext->OMSetRenderTargets(1, renderTargets, CGraphic_Device::GetInstance()->Get_DepthStencilView());
	//m_pDeviceContext->RSSetViewports(1, CGraphic_Device::GetInstance()->Get_ViewPortDesc_Ptr());

	//m_pDeviceContext->ClearRenderTargetView(CGraphic_Device::GetInstance()->Get_BackBufferRTV(), reinterpret_cast<const float*>(&Colors::Silver));
	//m_pDeviceContext->ClearDepthStencilView(CGraphic_Device::GetInstance()->Get_DepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
#endif
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Deferred"))))
		return E_FAIL;
	
	/* 리스트 순회 */
	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_RenderObjects[RENDER_NONALPHA].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	// Restore default states
	ClearRenderStates();


	return S_OK;
}

HRESULT CRenderer::Render_NonAlpha_War()
{
	/* 리스트 순회 */
	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHA_WAR])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_RenderObjects[RENDER_NONALPHA_WAR].clear();

	// Restore default states
	ClearRenderStates();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();

	// Restore default states
	ClearRenderStates();

	return S_OK;
}


HRESULT CRenderer::Render_Alpha()
{
	/* 카메라로부터 멀리 있는 객체부터 그린다. */
	m_RenderObjects[RENDER_ALPHA].sort([&](CGameObject* pSour, CGameObject* pDest) 
	{
		return pSour->Get_CamDistance() > pDest->Get_CamDistance();
	});


	/* 리스트 순회 */
	for (auto& pGameObject : m_RenderObjects[RENDER_ALPHA])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_RenderObjects[RENDER_ALPHA].clear();
	
	// Restore default states
	ClearRenderStates();


	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	/* 리스트 순회 */
	for (auto& pGameObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_RenderObjects[RENDER_UI].clear();

	// Restore default states
	ClearRenderStates();

	return S_OK;
}

HRESULT CRenderer::Render_Mouse()
{
	/* 리스트 순회 */
	for (auto& pGameObject : m_RenderObjects[RENDER_MOUSE])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_RenderObjects[RENDER_MOUSE].clear();

	// Restore default states
	ClearRenderStates();

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	//if (nullptr == m_pTarget_Manager ||
	//	nullptr == m_pLight_Manager)
	//	return E_FAIL;

	///* 장치에는 Target_Shade가 셋팅된다. */
	//if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_LightAcc"))))
	//	return E_FAIL;

	///* Target_Shade에 그린다. */
	//m_pLight_Manager->Render();

	//if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
	//	return E_FAIL;

	return S_OK;
}


HRESULT CRenderer::Render_Blend()
{
	m_pVIBuffer->Set_RawValue("g_TransformMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OrthoMatrix)), sizeof(_float4x4));

	m_pVIBuffer->Set_ShaderResourceView("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse")));

	// 아래 2개는 Lighting에서 찍어주는것. 
	m_pVIBuffer->Set_ShaderResourceView("g_ShadeTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Shade")));
	m_pVIBuffer->Set_ShaderResourceView("g_SpecularTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")));

	m_pVIBuffer->Render(3); // final pass로 출력하자

	return S_OK;
}


CRenderer * CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CRenderer*		pInstance = new CRenderer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CRenderer::Clone(void * pArg)
{
	this->AddRef();

	return this;
}

void CRenderer::Free()
{
#ifdef SHADOW_MAP_TEST
	SafeDelete(mSmap);
#endif

	__super::Free();

	for (auto& ObjectList : m_RenderObjects)
	{
		for (auto& pGameObject : ObjectList)
			Safe_Release(pGameObject);

		ObjectList.clear();		
	}

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);
	
	Safe_Release(m_pVIBuffer);
}

