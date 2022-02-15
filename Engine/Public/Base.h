#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
public:
	CBase();
	virtual ~CBase() = default;
public:
	/* ���۷���ī��Ʈ�� ������Ų��. */
	unsigned long AddRef();

	/* ���۷���ī��Ʈ�� ���ҽ�Ų��. */
	/* �����Ѵ�.  */
	unsigned long Release();
private:
	unsigned long		m_dwRefCnt = 0;
public:
	virtual void Free() = 0;
};

END