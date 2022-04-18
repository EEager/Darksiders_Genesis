#include "stdafx.h"
#include "..\public\ParticleSystem\ParticleSystem.h"

#include "GameInstance.h"
#include "d3dx11effect.h"


#ifdef USE_IMGUI
#include "imgui_Manager.h"
#endif

CParticleSystem::CParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CParticleSystem::CParticleSystem(const CParticleSystem& rhs)
	: CGameObject(rhs)
{
}

ID3D11ShaderResourceView* CreateRandomTexture1DSRV(ID3D11Device* device)
{
	// 
	// Create the random data.
	//
	XMFLOAT4 randomValues[1024];

	for (int i = 0; i < 1024; ++i)
	{
		randomValues[i].x = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].y = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].z = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].w = MathHelper::RandF(-1.0f, 1.0f);
	}

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = randomValues;
	initData.SysMemPitch = 1024 * sizeof(XMFLOAT4);
	initData.SysMemSlicePitch = 0;

	//
	// Create the texture.
	//
	D3D11_TEXTURE1D_DESC texDesc;
	texDesc.Width = 1024;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;

	ID3D11Texture1D* randomTex = 0;
	HR(device->CreateTexture1D(&texDesc, &initData, &randomTex));

	//
	// Create the resource view.
	//
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture1D.MostDetailedMip = 0;

	ID3D11ShaderResourceView* randomTexSRV = 0;
	HR(device->CreateShaderResourceView(randomTex, &viewDesc, &randomTexSRV));

	ReleaseCOM(randomTex);

	return randomTexSRV;
}

HRESULT CParticleSystem::NativeConstruct_Prototype(const _tchar* pShaderFilePath, int maxParticleNum)
{	
	// #1. Create Effect
	_uint		iFlag = 0;
#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif // _DEBUG	
	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	m_StreamOutTech = m_pEffect->GetTechniqueByName("StreamOutTech");
	m_DrawTech = m_pEffect->GetTechniqueByName("DrawTech");

	// #2. Create Layout
	_uint iNumElements = 5;
	D3D11_INPUT_ELEMENT_DESC		ElementDesc[] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"AGE",      0, DXGI_FORMAT_R32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TYPE",     0, DXGI_FORMAT_R32_UINT,        0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	D3DX11_PASS_DESC passDesc;
	m_StreamOutTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(m_pDevice->CreateInputLayout(ElementDesc, iNumElements, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_ParticleLayout));

	// #3. Init Stuffs
	mFirstRun = true; // 처음 정점은 emitter이기에 이걸로 구분해주자.
	mTimeStep = 0.0f; // GS Stream에서 각 정점 파티클 생존시간 계산하기 위한 게임 dt.
	mAge = 0.0f; // 현재 이 파티클시스템의 경과시간.

	mMaxParticles = maxParticleNum; //  maxParticles;
	mEmitPosW = _float3(17.f, 1.f, 430.f); // 파티클 시작 방출 위치. 
	mEmitColor = _float3(1.f, 1.f, 1.f);
	mEmitSize = _float2(0.1f, 0.1f);

	mEmitInitAccel = _float3( 0.0f, 7.8f, 0.0f );

	mEmitRandomPower = 4.f;

	mRandomTexSRV = CreateRandomTexture1DSRV(m_pDevice);

	BuildVB(m_pDevice);

	SetUp_Component();

	return S_OK;
}


HRESULT CParticleSystem::NativeConstruct(void * pArg)
{
	return S_OK;
}

_int CParticleSystem::Tick(_float fTimeDelta)
{
	if (m_isDead)
	{
		// 죽어서 반납하기 전에 몇가지를 초기화해야한다
		mFirstRun = true; // 처음에는 emitter를 넣는거 알지? 
		mAge = 0.f; // 파티클시스템 생존시간 초기화
		m_isAvailable = true;  // 화장실 사용가능하도록하고 반납
		// mGameTime는 초기화하지말자
		return -1;
	}

	mTimeStep = fTimeDelta;
	mAge += fTimeDelta;
	mGameTime += fTimeDelta;

	if (CInput_Device::GetInstance()->Key_Down(DIK_P))
	{
		mFirstRun = true;
		mAge = 0.f;
	}

	return _int();
}

_int CParticleSystem::LateTick(_float fTimeDelta)
{
	// 만약 Loop가 아닌경우에는 mAge를 계속 체크를 해줘야한다. 
	if (mEmitLoop == false)
	{
		if (mAge > mMaxAge) 
		{
			m_isDead = true;
			return 0;
		}
	}


	if (nullptr == m_pRendererCom)
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHA, this))) 
		return -1;
	if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
		return -1;
	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

HRESULT CParticleSystem::Render(_uint iPassIndex)
{
	m_pRendererCom->ClearRenderStates();

	if (FAILED(SetUp_ConstantTable(iPassIndex)))
		return E_FAIL;

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

	m_DrawTech->GetPassByIndex(0)->Apply(0, m_pDeviceContext);

	m_pDeviceContext->DrawAuto();
	//for (UINT p = 0; p < techDesc.Passes; ++p)
	//{
	//	m_DrawTech->GetPassByIndex(p)->Apply(0, m_pDeviceContext);

	//	m_pDeviceContext->DrawAuto();
	//}

	return S_OK;
}

