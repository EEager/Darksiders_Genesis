#include "..\Public\Channel.h"

CChannel::CChannel()
	: m_isClone(false)
{

}

CChannel::CChannel(const CChannel& rhs)
	: m_TransformationMatrix(rhs.m_TransformationMatrix)
	, m_iCurrentKeyFrameIndex(rhs.m_iCurrentKeyFrameIndex)
	, m_KeyFrames(rhs.m_KeyFrames)
	, m_isClone(true)
{
	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CChannel::NativeConstruct(const char* pName)
{
	strcpy_s(m_szName, pName);

	XMStoreFloat4x4(&m_LatestTransformationMat, XMMatrixIdentity());

	return S_OK;
}

CChannel* CChannel::Clone()
{
	return new CChannel(*this);
}


CChannel* CChannel::Create(const char* pName)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->NativeConstruct(pName)))
	{
		MSG_BOX("Failed To Creating CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CChannel::Free()
{
	if (false == m_isClone)
	{
		for (auto& pKeyFrame : m_KeyFrames)
			Safe_Delete(pKeyFrame);
		m_KeyFrames.clear();
	}
}

