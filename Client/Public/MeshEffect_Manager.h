#pragma once

#include "MeshEffect.h"

BEGIN(Client)

class CMeshEffect_Manager final : public CBase
{
	DECLARE_SINGLETON(CMeshEffect_Manager)
public:
	CMeshEffect_Manager();
	virtual ~CMeshEffect_Manager() = default;

public:
	// 초기화는 Loader에서 진행하자. 그냥 콜백함수 등록만 시켜주는 역할을 한다.
	void Initialize(); 

	
	// 등록된 콜백함수를 호출하는 기능이다. 플레이어가 이것을 호출할것이다.
	// 필요한 경우 이펙트를 생성할 위치도 지정하자
	void Add_MeshEffects_To_Layer(const _tchar* pMeshEffectTag, _float fTimeDelta, _float3* vPos = nullptr);

private:
	// Tag로 관리되는 이펙트 콜백함수들이다.
	unordered_map<const _tchar*, function<void(_float deltaTime)>>  m_EffectCallBack;

	void Effect_War_Skill_1(_float fTimeDelta);
	class CTransform* m_pWarTransform = nullptr;
public:
	virtual void Free();
};
END