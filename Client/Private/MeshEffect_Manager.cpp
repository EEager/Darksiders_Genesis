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
	// #3. �״��� ��Į�� �����մϴ�.
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Decal", TEXT("Prototype_GameObject_Decal"), &vDecal)))
		assert(0);

	// #1. ���� �˵��� �����Ѵ�.
	// War ��ų 1���̴�. War �ణ �ؿ��� Į�� ������� ��������
	// ������ 6��. ���̴� 2.
	_float offset = 1.5f;
	for (int i = 0; i < 6; i++)
	{ 
		CHAOSEATERDESC tempDesc; 
		tempDesc.vPos = vWarPos + XMVectorSet(offset * XMScalarCos(XMConvertToRadians(i * 60.f)), -0.75f,	offset * XMScalarSin(XMConvertToRadians(i * 60.f)), 0.f);
			
		// Į�� ���� ������ 
		_vector vDir = XMVector3Normalize(tempDesc.vPos - vWarPos);
		tempDesc.vLook = vWarPos + vDir * 10.f;
		tempDesc.vLook = XMVectorSetY(tempDesc.vLook, XMVectorGetY(vWarPos) + 20.f);

		if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_MeshEffect", TEXT("Prototype_GameObject_MeshEffect_War_ChaosEater"), &tempDesc)))
			assert(0);
	}

	// �ٱ������� 8��. ���̴� 4.
	offset = 3.f;
	for (int i = 0; i < 8; i++)
	{
		CHAOSEATERDESC tempDesc;
		tempDesc.vPos = vWarPos + XMVectorSet(offset * XMScalarCos(XMConvertToRadians(i * 45.f)), 0.f, offset * XMScalarSin(XMConvertToRadians(i * 45.f)), 0.f);

		// Į�� ���� ������ 
		_vector vDir = XMVector3Normalize(tempDesc.vPos - vWarPos);
		tempDesc.vLook = vWarPos + vDir * 10.f;
		tempDesc.vLook = XMVectorSetY(tempDesc.vLook, XMVectorGetY(vWarPos) + 20.f);

		if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_MeshEffect", TEXT("Prototype_GameObject_MeshEffect_War_ChaosEater"), &tempDesc)))
			assert(0);
	}

	// #2. �״��� �������� �����մϴ�.
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_MeshEffect", TEXT("Prototype_GameObject_MeshEffect_Sphere"), &vWarPos)))
		assert(0);



	RELEASE_INSTANCE(CGameInstance);
}

void CMeshEffect_Manager::Free()
{
}
