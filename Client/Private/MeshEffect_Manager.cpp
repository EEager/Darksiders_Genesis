#include "stdafx.h"

#include "MeshEffect_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CMeshEffect_Manager)

CMeshEffect_Manager::CMeshEffect_Manager()
{

}

void CMeshEffect_Manager::Initialize()
{
	// Effect_War_Skill_1
	m_EffectCallBack.emplace(L"Effect_War_Skill_1",bind(&CMeshEffect_Manager::Effect_War_Skill_1, this, placeholders::_1));
}

void CMeshEffect_Manager::Add_MeshEffects_To_Layer(const _tchar* pMeshEffectTag, _float fTimeDelta, _float3* vPos)
{
	auto iterFind = find_if(m_EffectCallBack.begin(), m_EffectCallBack.end(), CTagFinder(pMeshEffectTag));
	if (iterFind == m_EffectCallBack.end())
		assert(0);
	return iterFind->second(fTimeDelta);
}


bool targetWarOnce;
void CMeshEffect_Manager::Effect_War_Skill_1(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (targetWarOnce == false)
	{
		m_pWarTransform = static_cast<CTransform*>(pGameInstance->Get_War()->Get_ComponentPtr(L"Com_Transform"));
		targetWarOnce = true;
	}
	// #1. 먼저 검들을 생성한다.
	// War 스킬 1번이다. War 약간 밑에서 칼을 사방으로 생성하자
	_vector vWarPos = m_pWarTransform->Get_State(CTransform::STATE_POSITION);

	for (int i = 0; i < 15; i++)
	{ 
		CHAOSEATERDESC tempDesc; 
		tempDesc.vPos = vWarPos + XMVectorSet(MathHelper::RandF(-2.f, 2.f),
				-0.f,	MathHelper::RandF(-2.f, 2.f), 0.f);
			
		// 칼이 보는 방향은 
		_vector vDir = XMVector3Normalize(tempDesc.vPos - vWarPos);
		tempDesc.vLook = vWarPos + vDir * MathHelper::RandF(10.f, 15.f);
		tempDesc.vLook = XMVectorSetY(tempDesc.vLook, 20.f);

		if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_MeshEffect", TEXT("Prototype_GameObject_MeshEffect_War_ChaosEater"), &tempDesc)))
			assert(0);
	}

	// #2. 그다음 공기팡을 생성합니다.
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_MeshEffect", TEXT("Prototype_GameObject_MeshEffect_Sphere"), &vWarPos)))
		assert(0);

	RELEASE_INSTANCE(CGameInstance);
}

void CMeshEffect_Manager::Free()
{
}
