#include "..\public\Base.h"

CBase::CBase()
{

}

unsigned long Engine::CBase::AddRef()
{	
	return ++m_dwRefCnt;	
}

/* �����Ѵ�. */
/* �����Ѵ�. */
unsigned long Engine::CBase::Release()
{
	if (0 == m_dwRefCnt)
	{
		Free();

		delete this;

		return 0;
	}
	else
		return m_dwRefCnt--;	// �����ϱ��� refCnt�� ��ȯ�Ѵ�
}
