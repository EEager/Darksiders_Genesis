#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec = 0.f;
		_float		fRotationPerSec = 0.f;
	}TRANSFORMDESC;
private:
	explicit CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	void Set_State(STATE eState, _fvector vState);
	void Set_WorldMatrix(_fmatrix fMat);
	void Set_State_Lerp(STATE eState, _fvector vState, _float fRatio);
	void Set_TransformDesc(const TRANSFORMDESC& TransformDesc);

	_vector Get_State(STATE eState);
	_vector Get_State_Vec(STATE eState);
	_float4 Get_State_Flt(STATE eState);
	_float Get_Scale(STATE eState) {
		return XMVectorGetX(XMVector3Length(Get_State(eState)));
	}
	_matrix Get_WorldMatrixInverse() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	_float4x4* Get_WorldFloat4x4Ptr() {
		return &m_WorldMatrix;
	}

	_matrix Get_WorldMatrix() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	_float4x4* Get_WorldMatrix_4x4() {
		return &m_WorldMatrix;
	}

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	HRESULT Bind_OnShader(class CVIBuffer* pVIBuffer, const char* pConstantName);
	HRESULT Bind_OnShader(class CModel* pModel, const char* pConstantName);
	

public:
	void Go_Straight(_float fTimeDelta, class CNavigation* pNaviCom = nullptr);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Rotation(_fvector vAxis, _float fRadian);
	void Turn(_fvector vAxis, _float fTimeDelta);
	void TurnTo_AxisY_Degree(_float fDegreeGoal, _float fTimeDelta);
	void LookAt_Lerp(_fvector vTargetPos, _float fRatio);
	void LookAt(_fvector vTargetPos);

private:
	_float4x4		m_WorldMatrix;
	TRANSFORMDESC	m_TransformDesc;


	// Momentum : °¡¼Óµµ ¤¾ 
public:
	void Gravity(_float fTimeDelta);

private:
	_float4			m_MomentumMatrix;
	_float			m_MomentumPower = 0.f;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
