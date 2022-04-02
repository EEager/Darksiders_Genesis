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
	(������������)->(���� ù ������) ���� �������ϳ��� �ϳ��� Ƥ��. �ѹ� loop�� ������ �������� ���� �ƴ϶�. 0���� ��������. 

- m_bOnceAnimated :
	��~~ ó�� ������ ��, matLatestTransform�� ���� ���°��� �ƴϱ⿡, �ִϸ��̼��� �̻��ϴ�. ��ó�� �ִϸ������� 0���� ��������.
	
- m_isBeginFirst :
	State����Ǹ� ä�ε��� ù Ű�����Ӻ��� �����ϵ�������.
*/
HRESULT CAnimation::Update_TransformationMatrix(_float fTimeDelta, _bool isLoop)
{
	// ù �������� ��� ���� m_fTimeAcc �ʱ�ȭ
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

	/* CAnimation�� ��� �������� �������ִ�(m_Channels). 
	   m_Channels���� �� ������ Ű������(�ð������� ���ĵ� �ִϸ����ÿ� ���� ����������)�� ���ϰ��ִ�.
	   ���� ����� �ð�(m_fTimeAcc)�� �°� �ִϸ����ÿ� �� ����� Ű�����Ӱ� ������ �̿��Ͽ� ������. */

	// ex) 135���ǻ��� �� ��ȸ�ϸ鼭 �ش� ������ ������ �ִ� 159���� Ű������ ���� �Ǵ� ������ �Ǵ� ó���� ������ִ� ����� �����. 
	for (int idx = 0; idx < m_Channels.size(); idx++)
	{
		vector<KEYFRAME*>	KeyFrames = *m_Channels[idx]->Get_KeyFrames();

		_matrix matLatestTransform = XMMatrixIdentity();
		if (m_LatestChannels)
			matLatestTransform = ((*m_LatestChannels)[idx])->Get_TransformationMatrix(); // ���� �ִϸ��̼ǰ� ��������

		_uint		iCurrentKeyFrameIndex = m_Channels[idx]->Get_KeyFrameIndex();
		m_iCurrentIdx = iCurrentKeyFrameIndex;

		if (true == m_isFinished || m_isBeginFirst) // �������̰ų�, ù �������� ���, ù��° Ű�����Ӻ��� �����Ѵ�. 
		{
			iCurrentKeyFrameIndex = 0;
			m_Channels[idx]->Set_KeyFrameIndex(iCurrentKeyFrameIndex);
		}

		if (m_fTimeAcc >= KeyFrames.back()->Time) // ������ Ű�������� ���
		{
			// ������ Ű������ ����� ������.
			vScale = XMLoadFloat3(&KeyFrames.back()->vScale); 
			vRotation = XMLoadFloat4(&KeyFrames.back()->vRotation);
			vPosition = XMLoadFloat3(&KeyFrames.back()->vPosition);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		else 
		{
		   /* while : ��ŸŸ�� �ѹ��� �� �����ϸ� iCurrentKeyFrameIndex ��꿡 ������ ����µ� �̸� ��������. 
			* 
			* 1) m_fTimeAcc += m_TickPerSecond * fTimeDelta �� �̷��� ���ǰ�
			* 2) KeyFrames[iCurrentKeyFrameIndex + 1]->Time ���� iCurrentKeyFrameIndex�� ����ϸ�
			* 3) 0���� 2�� ��ŸŸ���� Ƣ�°�� ���� iCurrentKeyFrameIndex�� 2�ε� 1�� ����Ű�Եȴ�.
			*    �̴� fRatio ���� ������ �߻��Ѵ�.
			* 
			* ==================================================
			* KeyIdx: 0       1        2        3       n-1
			*         |-------|--------|--------|  ... --| 
			* ==================================================
			*/
			while (m_fTimeAcc > KeyFrames[iCurrentKeyFrameIndex + 1]->Time) 
				m_Channels[idx]->Set_KeyFrameIndex(++iCurrentKeyFrameIndex);

			// ó�� Ű������ �ε����̸� ���� Ű�������� �ֱ���ġ�� 1��° Ű�������� ���Ͽ� �����Ѵ�. 
 			if (m_LatestChannels && iCurrentKeyFrameIndex == 0 && !m_bOnceFinished && m_bOnceAnimated)
			{
				// Ratio : a ~ b ���� ���� : (x-a)/(b-a) : x�� a�� 0, x��b�� 1
				_float		fRatio = (m_fTimeAcc - KeyFrames[iCurrentKeyFrameIndex]->Time) /
					(KeyFrames[iCurrentKeyFrameIndex + 1]->Time - KeyFrames[iCurrentKeyFrameIndex]->Time);
				
				fRatio = MathHelper::Saturate<_float>(1.5f * fRatio);

				_vector vLatest_Scale;
				_vector vLatest_Rotate;
				_vector vLatest_Translation;
				XMMatrixDecompose(&vLatest_Scale, &vLatest_Rotate, &vLatest_Translation, matLatestTransform);
				
				// ���� Ű�������� �ֱ���ġ�� 1��° ��ġ�� �������Ѵ�. 
				vScale = XMVectorLerp(vLatest_Scale,
					XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex+1]->vScale), fRatio);

				vRotation = XMQuaternionSlerp(vLatest_Rotate,
					XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex+1]->vRotation), fRatio);

				vPosition = XMVectorLerp(vLatest_Translation,
					XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex+1]->vPosition), fRatio);

				vPosition = XMVectorSetW(vPosition, 1.f);
			}
			else // Ű�����ӻ��̿� ������ ���� ��������� ������������ ������
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

		// �����鿡�� ������ TransformationMatrix�� �������.
		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		// ä�ο� �ش� Ű�������� ����� ������ �����صд�. 
		m_Channels[idx]->Set_TransformationMatrix(TransformationMatrix);
	}


	if (m_isBeginFirst) 
		m_isBeginFirst = false;

	 if (!m_bOnceAnimated)
		 m_bOnceAnimated = true;

	// ��ü �ִϸ��̼� duration �ѱ���
	if (m_isFinished)
	{
		if (isLoop) // �ݺ������̸� �ٽ� �ִϸ������ϵ���
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
