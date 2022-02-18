#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
public:
	CChannel();
	virtual ~CChannel() = default;

private:
	/* �� ���� Ȱ��ǰ��ִ� Ű�����ӵ� */
	vector<KEYFRAME*>		m_KeyFrames;

public:
	virtual void Free() override;
};

END

