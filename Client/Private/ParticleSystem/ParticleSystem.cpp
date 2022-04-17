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

HRESULT CParticleSystem::NativeConstruct_Prototype(const _tchar* pShaderFilePath)
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

	mMaxParticles = 500; //  maxParticles;
	mEmitPosW = _float3(17.f, 1.f, 430.f); // 파티클 시작 방출 위치. 
	mEmitDirW = _float3(0.0f, 1.0f, 0.0f); // 파티클 시작 가속도.

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
	mTimeStep = fTimeDelta;
	mAge += fTimeDelta;

	return _int();
}

_int CParticleSystem::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHA, this))) 
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
	// the VB that contains the current particle list.ㅍ
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
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_DrawTech->GetPassByIndex(p)->Apply(0, m_pDeviceContext);

		m_pDeviceContext->DrawAuto();
	}

	return S_OK;
}

HRESULT CParticleSystem::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{


	return S_OK;
}


HRESULT CParticleSystem::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 7.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(10.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)m_pTransformCom.GetAddressOf(), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)m_pRendererCom.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_Flare"), TEXT("Com_Texture"), (CComponent**)m_pTextureCom.GetAddressOf())))
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
		// 월행 던져주자
		_matrix MyWorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldFloat4x4Ptr());
		_matrix	WorldMatrixTransPose = XMMatrixTranspose(MyWorldMatrix);
		_float4x4		WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, WorldMatrixTransPose);
		Set_RawValue("gWorldMatrix", &WorldMatrix, sizeof(_float4x4));

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
	Set_RawValue("gEmitDirW", &mEmitDirW, sizeof(_float3));
	Set_RawValue("gTimeStep", &mTimeStep, sizeof(_float));

	// 랜덤값 던져주자
	_vector randFloat3 = MathHelper::RandUnitVec3();
	Set_RawValue("gRandom", &randFloat3, sizeof(_vector));

	// 텍스쳐 던져주자
	ID3DX11EffectShaderResourceVariable* pValiable = m_pEffect->GetVariableByName("g_DiffuseTexture")->AsShaderResource();
	if (nullptr == pValiable)
		return E_FAIL;
	pValiable->SetResource(m_pTextureCom->Get_SRV(0));

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
CParticleSystem * CParticleSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath)
{
	CParticleSystem*		pInstance = new CParticleSystem(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath)))
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
	Safe_Release(mDrawVB);
	Safe_Release(mStreamOutVB);
	Safe_Release(m_StreamOutTech);
	Safe_Release(m_DrawTech);
	Safe_Release(m_ParticleLayout);
}
