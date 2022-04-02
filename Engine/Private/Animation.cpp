#include "..\Public\Animation.h"
#include "MathHelper.h"



CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_Duration(rhs.m_Duration)
	, m_fTimeAcc(rhs.m_fTimeAcc)
	, m_isFinished(rhs.m_isFinished)
	, m_TickPerSecond(rhs.m_TickPerSecond)
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto& pPrototypeChannel : rhs.m_Channels)
	{
		m_Channels.push_back(pPrototypeChannel->Clone());
	}
}

HRESULT CAnimation::NativeConstruct(char * pName, _double Duration, _double TickPerSecond)
{
	strcpy_s(m_szName, pName);
	m_Duration = Duration;
	m_TickPerSecond = TickPerSecond;

	return S_OK;
}

/*
[Boolean Flag Desc]
- m_bOnceFinished : 
	(마지막프레임)->(다음 첫 프레임) 사이 프레임하나가 하나가 튄다. 한번 loop이 끝나면 이전상태 것이 아니라. 0부터 시작하자. 

- m_bOnceAnimated :
	맨~~ 처음 시작할 때, matLatestTransform은 이전 상태것이 아니기에, 애니메이션이 이상하다. 맨처음 애니메이팅은 0부터 시작하자.
	
- m_isBeginFirst :
	State변경되면 채널들의 첫 키프레임부터 시작하도록하자.
*/
HRESULT CAnimation::Update_TransformationMatrix(_float fTimeDelta, _bool isLoop)
{
	// 첫 프레임인 경우 이전 m_fTimeAcc 초기화
	if (m_isBeginFirst == true)
	{
		m_fTimeAcc = 0;
		m_bOnceFinished = false;
		m_isFinished = false;
	}

	m_fTimeAcc += m_TickPerSecond * fTimeDelta;

	if (m_fTimeAcc >= m_Duration)
	{
		m_isFinished = true;
		m_bOnceFinished = true;
		m_fTimeAcc = 0.f;

		if (isLoop == false)
			return S_OK;
	}

	_vector		vScale;
	_vector		vRotation;
	_vector		vPosition;

	/* CAnimation는 모든 뼈정보를 가지고있다(m_Channels). 
	   m_Channels내의 각 뼈들은 키프레임(시간순으로 정렬된 애니메이팅용 뼈의 스자이정보)을 지니고있다.
	   현재 재생된 시간(m_fTimeAcc)에 맞게 애니메이팅용 뼈 행렬을 키프레임간 보간을 이용하여 만들자. */

	// ex) 135개의뼈를 다 순회하면서 해당 뼈들이 가지고 있는 159개의 키프레임 사이 또는 마지막 또는 처음을 만들어주는 행렬을 만든다. 
	for (int idx = 0; idx < m_Channels.size(); idx++)
	{
		vector<KEYFRAME*>	KeyFrames = *m_Channels[idx]->Get_KeyFrames();

		_matrix matLatestTransform = XMMatrixIdentity();
		if (m_LatestChannels)
			matLatestTransform = ((*m_LatestChannels)[idx])->Get_TransformationMatrix(); // 이전 애니메이션과 보간하자

		_uint		iCurrentKeyFrameIndex = m_Channels[idx]->Get_KeyFrameIndex();
		m_iCurrentIdx = iCurrentKeyFrameIndex;

		if (true == m_isFinished || m_isBeginFirst) // 마지막이거나, 첫 프레임인 경우, 첫번째 키프레임부터 시작한다. 
		{
			iCurrentKeyFrameIndex = 0;
			m_Channels[idx]->Set_KeyFrameIndex(iCurrentKeyFrameIndex);
		}

		if (m_fTimeAcc >= KeyFrames.back()->Time) // 마지막 키프레임인 경우
		{
			// 마지막 키프레임 행렬을 만들자.
			vScale = XMLoadFloat3(&KeyFrames.back()->vScale); 
			vRotation = XMLoadFloat4(&KeyFrames.back()->vRotation);
			vPosition = XMLoadFloat3(&KeyFrames.back()->vPosition);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		else 
		{
		   /* while : 델타타임 한번에 팍 증가하면 iCurrentKeyFrameIndex 계산에 오차가 생기는데 이를 방지하자. 
			* 
			* 1) m_fTimeAcc += m_TickPerSecond * fTimeDelta 는 이렇게 계산되고
			* 2) KeyFrames[iCurrentKeyFrameIndex + 1]->Time 으로 iCurrentKeyFrameIndex를 계산하면
			* 3) 0에서 2로 델타타임이 튀는경우 실제 iCurrentKeyFrameIndex는 2인데 1을 가리키게된다.
			*    이는 fRatio 계산기 오류가 발생한다.
			* 
			* ==================================================
			* KeyIdx: 0       1        2        3       n-1
			*         |-------|--------|--------|  ... --| 
			* ==================================================
			*/
			while (m_fTimeAcc > KeyFrames[iCurrentKeyFrameIndex + 1]->Time) 
				m_Channels[idx]->Set_KeyFrameIndex(++iCurrentKeyFrameIndex);

			// 처음 키프레임 인덱스이면 이전 키프레임의 최근위치와 1번째 키프레임을 비교하여 보간한다. 
 			if (m_LatestChannels && iCurrentKeyFrameIndex == 0 && !m_bOnceFinished && m_bOnceAnimated)
			{
				// Ratio : a ~ b 사이 보간 : (x-a)/(b-a) : x가 a면 0, x가b면 1
				_float		fRatio = (m_fTimeAcc - KeyFrames[iCurrentKeyFrameIndex]->Time) /
					(KeyFrames[iCurrentKeyFrameIndex + 1]->Time - KeyFrames[iCurrentKeyFrameIndex]->Time);
				
				fRatio = MathHelper::Saturate<_float>(1.5f * fRatio);

				_vector vLatest_Scale;
				_vector vLatest_Rotate;
				_vector vLatest_Translation;
				XMMatrixDecompose(&vLatest_Scale, &vLatest_Rotate, &vLatest_Translation, matLatestTransform);
				
				// 이전 키프레임의 최근위치와 1번째 위치간 보정을한다. 
				vScale = XMVectorLerp(vLatest_Scale,
					XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex+1]->vScale), fRatio);

				vRotation = XMQuaternionSlerp(vLatest_Rotate,
					XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex+1]->vRotation), fRatio);

				vPosition = XMVectorLerp(vLatest_Translation,
					XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex+1]->vPosition), fRatio);

				vPosition = XMVectorSetW(vPosition, 1.f);
			}
			else // 키프레임사이에 있을때 뼈의 상태행렬을 선형보간으로 만들어낸다
			{
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
		}

		// 정점들에게 적용할 TransformationMatrix를 만들었다.
		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		// 채널에 해당 키프레임의 뼈행렬 정보를 저장해둔다. 
		m_Channels[idx]->Set_TransformationMatrix(TransformationMatrix);
	}


	if (m_isBeginFirst) 
		m_isBeginFirst = false;

	 if (!m_bOnceAnimated)
		 m_bOnceAnimated = true;

	// 전체 애니메이션 duration 넘긴경우
	if (m_isFinished)
	{
		if (isLoop) // 반복동작이면 다시 애니메이팅하도록
			m_isFinished = false;
	}

	return S_OK;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
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
