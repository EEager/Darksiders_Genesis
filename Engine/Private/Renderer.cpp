#include "..\public\Renderer.h"
#include "RendererStates.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "VIBuffer_Rect.h"
#include "PipeLine.h"
#include "Texture.h"




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
	// Just PreBackBuffer
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_PreBB"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// NonAlpha
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Diffuse"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Normal"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Depth_Cur"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Emissive"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_HitPower"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Depth_Prev"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// LightAcc
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Shade"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Specular"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// Shadown Map
	// 광원 시선에서 바라본 깊이값들을 저장할 렌더타겟
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Shadow_Env"), m_pDevice, m_pDeviceContext, SHADOWMAP_SIZE_X, SHADOWMAP_SIZE_Y, DXGI_FORMAT_R24G8_TYPELESS, _float4(1.f, 1.f, 1.f, 1.f), CRenderTarget::RT_DEPTH_STENCIL)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Shadow_Obj"), m_pDevice, m_pDeviceContext, SHADOWMAP_SIZE_X, SHADOWMAP_SIZE_Y, DXGI_FORMAT_R24G8_TYPELESS, _float4(1.f, 1.f, 1.f, 1.f), CRenderTarget::RT_DEPTH_STENCIL)))
		return E_FAIL;

	// Distortion Map
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Distortion"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

#ifdef _DEBUG
#define WIDTH 150
	/* Debug */
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_PreBB"), WIDTH * 0, WIDTH * 5, WIDTH, WIDTH)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Diffuse"), WIDTH * 0, WIDTH * 0, WIDTH, WIDTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Normal"), WIDTH * 1, WIDTH * 0, WIDTH, WIDTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Depth_Cur"), WIDTH * 2, WIDTH * 0, WIDTH, WIDTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Depth_Prev"), WIDTH * 3, WIDTH * 0, WIDTH, WIDTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Emissive"), WIDTH * 4, WIDTH * 0, WIDTH, WIDTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_HitPower"), WIDTH * 5, WIDTH * 0, WIDTH, WIDTH)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Shade"), WIDTH * 0, WIDTH * 1, WIDTH, WIDTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Specular"), WIDTH * 1, WIDTH * 1, WIDTH, WIDTH)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Shadow_Env"), WIDTH * 0, WIDTH * 2, WIDTH, WIDTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Shadow_Obj"), WIDTH * 1, WIDTH * 2, WIDTH, WIDTH)))
		return E_FAIL;

	// Distortion
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Distortion"), WIDTH * 0, WIDTH * 3, WIDTH, WIDTH)))
		return E_FAIL;
#endif // _DEBUG

	/* MRT */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_PreBB"), TEXT("Target_PreBB"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth_Cur"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Emissive"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_HitPower"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth_Prev"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	// Shadow
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Shadows"), TEXT("Target_Shadow_Env"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Shadows"), TEXT("Target_Shadow_Obj"))))
		return E_FAIL;

	// Distortion
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Alpha"), TEXT("Target_PreBB"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Alpha"), TEXT("Target_Distortion"))))
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

HRESULT CRenderer::Set_RawValue(const char* pConstantName, void* pData, _uint iSize)
{
	if (nullptr == m_pEffect)
		return E_FAIL;
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iSize);
}

