#pragma once

#include "Base.h"

/* 애니메이션 마다 이뼈가 사용된다면 애니메이션 생성된다. */
 
BEGIN(Engine)

class CChannel final : public CBase
{
public:
	CChannel();
	CChannel(const CChannel& rhs);
	virtual ~CChannel() = default;

public:
	const char* Get_Name() const {
		return m_szName;
	}

public:
	const vector<KEYFRAME*>* Get_KeyFrames() const {
		return &m_KeyFrames;
	}

	_uint Get_KeyFrameIndex() const {
		return m_iCurrentKeyFrameIndex;
	}

	_matrix Get_TransformationMatrix() {
		return XMLoadFloat4x4(&m_TransformationMatrix);
	}

	_matrix Get_Latest_TransformationMatrix() {
		return XMLoadFloat4x4(&m_LatestTransformationMat);
	}



public:
	HRESULT NativeConstruct(const char* pName);

public:
	HRESULT Add_KeyFrame(KEYFRAME* pKeyFrame) {
		m_KeyFrames.push_back(pKeyFrame);
		return S_OK;
	}

	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

	void Set_Latest_TransformationMatrix(_fmatrix lastestTransform) {
		XMStoreFloat4x4(&m_LatestTransformationMat, lastestTransform);
	}

	void Set_KeyFrameIndex(_uint iKeyFrameIndex) {
		m_iCurrentKeyFrameIndex = iKeyFrameIndex;
	}

private:
	char					m_szName[MAX_PATH] = "";
	_float4x4				m_TransformationMatrix;
	_float4x4				m_LatestTransformationMat; // 이전 키프레임 행렬저장.
	_uint					m_iCurrentKeyFrameIndex = 0;

	_bool					m_isClone = false;


	/* 특정 애니메이션에서 이뼈가 표현되어야할 키프레임들. */
	vector<KEYFRAME*>			m_KeyFrames;
	typedef vector<KEYFRAME*>	KEYFRAMES;

public:
	static CChannel* Create(const char* pName);
	CChannel* Clone();
	virtual void Free() override;
};

END

