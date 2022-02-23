#include "..\Public\Animation.h"



CAnimation::CAnimation()
{
}

HRESULT CAnimation::NativeConstruct(char * pName, _double Duration, _double TickPerSecond)
{
	strcpy_s(m_szName, pName);
	m_Duration = Duration;
	m_TickPerSecond = TickPerSecond;

	return S_OK;
}

HRESULT CAnimation::Update_TransformationMatrix(_float fTimeDelta, _bool isLoop)
{
	m_fTimeAcc += m_TickPerSecond * fTimeDelta;

	if (m_fTimeAcc >= m_Duration)
	{
		m_isFinished = true;
		m_fTimeAcc = 0.f;
	}

	_vector		vScale;
	_vector		vRotation;
	_vector		vPosition;

	/* 채널들이 키프레임들을 가지고 있으니까. 가지고와서 상태 행렬을 만들고. . */
	/* 재생된 시간에 맞는 상태 행렬을 채널에 보관해주려고. */
	for (auto& pChannel : m_Channels)
	{
		vector<KEYFRAME*>	KeyFrames = *pChannel->Get_KeyFrames();

		_uint		iCurrentKeyFrameIndex = pChannel->Get_KeyFrameIndex();

		if (true == m_isFinished)
		{
			iCurrentKeyFrameIndex = 0;
			pChannel->Set_KeyFrameIndex(iCurrentKeyFrameIndex);
		}


		if (m_fTimeAcc >= KeyFrames.back()->Time)
		{
			vScale = XMLoadFloat3(&KeyFrames.back()->vScale);
			vRotation = XMLoadFloat4(&KeyFrames.back()->vRotation);
			vPosition = XMLoadFloat3(&KeyFrames.back()->vPosition);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}

		else /* 키프레임사이에 있을때 뼈의 상태행렬을 선형보간으로 만들어낸다. */
		{
			while (m_fTimeAcc > KeyFrames[iCurrentKeyFrameIndex + 1]->Time)
				pChannel->Set_KeyFrameIndex(++iCurrentKeyFrameIndex);

			_float		fRatio = (m_fTimeAcc - KeyFrames[iCurrentKeyFrameIndex]->Time) /
				(KeyFrames[iCurrentKeyFrameIndex + 1]->Time - KeyFrames[iCurrentKeyFrameIndex]->Time);

			vScale = XMVectorLerp(XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex]->vScale),
				XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex + 1]->vScale), fRatio);

			vRotation = XMQuaternionSlerp(XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex]->vRotation),
				XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex + 1]->vRotation), fRatio);

			vPosition = XMVectorLerp(XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex]->vPosition),
				XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex + 1]->vPosition), fRatio);

			vPosition = XMVectorSetW(vPosition, 1.f);
		}

		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		pChannel->Set_TransformationMatrix(TransformationMatrix);

	}

	if (true == m_isFinished)
	{
		if (true == isLoop)
			m_isFinished = false;
	}



	return S_OK;
}

CAnimation * CAnimation::Create(char * pName, _double Duration, _double TickPerSecond)
{
	CAnimation*	pInstance = new CAnimation();

	if (FAILED(pInstance->NativeConstruct(pName, Duration, TickPerSecond)))
	{
		MSG_BOX("Failed To Creating CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimation::Free()
{

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
	
}
