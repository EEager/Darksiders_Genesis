#pragma once

/* ���� �Ҵ�� ������ �ּҸ� �����Ѵ�. */
/* ������ ��ü �۾��� �����Ѵ�. */
/* ������ ƽ�Լ��� �ݺ������� ȣ�����ش�. */
#include "Base.h"

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;
public:
	/* Ư�������� �������ش�. */	
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNextLevel);
	_int Tick(_float fTimeDelta);
	HRESULT Render();

public: // Used in ImGUI
	_uint Get_CurrentLevel() { return m_iCurrentLevelIndex; }

private:
	class CLevel*				m_pCurrentLevel = nullptr;
	_uint						m_iCurrentLevelIndex = 0;
public:
	virtual void Free() override;
};

END