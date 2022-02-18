#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT NativeConstruct(char* pName, _double Duration, _double TickPerSecond);

private:
	char			m_szName[MAX_PATH] = "";
	_double			m_Duration = 0.0; /* 애니메이션을 재생하는데 걸리는 총 시간. */
	_double			m_TickPerSecond = 0.0; /* 초당 재생해야하는 애니메이션 속도. */

private:
	/* 현재 애니메이션에 영향르 주는 뼈들. */
	vector<class CChannel*>			m_Channels;	
	typedef vector<class CChannel*>	CHANNELS;

public:
	static CAnimation* Create(char* pName, _double Duration, _double TickPerSecond);
	virtual void Free();
};

END