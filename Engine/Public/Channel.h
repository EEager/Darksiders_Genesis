#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
public:
	CChannel();
	virtual ~CChannel() = default;

private:
	/* 이 뼈가 활용되고있는 키프레임들 */
	vector<KEYFRAME*>		m_KeyFrames;

public:
	virtual void Free() override;
};

END

