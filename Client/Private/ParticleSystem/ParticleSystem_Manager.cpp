#include "stdafx.h"
#include "ParticleSystem\ParticleSystem_Manager.h"

#include "Object_Manager.h"

IMPLEMENT_SINGLETON(CParticleSystem_Manager)

CParticleSystem_Manager::CParticleSystem_Manager()
{

}

// 클라에서 해당 메소드를 통해 오브젝트 매니져에 등록해 달라고 하자
void CParticleSystem_Manager::Add_Particle_To_Layer(const _tchar* pParticleTag)
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

	// 오브젝트 매니져에 등록하자
	auto ret = CObject_Manager::GetInstance()->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Particles", pParticle, L"Prototype_ParticleSystem");
	if (ret < 0)
		assert(0);
	return;
}

// Initalize를 결국 Gameplay Loader에서 해야하는 것인가? 
void CParticleSystem_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{

	for (int i = 0; i < 10; i++)
	{
		CParticleSystem* pParticle = static_cast<CParticleSystem*>(CParticle_Sword::Create(pDevice, pDeviceContext, L"../Bin/ShaderFiles/Shader_Effect_Metal.hlsl", 30/*파티클정점개수*/));
		pParticle->mEmitLoop = false;
		pParticle->mMaxAge = 2.f;
		Add_ParticleSystem(L"Particle_Sword", pParticle);
	}
	m_ParticleSystemsIndex.emplace(L"Particle_Sword", 0);
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
