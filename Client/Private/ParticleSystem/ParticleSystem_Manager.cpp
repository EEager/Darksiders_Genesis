#include "stdafx.h"
#include "ParticleSystem\ParticleSystem_Manager.h"

#include "Object_Manager.h"

IMPLEMENT_SINGLETON(CParticleSystem_Manager)

CParticleSystem_Manager::CParticleSystem_Manager()
{

}

// 클라에서 해당 메소드를 통해 오브젝트 매니져에 등록해 달라고 하자
void CParticleSystem_Manager::Add_Particle_To_Layer(const _tchar* pParticleTag, _float3* vPos)
{
	auto pPTCVector = Find_PTC(pParticleTag);
	if (pPTCVector == nullptr)
		return;

	auto pAvailableIdx = Find_AvailableIdx(pParticleTag); // 여기서부터 탐색하자
	if (pAvailableIdx == nullptr)
		return;

	int loopCnt = 0;
	CParticleSystem* pParticle = nullptr;
	while (loopCnt < pPTCVector->size())
	{
		pParticle = (*pPTCVector)[*pAvailableIdx];
		if (pParticle->m_isAvailable) // 사용가능하다면 그것을 넘겨주자
			break;
		*pAvailableIdx = (*pAvailableIdx + 1) % pPTCVector->size(); // 사용가능한것이 있는지 순회하자
		loopCnt++;
	}
	if (pParticle == nullptr) // 룹 다 돌았는데도 없다면 사용가능한것이 없는것이다.
		return;

	// pParticle를 초기화 시켜주자
	pParticle->m_isDead = false; // 만약 죽어있었다면 다시 살려내고 
	pParticle->m_isAvailable = false; // 화장실 사용중으로 바꿔주고
	if (vPos) // 초기화 위치가 있다면 설정해주자.
		pParticle->mEmitPosW = *vPos;


	// 오브젝트 매니져에 등록하자. 참고로 이 함수는 프로토타입을 넣어주는 것이 아니라. set만 하는것이다.
	auto ret = CObject_Manager::GetInstance()->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Particles", pParticle, L"Prototype_ParticleSystem");
	if (ret < 0)
		assert(0);
	return;
}

// Initalize를 결국 Gameplay Loader에서 해야하는 것인가? 
void CParticleSystem_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	//#############################################################
	// 이건 테스트용입니다.
	// Particle_Sword
	for (int i = 0; i < 10; i++)
	{
		CParticleSystem* pParticle = CParticle_Sword::Create(pDevice, pDeviceContext, L"../Bin/ShaderFiles/Shader_Effect_Particle.hlsl", 10/*mMaxParticles*/);
		Add_ParticleSystem(L"Particle_Sword", pParticle);
	}
	m_ParticleSystemsIndex.emplace(L"Particle_Sword", 0);
	//#############################################################

	// Particle_Blood
	for (int i = 0; i < 20; i++)
	{
		CParticleSystem* pParticle = CParticle_Blood::Create(pDevice, pDeviceContext, L"../Bin/ShaderFiles/Shader_Effect_Particle.hlsl", 50/*mMaxParticles*/);
		Add_ParticleSystem(L"Particle_Blood", pParticle);
	}
	m_ParticleSystemsIndex.emplace(L"Particle_Blood", 0);

	// Particle_LightAtk4
	for (int i = 0; i < 3; i++)
	{
		CParticleSystem* pParticle = CParticle_LightAtk4::Create(pDevice, pDeviceContext, L"../Bin/ShaderFiles/Shader_Effect_Particle.hlsl", 300/*mMaxParticles*/);
		Add_ParticleSystem(L"Particle_LightAtk4", pParticle);
	}
	m_ParticleSystemsIndex.emplace(L"Particle_LightAtk4", 0);

	// Particle_AirLand
	for (int i = 0; i < 3; i++)
	{
		CParticleSystem* pParticle = CParticle_LightAtk4::Create(pDevice, pDeviceContext, L"../Bin/ShaderFiles/Shader_Effect_Particle.hlsl", 300/*mMaxParticles*/);
		Add_ParticleSystem(L"Particle_AirLand", pParticle);
	}
	m_ParticleSystemsIndex.emplace(L"Particle_AirLand", 0);

	// Particle_War_Dash_Horse
	for (int i = 0; i < 4; i++)
	{
		CParticleSystem* pParticle = CParticle_War_Dash_Horse::Create(pDevice, pDeviceContext, L"../Bin/ShaderFiles/Shader_Effect_Particle.hlsl", 500/*mMaxParticles*/);
		Add_ParticleSystem(L"Particle_War_Dash_Horse", pParticle);
	}
	m_ParticleSystemsIndex.emplace(L"Particle_War_Dash_Horse", 0);

	// Particle_Box
	for (int i = 0; i < 10; i++)
	{
		CParticleSystem* pParticle = CParticle_Box::Create(pDevice, pDeviceContext, L"../Bin/ShaderFiles/Shader_Effect_Particle.hlsl", 10/*mMaxParticles*/);
		Add_ParticleSystem(L"Particle_Box", pParticle);
	}
	m_ParticleSystemsIndex.emplace(L"Particle_Box", 0);
}


// m_ParticleSystems 내 pParticleTag에 해당하는 곳에 pParticle를 넣어주자
HRESULT CParticleSystem_Manager::Add_ParticleSystem(const _tchar* pParticleTag, CParticleSystem* pParticle)
{
	vector<CParticleSystem*>* pPTCVector = Find_PTC(pParticleTag);
	if (nullptr == pPTCVector) // 없으면 새로 만들어서 넣어주자.
	{
		vector<CParticleSystem*>  PTCVector;
		PTCVector.push_back(pParticle);
		m_ParticleSystems.emplace(pParticleTag, PTCVector);
	}
	else
		pPTCVector->push_back(pParticle);
	return 0;
}

CParticleSystem* CParticleSystem_Manager::Get_Particle_Available(const _tchar* pParticleTag, int idx)
{
	vector<CParticleSystem*>* pPTCVector = Find_PTC(pParticleTag);
	if (nullptr == pPTCVector)
		return nullptr;

	return (*pPTCVector)[idx];
}


// pParticleTag에 해당하는 vector<CParticleSystem*>의 포인터를 반환하자
vector<CParticleSystem*>* CParticleSystem_Manager::Find_PTC(const _tchar* pParticleTag)
{
	auto iter = find_if(m_ParticleSystems.begin(), m_ParticleSystems.end(), CTagFinder(pParticleTag));

	if (iter == m_ParticleSystems.end())
		return nullptr;

	return &iter->second;
}

int* CParticleSystem_Manager::Find_AvailableIdx(const _tchar* pParticleTag)
{
	auto iter = find_if(m_ParticleSystemsIndex.begin(), m_ParticleSystemsIndex.end(), CTagFinder(pParticleTag));

	if (iter == m_ParticleSystemsIndex.end())
		return nullptr;

	return &iter->second;
}

void CParticleSystem_Manager::Free()
{
	for (auto& Pair : m_ParticleSystems)
	{
		for (auto& pParticle : Pair.second)
		{
			Safe_Release(pParticle);
		}
		Pair.second.clear();
	}

	m_ParticleSystems.clear();
}
