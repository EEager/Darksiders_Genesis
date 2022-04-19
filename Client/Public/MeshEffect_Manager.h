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
	// �ʱ�ȭ�� Loader���� ��������. �׳� �ݹ��Լ� ��ϸ� �����ִ� ������ �Ѵ�.
	void Initialize(); 

	
	// ��ϵ� �ݹ��Լ��� ȣ���ϴ� ����̴�. �÷��̾ �̰��� ȣ���Ұ��̴�.
	// �ʿ��� ��� ����Ʈ�� ������ ��ġ�� ��������
	void Add_MeshEffects_To_Layer(const _tchar* pMeshEffectTag, _float fTimeDelta, _float3* vPos = nullptr);

private:
	// Tag�� �����Ǵ� ����Ʈ �ݹ��Լ����̴�.
	unordered_map<const _tchar*, function<void(_float deltaTime)>>  m_EffectCallBack;

	void Effect_War_Skill_1(_float fTimeDelta);
	class CTransform* m_pWarTransform = nullptr;
public:
	virtual void Free();
};
END