bool testOnce;
HRESULT CRenderer::Draw()
{
	if (testOnce == false)
	{

#if 0 // test_particles
   		_uint		iFlag = 0;
#ifdef _DEBUG
		iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif // _DEBUG	
		FAILED(D3DX11CompileEffectFromFile(L"../Bin/ShaderFiles/Shader_Effect_Metal.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iFlag, 0, m_pDevice, &m_pEffect, nullptr));

		m_StreamOutTech = m_pEffect->GetTechniqueByName("StreamOutTech");
		m_DrawTech = m_pEffect->GetTechniqueByName("DrawTech");

		// ▼ ----------------------------------------
		// Compile Shader 
		D3D11_INPUT_ELEMENT_DESC		ElementDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"AGE",      0, DXGI_FORMAT_R32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TYPE",     0, DXGI_FORMAT_R32_UINT,        0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		_uint iNumElements = 5;
		D3DX11_PASS_DESC passDesc;
		m_StreamOutTech->GetPassByIndex(0)->GetDesc(&passDesc);
		HR(m_pDevice->CreateInputLayout(ElementDesc, iNumElements, passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize, &m_ParticleLayout));

		// Compile Shader End
		// ▲ ----------------------------------------

		// Init Stuffs
		mFirstRun = true;
		mTimeStep = 0.0f;
		mAge = 0.0f;

		mEmitPosW = _float3(17.f, 1.f, 430.f);
		mEmitDirW = _float3(0.0f, 1.0f, 0.0f);

		// BuildVB(m_pDevice);
		{
			D3D11_BUFFER_DESC vbd;
			vbd.Usage = D3D11_USAGE_DEFAULT;
			vbd.ByteWidth = sizeof(VTXPTC) * 1;
			vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vbd.CPUAccessFlags = 0;
			vbd.MiscFlags = 0;
			vbd.StructureByteStride = 0;

			// The initial particle emitter has type 0 and age 0.  The rest
			// of the particle attributes do not apply to an emitter.
			VTXPTC p;
			ZeroMemory(&p, sizeof(VTXPTC));
			p.Age = 0.0f;
			p.Type = 0;

			D3D11_SUBRESOURCE_DATA vinitData;
			vinitData.pSysMem = &p;

			HR(m_pDevice->CreateBuffer(&vbd, &vinitData, &mInitVB));

			//
			// Create the ping-pong buffers for stream-out and drawing.
			//
			vbd.ByteWidth = sizeof(VTXPTC) * 500;
			vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

			HR(m_pDevice->CreateBuffer(&vbd, 0, &mDrawVB));
			HR(m_pDevice->CreateBuffer(&vbd, 0, &mStreamOutVB));
		}
#endif
		testOnce = true;
	}

	// ----------------------------------
	// ▼ Deferred Process Start
	if (FAILED(m_pTarget_Manager->Begin_MRT_PreBB(m_pDeviceContext, TEXT("MRT_PreBB"))))
		return E_FAIL;

	// SkyBox
	if (FAILED(Render_Priority()))
		return E_FAIL;
	
	//ShadowMap 
	if (FAILED(Render_Shadow()))
		return E_FAIL;

	if (FAILED(Render_NonAlpha()))
		return E_FAIL;
	// 빛연산을 여기서하자 : shade와 spec에 넣자.
	if (FAILED(Render_LightAcc()))
		return E_FAIL;
	// [Deffered End] 최종 백버퍼에 합치자 = NonAlpha + LightAcc
	if (FAILED(Render_Blend()))
		return E_FAIL;

	// 빛처리 필요없는 애들의 경우 바로 백버퍼에 그리자
	if (FAILED(Render_NonLight()))
		return E_FAIL;

	// Render_Alpha // Distortion Test
	{
		// Begin_MRT_Post_Alpha : 0번째는 백버퍼, 1번째는 distortionMap 이다.
		if (FAILED(m_pTarget_Manager->Begin_MRT_Alpha(m_pDeviceContext, TEXT("MRT_Alpha"))))
			return E_FAIL;

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

		if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
			return E_FAIL;

		// Restore default states
		ClearRenderStates();
	}

	// PreBB에 모아놨던 것들을 진짜 백버퍼에 출력하자
	if (FAILED(m_pTarget_Manager->End_MRT_PreBB(m_pDeviceContext)))
		return E_FAIL;

	// Render_Blend_Final
	{
		// Bind matrix
		m_pVIBuffer->Set_RawValue("g_TransformMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)), sizeof(_float4x4));
		m_pVIBuffer->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OrthoMatrix)), sizeof(_float4x4));
		// Bind Texture
		m_pVIBuffer->Set_ShaderResourceView("g_PreBBTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_PreBB")));
		m_pVIBuffer->Set_ShaderResourceView("g_DistortionTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Distortion")));

		// Bind Delta Time
		//m_fDistortionTimeAcc = MathHelper::RandF(0.f, 0.0029f);

		_bool tmpTrue = true; // 검기는 이걸로 갑니다 ^_^
		_bool tmpFlase = false; // 화염은 이걸로 갑니다 ^_^

		m_pVIBuffer->Set_RawValue("g_swapDistortion", &tmpTrue, sizeof(_bool));
		if (tmpTrue)
		{
			_float heatHazeSpeed = .1f; 
			m_pVIBuffer->Set_RawValue("g_HeatHazeSpeed", &heatHazeSpeed, sizeof(_float));
			_float heatHazeStrength = 0.01f;
			m_pVIBuffer->Set_RawValue("g_HeatHazeStrength", &heatHazeStrength, sizeof(_float));
		}
		else
		{
			//m_fDistortionTimeAcc = MathHelper::RandF(0.f, 0.0029f);
			m_fDistortionTimeAcc += 0.016f;
			if (m_fDistortionTimeAcc >= 1000.f) 
			{
				m_fDistortionTimeAcc = 0.f;
			}
			m_pVIBuffer->Set_RawValue("g_fTimeDelta", &m_fDistortionTimeAcc, sizeof(_float));

			_float heatHazeSpeed = 2.f;
			m_pVIBuffer->Set_RawValue("g_HeatHazeSpeed", &heatHazeSpeed, sizeof(_float));
			_float heatHazeStrength = 0.05f; 
			m_pVIBuffer->Set_RawValue("g_HeatHazeStrength", &heatHazeStrength, sizeof(_float));
		}

		// 사막의 아지랑이를 표현하자
		m_pVIBuffer->Set_RawValue("g_DesertDistortion", &tmpFlase, sizeof(_bool));
		if (m_pTextureNoise)
			m_pTextureNoise->SetUp_OnShader(m_pVIBuffer, "g_NoiseTexture_HeatHaze", 2);
		m_pVIBuffer->Render(5); // RenderPreBBToBB_Pass
	}

	// ▲ Deferred Process End
	// ----------------------------------


