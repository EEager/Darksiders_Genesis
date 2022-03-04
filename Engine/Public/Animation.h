#pragma once

#include "Channel.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	const char* Get_Name() const {
		return m_szName;
	}

	vector<class CChannel*>* Get_Channels() {
		return &m_Channels;
	}

	void Set_Latest_Channels(vector<class CChannel*>* pLatestChannels) {
		//m_LatestChannels.assign(pLatestChannels->begin(), pLatestChannels->end());
		m_LatestChannels = pLatestChannels;
	}

public:
	HRESULT NativeConstruct(char* pName, _double Duration, _double TickPerSecond);
	HRESULT Add_Channels(class CChannel* pChannel) { 
		m_Channels.push_back(pChannel);
		return S_OK;
	}

	HRESULT Update_TransformationMatrix(_float fTimeDelta, _bool isLoop);

public:
	// 애니메이션은 시작 키프레임부터 시작하자
	void SetBeginFirst(_bool b = true) { m_isBeginFirst = b; }
	// 해당 애니메이션이 끝나었느냐? 
	_bool Get_isFinished() { return m_isFinished; }

private:
	/* 현재 애니메이션에 영향을 주는 뼈들. */
	vector<class CChannel*>			m_Channels;
	typedef vector<class CChannel*>	CHANNELS;

	/* 애니메이션에 영향을 주는 가장 최근 뼈들. 
	   모델 SetUp_Animation에서 넣어주자. */
	vector<class CChannel*>*			m_LatestChannels;
	typedef vector<class CChannel*>	LATEST_CHANNELS;

private:
	char			m_szName[MAX_PATH] = "";
	_double			m_Duration = 0.0; /* 애니메이션을 재생하는데 걸리는 총 시간. */
	_double			m_TickPerSecond = 0.0; /* 초당 재생해야하는 애니메이션 속도. */
	_bool			m_isFinished = false;
	_float			m_fTimeAcc = 0.0f;
	_bool			m_bOnceFinished = false;
	_bool			m_bOnceStart = false;


private:
	_bool m_isBeginFirst = true; // 애니메이션 인덱스바뀌면 키프레임 처음부터 출력하자.

public:
	static CAnimation* Create(char* pName, _double Duration, _double TickPerSecond);
	CAnimation* Clone();
	virtual void Free();
};

END