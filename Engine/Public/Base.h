#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
public:
	CBase();
	virtual ~CBase() = default;
public:
	/* 레퍼런스카운트를 증가시킨다. */
	unsigned long AddRef();

	/* 레퍼런스카운트를 감소시킨다. */
	/* 삭제한다.  */
	unsigned long Release();
private:
	unsigned long		m_dwRefCnt = 0;
public:
	virtual void Free() = 0;
};

END