HRESULT CParticleSystem::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
#ifdef USE_IMGUI
	// Imgui 로 시험하자.
	char buf[32];
	sprintf_s(buf, "ParticleSystem##%d", m_CloneIdx);
	ImGui::Begin(buf);
	{
		// 텍스처 어떤거 사용할지. 
		const char* textureItems[] = { "m_pTextureParticle", "m_pTextureDecayPuffs", "m_pTextureDust",  "m_pTextureFogCloud" ,"m_pTextureRockChips" };
		ImGui::Combo("Texture Select", &mTextureTagIdx, textureItems, IM_ARRAYSIZE(textureItems));

		// 텍스쳐 인덱스
		ImGui::InputInt("TextureIdx", &m_iTextureIdx);

		// mEmitPosW
		float EmitPos[3] = { mEmitPosW.x, mEmitPosW.y,mEmitPosW.z };
		if (ImGui::DragFloat3("EmitPos", EmitPos, .01f))
			mEmitPosW = _float3(EmitPos[0], EmitPos[1], EmitPos[2]);

		// mEmitColor;
		float EmitColor[3] = { mEmitColor.x, mEmitColor.y,mEmitColor.z };
		if (ImGui::DragFloat3("mEmitColor", EmitColor, .01f))
			mEmitColor = _float3(EmitColor[0], EmitColor[1], EmitColor[2]);

		// mEmitInitAccel;
		float EmitAccel[3] = { mEmitInitAccel.x, mEmitInitAccel.y,mEmitInitAccel.z };
		if (ImGui::DragFloat3("mEmitInitAccel", EmitAccel, .01f))
			mEmitInitAccel = _float3(EmitAccel[0], EmitAccel[1], EmitAccel[2]);

		// mEmitSize;
		float EmitSize[2] = { mEmitSize.x, mEmitSize.y };
		if (ImGui::DragFloat2("mEmitSize", EmitSize, .01f))
			mEmitSize = _float2(EmitSize[0], EmitSize[1]);

		// mEmitRandomPower
		ImGui::DragFloat("mEmitRandomPower", &mEmitRandomPower, 0.01f);
	}
	ImGui::End();
#endif
	return S_OK;
}


HRESULT CParticleSystem::SetUp_Component()
{
	///* For.Com_Transform */
	//CTransform::TRANSFORMDESC		TransformDesc;
	//ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	//TransformDesc.fSpeedPerSec = 7.f;
	//TransformDesc.fRotationPerSec = XMConvertToRadians(10.0f);

	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)m_pTransformCom.GetAddressOf(), &TransformDesc)))
	//	return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)m_pRendererCom.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Texture1 */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle"), TEXT("Com_Texture1"), (CComponent**)m_pTextureParticle.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Texture2 */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DecayPuffs"), TEXT("Com_Texture2"), (CComponent**)m_pTextureDecayPuffs.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Texture3 */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Dust"), TEXT("Com_Texture3"), (CComponent**)m_pTextureDust.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Texture4 */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FogCloud"), TEXT("Com_Texture4"), (CComponent**)m_pTextureFogCloud.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Texture5 */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RockChips"), TEXT("Com_Texture5"), (CComponent**)m_pTextureRockChips.GetAddressOf())))
		return E_FAIL;



	return S_OK;
}

HRESULT CParticleSystem::Set_RawValue(const char* pConstantName, void* pData, _uint iSize)
{
	if (nullptr == m_pEffect)
		return E_FAIL;
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iSize);
}

