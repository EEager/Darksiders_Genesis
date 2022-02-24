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

	/* ä�ε��� Ű�����ӵ��� ������ �����ϱ�. ������ͼ� ���� ����� �����. . */
	/* ����� �ð��� �´� ���� ����� ä�ο� �������ַ���. */
	// 135���ǻ��� �� ��ȸ�ϸ鼭 �ش� ������ ������ �ִ� 159���� Ű������ ���� �Ǵ� ������ �Ǵ� ó���� ������ִ� ����� �����. 
	for (auto& pChannel : m_Channels)
	{
		vector<KEYFRAME*>	KeyFrames = *pChannel->Get_KeyFrames();

		_uint		iCurrentKeyFrameIndex = pChannel->Get_KeyFrameIndex();

		if (true == m_isFinished || m_isBeginFirst)
		{
			iCurrentKeyFrameIndex = 0;
			pChannel->Set_KeyFrameIndex(iCurrentKeyFrameIndex);
		}

		if (m_fTimeAcc >= KeyFrames.back()->Time) /* #1. ������ Ű�������̸�. */
		{
			// ������ Ű������ ����� ��������.
			vScale = XMLoadFloat3(&KeyFrames.back()->vScale); 
			vRotation = XMLoadFloat4(&KeyFrames.back()->vRotation);
			vPosition = XMLoadFloat3(&KeyFrames.back()->vPosition);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		else /* #2. Ű�����ӻ��̿� ������ ���� ��������� ������������ ������. */
		{
		   /* ��ŸŸ��Ƣ�°� ������. 
			* 
			* 1) m_fTimeAcc += m_TickPerSecond * fTimeDelta �� �̷��� ���ǰ�
			* 2) KeyFrames[iCurrentKeyFrameIndex + 1]->Time ���� iCurrentKeyFrameIndex�� ����ϸ�
			* 3) 0���� 2�� ��ŸŸ���� Ƣ�°�� ���� iCurrentKeyFrameIndex�� 2�ε� 1�� ����Ű�Եȴ�.
			*    �̴� fRatio ���� ������ �߻��Ѵ�.
			* 
			* KeyIdx: 0       1        2        3       n-1
			*         |-------|--------|--------|  ... --| 
			*/
			while (m_fTimeAcc > KeyFrames[iCurrentKeyFrameIndex + 1]->Time) 
				pChannel->Set_KeyFrameIndex(++iCurrentKeyFrameIndex);

			/* #3. ó�� Ű�������̸� ���� �ֱ� ��İ��� ���Ͽ� �����Ѵ�. */
			if (iCurrentKeyFrameIndex == 0 && m_isBeginFirst == true)
			{
				_float		fRatio = (m_fTimeAcc - KeyFrames[iCurrentKeyFrameIndex]->Time) /
					(KeyFrames[iCurrentKeyFrameIndex + 1]->Time - KeyFrames[iCurrentKeyFrameIndex]->Time);

				// ����
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
				// 0 ~ 1 ������ ���� ������ ���Ѵ�.
				// p.s ���� if (m_fTimeAcc >= m_Duration)���� ������ Ű�������϶� iCurrentKeyFrameIndex = 0�̶� +1�� ������
				_float		fRatio = (m_fTimeAcc - KeyFrames[iCurrentKeyFrameIndex]->Time) /
					(KeyFrames[iCurrentKeyFrameIndex + 1]->Time - KeyFrames[iCurrentKeyFrameIndex]->Time);

				// ����
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

		// ä�ο� �����صд�. Get_TransformationMatrix(m_iCurrentAnimIndex)�� ���̶�Ű��ȸ�ϸ鼭 Update_CombinedTransformationMatrix���� ���ȴ�.
		pChannel->Set_TransformationMatrix(TransformationMatrix);

		// 

	}

	if (m_isBeginFirst)
	{
		m_isBeginFirst = false;
	}

	// �̰Ŵ� ���� �պ��� ���� 
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
