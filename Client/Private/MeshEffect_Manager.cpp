#include "stdafx.h"

#include "MeshEffect_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CMeshEffect_Manager)

bool targetWarOnce;
bool targetBossOnce;

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


void CMeshEffect_Manager::Effect_War_Skill_1(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (targetWarOnce == false)
	{
		m_pWarTransform = static_cast<CTransform*>(pGameInstance->Get_War()->Get_ComponentPtr(L"Com_Transform"));
		targetWarOnce = true;
	}
	_vector vWarPos = m_pWarTransform->Get_State(CTransform::STATE_POSITION);

	_vector vDecal = vWarPos + XMVectorSet(0.f, 0.15f, 0.f, 0.f);
	// #3. 그다음 데칼을 생성합니다.
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Decal", TEXT("Prototype_GameObject_Decal"), &vDecal)))
		assert(0);

	// #1. 먼저 검들을 생성한다.
	// War 스킬 1번이다. War 약간 밑에서 칼을 사방으로 생성하자
	// 안쪽은 6개. 길이는 2.
	_float offset = 1.5f;
	for (int i = 0; i < 6; i++)
	{ 
		CHAOSEATERDESC tempDesc; 
		tempDesc.vPos = vWarPos + XMVectorSet(offset * XMScalarCos(XMConvertToRadians(i * 60.f)), -0.75f,	offset * XMScalarSin(XMConvertToRadians(i * 60.f)), 0.f);
			
		// 칼이 보는 방향은 
		_vector vDir = XMVector3Normalize(tempDesc.vPos - vWarPos);
		tempDesc.vLook = vWarPos + vDir * 10.f;
		tempDesc.vLook = XMVectorSetY(tempDesc.vLook, XMVectorGetY(vWarPos) + 20.f);

		if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_MeshEffect", TEXT("Prototype_GameObject_MeshEffect_War_ChaosEater"), &tempDesc)))
			assert(0);
	}

	// 바깥쪽은은 8개. 길이는 4.
	offset = 3.f;
	for (int i = 0; i < 8; i++)
	{
		CHAOSEATERDESC tempDesc;
		tempDesc.vPos = vWarPos + XMVectorSet(offset * XMScalarCos(XMConvertToRadians(i * 45.f)), 0.f, offset * XMScalarSin(XMConvertToRadians(i * 45.f)), 0.f);

		// 칼이 보는 방향은 
		_vector vDir = XMVector3Normalize(tempDesc.vPos - vWarPos);
		tempDesc.vLook = vWarPos + vDir * 10.f;
		tempDesc.vLook = XMVectorSetY(tempDesc.vLook, XMVectorGetY(vWarPos) + 20.f);

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
