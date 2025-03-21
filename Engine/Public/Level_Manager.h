#pragma once

/* 현재 할당된 레벨의 주소를 보관한다. */
/* 레벨에 교체 작업도 수행한다. */
/* 레벨의 틱함수를 반복적으로 호출해준다. */
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;
public:
	/* 특정레벨을 셋팅해준다. */	
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNextLevel);
	_int Tick(_float fTimeDelta);
	HRESULT Render();
	HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

public: // Used in ImGUI
	_uint Get_CurrentLevelIdx() { return m_iCurrentLevelIndex; }
	class CLevel* Get_CurrentLevel() { return m_pCurrentLevel; }

private:
	class CLevel*				m_pCurrentLevel = nullptr;
	_uint						m_iCurrentLevelIndex = 0;
public:
	virtual void Free() override;
};

END