HRESULT CParticleSystem::SetUp_ConstantTable(_uint iPassIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Bind Transform
	{
		//// 월행 던져주자
		//_matrix MyWorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldFloat4x4Ptr());
		//_matrix	WorldMatrixTransPose = XMMatrixTranspose(MyWorldMatrix);
		//_float4x4		WorldMatrix;
		//XMStoreFloat4x4(&WorldMatrix, WorldMatrixTransPose);
		//Set_RawValue("gWorldMatrix", &WorldMatrix, sizeof(_float4x4));

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
	}

	// 카메라 위치 던져주자
	_float4			vCamPosition;
	XMStoreFloat4(&vCamPosition, pGameInstance->Get_CamPosition());
	auto ret = Set_RawValue("gEyePosW", &vCamPosition, sizeof(_float4));

	// Bind Stuffs
	Set_RawValue("gEmitPosW", &mEmitPosW, sizeof(_float3));
	Set_RawValue("gEmitColor", &mEmitColor, sizeof(_float3));
	Set_RawValue("gAccelW", &mEmitInitAccel, sizeof(_float3)); // 초기속도
	Set_RawValue("gRandomPwr", &mEmitRandomPower, sizeof(_float)); // 랜덤방향세기. 세질수록 초기속도 무시할 수 있음.
	Set_RawValue("gEmitSize", &mEmitSize, sizeof(_float2));
	Set_RawValue("gTimeStep", &mTimeStep, sizeof(_float));
	Set_RawValue("gGameTime", &mGameTime, sizeof(_float));

	// 기타
	Set_RawValue("useLoop", &mEmitLoop, sizeof(_bool));
	Set_RawValue("maxAge", &mMaxAge, sizeof(_float));

	// 랜덤방향의 랜덤값 던져주자. 방향으로 지정할 수도 있음.
	_vector randFloat3 = MathHelper::RandUnitVec3();
	Set_RawValue("gRandomDir", &randFloat3, sizeof(_vector));

	// g_DiffuseTexture
	ID3DX11EffectShaderResourceVariable* pValiable = m_pEffect->GetVariableByName("g_DiffuseTexture")->AsShaderResource();
#ifdef USE_IMGUI
	ID3D11ShaderResourceView* pResource = nullptr;
	switch (mTextureTagIdx)
	{
	case 0:
		pResource = m_pTextureParticle->Get_SRV(m_iTextureIdx);
		break;
	case 1:
		pResource = m_pTextureDecayPuffs->Get_SRV(m_iTextureIdx);
		break;
	case 2:
		pResource = m_pTextureDust->Get_SRV(m_iTextureIdx);
		break;
	case 3:
		pResource = m_pTextureFogCloud->Get_SRV(m_iTextureIdx);
		break;
	case 4:
		pResource = m_pTextureRockChips->Get_SRV(m_iTextureIdx);
		break;
	default:
		assert(0);
	}
#else
	auto pResource = m_pTextureParticle->Get_SRV(m_iTextureIdx);
#endif
	pValiable->SetResource(pResource);


	// g_RandomTex
	pValiable = m_pEffect->GetVariableByName("g_RandomTex")->AsShaderResource();
	pValiable->SetResource(mRandomTexSRV);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


void CParticleSystem::BuildVB(ID3D11Device* device)
{
	//
	// Create the buffer to kick-off the particle system.
	//
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

	HR(device->CreateBuffer(&vbd, &vinitData, &mInitVB));

	//
	// Create the ping-pong buffers for stream-out and drawing.
	//
	vbd.ByteWidth = sizeof(VTXPTC) * mMaxParticles;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

	HR(device->CreateBuffer(&vbd, 0, &mDrawVB));
	HR(device->CreateBuffer(&vbd, 0, &mStreamOutVB));
}


// Loader에서 미리 만들어놓고, 파티클매니져에서 오브젝트 풀링으로, 클라한테 하나씩 넘겨주자.
CParticleSystem * CParticleSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, int maxParticleNum)
{
	CParticleSystem*		pInstance = new CParticleSystem(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, maxParticleNum)))
	{
		MSG_BOX("Failed to Created CParticleSystem");
		Safe_Release(pInstance);
	}

	return pInstance;
}


// 필요할때 파티클시스템 매니져에서 알아서 복사만 잘 해줄것이다.
CGameObject * CParticleSystem::Clone(void* pArg)
{
	return nullptr;
}

void CParticleSystem::Free()
{
	__super::Free(); 
	Safe_Release(mInitVB);
	Safe_Release(mRandomTexSRV);
	Safe_Release(mDrawVB);
	Safe_Release(mStreamOutVB);
	Safe_Release(m_StreamOutTech); 
	Safe_Release(m_DrawTech);
	Safe_Release(m_ParticleLayout);
}






// -----------------------------------
// CParticle_Sword
// -----------------------------------
CParticle_Sword::CParticle_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CParticleSystem(pDevice, pDeviceContext)
{
}

CParticle_Sword::CParticle_Sword(const CParticle_Sword& rhs)
	: CParticleSystem(rhs)
{
}

HRESULT CParticle_Sword::NativeConstruct_Prototype(const _tchar* pShaderFilePath, int maxParticleNum)
{
	return CParticleSystem::NativeConstruct_Prototype(pShaderFilePath, maxParticleNum);
}

HRESULT CParticle_Sword::NativeConstruct(void* pArg)
{
	// not used
	return E_NOTIMPL;
}

_int CParticle_Sword::Tick(_float fTimeDelta)
{
	if (CParticleSystem::Tick(fTimeDelta) < 0)
		return -1;

	// mEmitPosW는 플레이어 칼 위치를 따라간다.

	return 0;
}

_int CParticle_Sword::LateTick(_float fTimeDelta)
{
	if (CParticleSystem::LateTick(fTimeDelta) < 0)
		return -1;

	return 0;
}

HRESULT CParticle_Sword::Render(_uint iPassIndex)
{
	if (CParticleSystem::Render(iPassIndex) < 0)
		return -1;

	return 0;
}

HRESULT CParticle_Sword::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	if (CParticleSystem::PostRender(m_spriteBatch, m_spriteFont) < 0)
		return -1;

	return 0;
}


CParticle_Sword* CParticle_Sword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, int maxParticleNum)
{
	CParticle_Sword* pInstance = new CParticle_Sword(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, maxParticleNum)))
	{
		MSG_BOX("Failed to Created CParticle_Sword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParticle_Sword::Clone(void* pArg)
{
	return nullptr;
}

void CParticle_Sword::Free()
{
	CParticleSystem::Free();
}
