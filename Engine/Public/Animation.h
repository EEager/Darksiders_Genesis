#pragma once

#include "Channel.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT NativeConstruct(char* pName, _double Duration, _double TickPerSecond);
	HRESULT Add_Channels(class CChannel* pChannel) { 
		m_Channels.push_back(pChannel);
		Safe_AddRef(pChannel);
		return S_OK;
	}

	HRESULT Update_TransformationMatrix(_float fTimeDelta);

private:
	char			m_szName[MAX_PATH] = "";
	_double			m_Duration = 0.0; /* �ִϸ��̼��� ����ϴµ� �ɸ��� �� �ð�. */
	_double			m_TickPerSecond = 0.0; /* �ʴ� ����ؾ��ϴ� �ִϸ��̼� �ӵ�. */
	_bool			m_isFinished = false;
	_float			m_fTimeAcc = 0.0f;

private:
	/* ���� �ִϸ��̼ǿ� ���⸣ �ִ� ����. */
	vector<class CChannel*>			m_Channels;	
	typedef vector<class CChannel*>	CHANNELS;

public:
	static CAnimation* Create(char* pName, _double Duration, _double TickPerSecond);
	virtual void Free();
};

END