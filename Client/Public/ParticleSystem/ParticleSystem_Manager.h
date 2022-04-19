#pragma once

#include "ParticleSystem/ParticleSystem.h"

// ��ƼŬ �ý��۵��� ��Ƶΰ� �ִ�. 
// �÷��̾ ���Ͱ� "Particle_Blood" �Ǵ� "Particle_�� ������Ʈ �Ŵ������� �ְ� �����޶�� �Ұ��̴�.
// ������Ʈ Ǯ������ �̸� �����ϵ��� �ϴ� ������ �ϰ� �ִ�. 

BEGIN(Client)

class CParticleSystem_Manager final : public CBase
{
	DECLARE_SINGLETON(CParticleSystem_Manager)
private:
	CParticleSystem_Manager();
	virtual ~CParticleSystem_Manager() = default;

public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext); // Loader���� ������ϴ°��ΰ�..? 
	void Add_Particle_To_Layer(const _tchar* pParticleTag, _float3* vPos = nullptr); // �÷��̾ �̰��� ȣ���Ұ��̴�.
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