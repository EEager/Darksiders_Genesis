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
	if (m_isBeginFirst == true)
	{
		m_fTimeAcc = 0;
	}

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
	// 135개의뼈를 다 순회하면서 해당 뼈들이 가지고 있는 159개의 키프레임 사이 또는 마지막 또는 처음을 만들어주는 행렬을 만든다. 
	for (auto& pChannel : m_Channels)
	{
		vector<KEYFRAME*>	KeyFrames = *pChannel->Get_KeyFrames();

		_uint		iCurrentKeyFrameIndex = pChannel->Get_KeyFrameIndex();

		if (true == m_isFinished || m_isBeginFirst)
		{
			iCurrentKeyFrameIndex = 0;
			pChannel->Set_KeyFrameIndex(iCurrentKeyFrameIndex);
		}

		if (m_fTimeAcc >= KeyFrames.back()->Time) /* #1. 마지막 키프레임이면. */
		{
			// 마지막 키프레임 행렬을 가져오자.
			vScale = XMLoadFloat3(&KeyFrames.back()->vScale); 
			vRotation = XMLoadFloat4(&KeyFrames.back()->vRotation);
			vPosition = XMLoadFloat3(&KeyFrames.back()->vPosition);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		else /* #2. 키프레임사이에 있을때 뼈의 상태행렬을 선형보간으로 만들어낸다. */
		{
		   /* 델타타임튀는거 방지용. 
			* 
			* 1) m_fTimeAcc += m_TickPerSecond * fTimeDelta 는 이렇게 계산되고
			* 2) KeyFrames[iCurrentKeyFrameIndex + 1]->Time 으로 iCurrentKeyFrameIndex를 계산하면
			* 3) 0에서 2로 델타타임이 튀는경우 실제 iCurrentKeyFrameIndex는 2인데 1을 가리키게된다.
			*    이는 fRatio 계산기 오류가 발생한다.
			* 
			* KeyIdx: 0       1        2        3       n-1
			*         |-------|--------|--------|  ... --| 
			*/
			while (m_fTimeAcc > KeyFrames[iCurrentKeyFrameIndex + 1]->Time) 
				pChannel->Set_KeyFrameIndex(++iCurrentKeyFrameIndex);

			/* #3. 처음 키프레임이면 가장 최근 행렬값과 비교하여 보간한다. */
			if (iCurrentKeyFrameIndex == 0 && m_isBeginFirst == true)
			{
				_float		fRatio = (m_fTimeAcc - KeyFrames[iCurrentKeyFrameIndex]->Time) /
					(KeyFrames[iCurrentKeyFrameIndex + 1]->Time - KeyFrames[iCurrentKeyFrameIndex]->Time);

				// 보간
				vScale = XMVectorLerp(XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex]->vScale),
					XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex + 1]->vScale), fRatio);

				vRotation = XMQuaternionSlerp(XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex]->vRotation),
					XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex + 1]->vRotation), fRatio);

				vPosition = XMVectorLerp(XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex]->vPosition),
					XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex + 1]->vPosition), fRatio);

				vPosition = XMVectorSetW(vPosition, 1.f);

				bool XM_CALLCONV  noexcept XMMatrixDecompose(
					[in, out] XMVECTOR * outScale,
					[in, out] XMVECTOR * outRotQuat,
					[in, out] XMVECTOR * outTrans,
					[in]      FXMMATRIX M
				);
			}
			else
			{
				// 0 ~ 1 사이의 보간 비율을 구한다.
				// p.s 위에 if (m_fTimeAcc >= m_Duration)에서 마지막 키프레임일때 iCurrentKeyFrameIndex = 0이라서 +1은 ㄱㅊ다
				_float		fRatio = (m_fTimeAcc - KeyFrames[iCurrentKeyFrameIndex]->Time) /
					(KeyFrames[iCurrentKeyFrameIndex + 1]->Time - KeyFrames[iCurrentKeyFrameIndex]->Time);

				// 보간
				vScale = XMVectorLerp(XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex]->vScale),
					XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex + 1]->vScale), fRatio);

				vRotation = XMQuaternionSlerp(XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex]->vRotation),
					XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex + 1]->vRotation), fRatio);

				vPosition = XMVectorLerp(XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex]->vPosition),
					XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex + 1]->vPosition), fRatio);

				vPosition = XMVectorSetW(vPosition, 1.f);
			}
		}

		// 정점들에게 적용할 TransformationMatrix를 만들었다.
		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		// 채널에 저장해둔다. Get_TransformationMatrix(m_iCurrentAnimIndex)은 하이라키순회하면서 Update_CombinedTransformationMatrix에서 사용된다.
		pChannel->Set_TransformationMatrix(TransformationMatrix);

		// 

	}

	if (m_isBeginFirst)
	{
		m_isBeginFirst = false;
	}

	// 이거는 조금 손보자 ㅎㅎ 
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