#if 0 // test_particles
	// Restore default states
	ClearRenderStates();

	mTimeStep = 0.016f;
	mAge += 0.016f;
	// 뷰행 던져주자
	_matrix	TransformMatrix = CPipeLine::GetInstance()->Get_Transform(CPipeLine::TS_VIEW);
	TransformMatrix = XMMatrixTranspose(TransformMatrix);
	_float4x4	TransformFloat4x4;
	XMStoreFloat4x4(&TransformFloat4x4, TransformMatrix);
	Set_RawValue("gViewMatrix", &TransformFloat4x4, sizeof(_float4x4));

	// 투행 던져주자
	TransformMatrix = CPipeLine::GetInstance()->Get_Transform(CPipeLine::TS_PROJ);
	TransformMatrix = XMMatrixTranspose(TransformMatrix);
	TransformFloat4x4; 
	XMStoreFloat4x4(&TransformFloat4x4, TransformMatrix);
	Set_RawValue("gProjMatrix", &TransformFloat4x4, sizeof(_float4x4));

	// 카메라 위치 던져주자
	_float4			vCamPosition; 
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	XMStoreFloat4(&vCamPosition, pGameInstance->Get_CamPosition());
	RELEASE_INSTANCE(CGameInstance);

	auto ret = Set_RawValue("gEyePosW", &vCamPosition, sizeof(_float4));

	// Bind Stuffs
	Set_RawValue("gEmitPosW", &mEmitPosW, sizeof(_float3));
	Set_RawValue("gEmitDirW", &mEmitDirW, sizeof(_float3));
	Set_RawValue("gTimeStep", &mTimeStep, sizeof(_float));

	//
	// Set IA stage.
	//
	m_pDeviceContext->IASetInputLayout(m_ParticleLayout);
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT stride = sizeof(VTXPTC);
	UINT offset = 0;

	// On the first pass, use the initialization VB.  Otherwise, use
	// the VB that contains the current particle list.
	if (mFirstRun)
		m_pDeviceContext->IASetVertexBuffers(0, 1, &mInitVB, &stride, &offset);
	else
		m_pDeviceContext->IASetVertexBuffers(0, 1, &mDrawVB, &stride, &offset);

	//
	// Draw the current particle list using stream-out only to update them.  
	// The updated vertices are streamed-out to the target VB. 
	//
	m_pDeviceContext->SOSetTargets(1, &mStreamOutVB, &offset);

	D3DX11_TECHNIQUE_DESC techDesc;
	m_StreamOutTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_StreamOutTech->GetPassByIndex(p)->Apply(0, m_pDeviceContext);

		if (mFirstRun)
		{
			m_pDeviceContext->Draw(1, 0);
			mFirstRun = false;
		}
		else
		{
			m_pDeviceContext->DrawAuto();
		}
	}

	// Restore default states
	ClearRenderStates();


	// done streaming-out--unbind the vertex buffer
	ID3D11Buffer* bufferArray[1] = { 0 };
	m_pDeviceContext->SOSetTargets(1, bufferArray, &offset);

	// ping-pong the vertex buffers
	std::swap(mDrawVB, mStreamOutVB);

	//
	// Draw the updated particle system we just streamed-out. 
	//
	m_pDeviceContext->IASetVertexBuffers(0, 1, &mDrawVB, &stride, &offset);

	m_DrawTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_DrawTech->GetPassByIndex(p)->Apply(0, m_pDeviceContext);

		m_pDeviceContext->DrawAuto(); 
	}
