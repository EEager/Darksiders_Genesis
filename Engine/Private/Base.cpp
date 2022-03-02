#include "..\public\Base.h"

CBase::CBase()
{

}

unsigned long Engine::CBase::AddRef()
{	
	return ++m_dwRefCnt;	
}

/* 감소한다. */
/* 삭제한다. */
unsigned long Engine::CBase::Release()
{
	if (0 == m_dwRefCnt)
	{
		Free();

		delete this;

		return 0;
	}
	else
		return m_dwRefCnt--;	// 감소하기전 refCnt를 반환한다
}
