#include "stdafx.h"
#include "ParticleSystem\ParticleSystem_Manager.h"

#include "Object_Manager.h"

IMPLEMENT_SINGLETON(CParticleSystem_Manager)

CParticleSystem_Manager::CParticleSystem_Manager()
{

}

// Ŭ�󿡼� �ش� �޼ҵ带 ���� ������Ʈ �Ŵ����� ����� �޶�� ����
void CParticleSystem_Manager::Add_Particle_To_Layer(const _tchar* pParticleTag)
{
	auto pPTCVector = Find_PTC(pParticleTag);
	if (pPTCVector == nullptr)
		return;

	auto pAvailableIdx = Find_AvailableIdx(pParticleTag); // ���⼭���� Ž������
	if (pAvailableIdx == nullptr)
		return;

	int loopCnt = 0;
	CParticleSystem* pParticle = nullptr;
	while (loopCnt < pPTCVector->size())
	{
		pParticle = (*pPTCVector)[*pAvailableIdx];
		if (pParticle->m_isAvailable) // ��밡���ϴٸ� �װ��� �Ѱ�����
			break;
		*pAvailableIdx = (*pAvailableIdx + 1) % pPTCVector->size(); // ��밡���Ѱ��� �ִ��� ��ȸ����
		loopCnt++;
	}
	if (pParticle == nullptr) // �� �� ���Ҵµ��� ���ٸ� ��밡���Ѱ��� ���°��̴�.
		return;

	// pParticle�� �ʱ�ȭ ��������
	pParticle->m_isDead = false; // ���� �׾��־��ٸ� �ٽ� ������� 
	pParticle->m_isAvailable = false; // ȭ��� ��������� �ٲ��ְ�

	// ������Ʈ �Ŵ����� �������
	auto ret = CObject_Manager::GetInstance()->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Particles", pParticle, L"Prototype_ParticleSystem");
	if (ret < 0)
		assert(0);
	return;
}

// Initalize�� �ᱹ Gameplay Loader���� �ؾ��ϴ� ���ΰ�? 
void CParticleSystem_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{

	for (int i = 0; i < 10; i++)
	{
		CParticleSystem* pParticle = static_cast<CParticleSystem*>(CParticle_Sword::Create(pDevice, pDeviceContext, L"../Bin/ShaderFiles/Shader_Effect_Metal.hlsl", 30/*��ƼŬ��������*/));
		pParticle->mEmitLoop = false;
		pParticle->mMaxAge = 2.f;
		Add_ParticleSystem(L"Particle_Sword", pParticle);
	}
	m_ParticleSystemsIndex.emplace(L"Particle_Sword", 0);
}


// m_ParticleSystems �� pParticleTag�� �ش��ϴ� ���� pParticle�� �־�����
HRESULT CParticleSystem_Manager::Add_ParticleSystem(const _tchar* pParticleTag, CParticleSystem* pParticle)
{
	vector<CParticleSystem*>* pPTCVector = Find_PTC(pParticleTag);
	if (nullptr == pPTCVector) // ������ ���� ���� �־�����.
	{
		vector<CParticleSystem*>  PTCVector;
		PTCVector.push_back(pParticle);
		m_ParticleSystems.emplace(pParticleTag, PTCVector);
	}
	else
		pPTCVector->push_back(pParticle);
	return 0;
}


// pParticleTag�� �ش��ϴ� vector<CParticleSystem*>�� �����͸� ��ȯ����
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
