#include "..\Public\Channel.h"

CChannel::CChannel()
{

}

HRESULT CChannel::NativeConstruct(const char* pName)
{
	strcpy_s(m_szName, pName);

	XMStoreFloat4x4(&m_LatestTransformationMat, XMMatrixIdentity());

	return S_OK;
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
	for (auto& pKeyFrame : m_KeyFrames)
		Safe_Delete(pKeyFrame);

	m_KeyFrames.clear();
}

