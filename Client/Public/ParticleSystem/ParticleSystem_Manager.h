#pragma once

#include "ParticleSystem/ParticleSystem.h"

// 파티클 시스템들을 모아두고 있다. 
// 플레이어나 몬스터가 "Particle_Blood" 또는 "Particle_를 오브젝트 매니져한테 넣고 돌려달라고 할것이다.
// 오브젝트 풀링으로 이를 수행하도록 하는 역할을 하고 있다. 

BEGIN(Client)

class CParticleSystem_Manager final : public CBase
{
	DECLARE_SINGLETON(CParticleSystem_Manager)
private:
	CParticleSystem_Manager();
	virtual ~CParticleSystem_Manager() = default;

public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext); // Loader에서 해줘야하는것인가..? 
	void Add_Particle_To_Layer(const _tchar* pParticleTag, _float3* vPos = nullptr); // 플레이어가 이것을 호출할것이다.
	class CParticleSystem* Get_Particle_Available(const _tchar* pParticleTa, int idx);

private:
	HRESULT Add_ParticleSystem(const _tchar* pParticleTag, class CParticleSystem*);
	vector<CParticleSystem*>* Find_PTC(const _tchar* pParticleTag);
	int* Find_AvailableIdx(const _tchar* pParticleTag);

private:
	unordered_map<const _tchar*, vector<CParticleSystem*>>				m_ParticleSystems;
	typedef unordered_map<const _tchar*, vector<CParticleSystem*>>		PTCs;
	unordered_map<const _tchar*, int>				m_ParticleSystemsIndex;
private:

public:
	virtual void Free();
};
END