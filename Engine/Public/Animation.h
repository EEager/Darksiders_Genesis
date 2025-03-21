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

	// iChannelIdx 의 현재 키프레임 인덱스를 m_iCurrentIdx에 저장하고 싶을때 싶을때 사용한다.
	// -1이면 그냥 마지막꺼 대충 넣고, 아니면 해당 뼈 idx의 키프레임 값을 반환받자
	HRESULT Update_TransformationMatrix(_float fTimeDelta, _bool isLoop, int iChannelIdx = -1);

public:
	// 애니메이션은 시작 키프레임부터 시작하자
	void SetBeginFirst(_bool b = true){
		m_isBeginFirst = b; 
		m_fTimeAcc = 0;
		m_bOnceFinished = false;
		m_isFinished = false;
	}
	// 해당 애니메이션이 끝나었느냐? 
	_bool Get_isFinished() { return m_isFinished;
	}

private:
	/* 현재 애니메이션에 영향을 주는 뼈들. */
	vector<class CChannel*>			m_Channels;
	typedef vector<class CChannel*>	CHANNELS;

	/* 애니메이션에 영향을 주는 가장 최근 뼈들. 
	   모델 SetUp_Animation에서 넣어주자. */
	vector<class CChannel*>* m_LatestChannels = nullptr;
	typedef vector<class CChannel*>	LATEST_CHANNELS;

private:
	char			m_szName[MAX_PATH] = "";
	_double			m_Duration = 0.0; /* 애니메이션을 재생하는데 걸리는 총 시간. */
	_double			m_TickPerSecond = 0.0; /* 초당 재생해야하는 애니메이션 속도. */
	_bool			m_isFinished = false;
	_float			m_fTimeAcc = 0.0f;
	_bool			m_bOnceFinished = false;
	_bool			m_bOnceAnimated = false; // 애니메이션 가장~ 처음 것은 이전과 보간하지말자


public:
	_uint GetCurIdx() { return m_iCurrentIdx; }

private:
	_uint			m_iCurrentIdx = 0; // 현재 뼈들이 몇번째 키프레임 인덱스를 출력중인지.


private:
	_bool m_isBeginFirst = true; // 애니메이션 인덱스바뀌면 키프레임 처음부터 출력하자.

public:
	static CAnimation* Create(char* pName, _double Duration, _double TickPerSecond);
	CAnimation* Clone();
	virtual void Free();
};

END