#endif


	// ----------------------------------
	// ▼ Foward Process

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
		// 렌더타겟들 렌더링
		m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_PreBB"), 0);
		m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_Deferred"), 0);
		m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_LightAcc"), 0);
		m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_Shadows"), 4); // ShadowMapDebug_Pass
		m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_Alpha"), 0); 


		// 텍스트 렌더링
		m_pTarget_Manager->PostRender_DebugBuffer(TEXT("MRT_PreBB"), 0, m_spriteBatch, m_spriteFont);
		m_pTarget_Manager->PostRender_DebugBuffer(TEXT("MRT_Deferred"), 0, m_spriteBatch, m_spriteFont);
		m_pTarget_Manager->PostRender_DebugBuffer(TEXT("MRT_LightAcc"), 0, m_spriteBatch, m_spriteFont);
		m_pTarget_Manager->PostRender_DebugBuffer(TEXT("MRT_Shadows"), 0, m_spriteBatch, m_spriteFont);
		m_pTarget_Manager->PostRender_DebugBuffer(TEXT("MRT_Alpha"), 0, m_spriteBatch, m_spriteFont);
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

// Shadow
HRESULT CRenderer::Render_Shadow()
{
	// #1. 지형 그림자 깊이를 찍자
	if (FAILED(m_pTarget_Manager->BindDsvAndSetNullRenderTarget(m_pDeviceContext, TEXT("Target_Shadow_Env"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHA_TERRAIN])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render(3))) // BuildShadowMap_Pass
				return E_FAIL;
		}
	}

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	// #2. 오브젝트들의 그림자를 찍는다.
	if (FAILED(m_pTarget_Manager->BindDsvAndSetNullRenderTarget(m_pDeviceContext, TEXT("Target_Shadow_Obj"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHA_WAR])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render(3))) // BuildShadowMap_Pass
				return E_FAIL;
		}
	}

	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render(3))) // BuildShadowMap_Pass
				return E_FAIL;
		}
	}

	// Restore default states
	ClearRenderStates();
	m_pDeviceContext->RSSetViewports(1, CGraphic_Device::GetInstance()->Get_ViewPortDesc_Ptr());

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonAlpha()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Deferred"))))
		return E_FAIL;

	// War의 경우 RENDER_NONALPHA_WAR 그룹에 들어가여 MRT_Deferred의 Depth_War를 추가로 기록한다.
	// 이는 Render_Blend에서 War가 가려졌을때 외곽선 검출에 사용된다.
	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHA_WAR])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render(1))) // onlyWarPass
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}
	m_RenderObjects[RENDER_NONALPHA_WAR].clear();
	// Restore default states
	ClearRenderStates();

	// 지형찍는다.
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
	
	// 위에서 War를 먼저 찍고 그 다음 다른 객체들을 찍도록 하자.
	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render(0))) // deferredPass
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
	if (nullptr == m_pTarget_Manager ||
		nullptr == m_pLight_Manager)
		return E_FAIL;

	/* 장치에는 Target_Shade가 셋팅된다. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_LightAcc"))))
		return E_FAIL;

	/* Target_Shade에 그린다. */
	m_pLight_Manager->Render();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

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


	// 안개 효과를 위해 픽셀의 world 위치를 알아오자
	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
	_matrix		ViewMatrixInverse = XMMatrixInverse(nullptr, pPipeLine->Get_Transform(CPipeLine::TS_VIEW));
	_matrix		ProjMatrixInverse = XMMatrixInverse(nullptr, pPipeLine->Get_Transform(CPipeLine::TS_PROJ));
	m_pVIBuffer->Set_RawValue("g_ViewMatrixInverse", &XMMatrixTranspose(ViewMatrixInverse), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ProjMatrixInverse", &XMMatrixTranspose(ProjMatrixInverse), sizeof(_float4x4));
	RELEASE_INSTANCE(CPipeLine);

	// Target_Depth_Cur
	m_pVIBuffer->Set_ShaderResourceView("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth_Cur")));

	// Target_Depth_Prev
	m_pVIBuffer->Set_ShaderResourceView("g_DepthTexture_War", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth_Prev")));

	// EmissiveTexture
	m_pVIBuffer->Set_ShaderResourceView("g_EmissiveTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Emissive")));

	// HitPowerTexture
	m_pVIBuffer->Set_ShaderResourceView("g_HitPowerTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_HitPower")));

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
	Safe_Release(m_pTextureNoise);

	Safe_Release(mInitVB);
	Safe_Release(mDrawVB);
	Safe_Release(mStreamOutVB);
}

