#include "..\Public\Transform.h"
#include "VIBuffer.h"
#include "Model.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	_float4		vTmp;

	XMStoreFloat4(&vTmp, vState);

	memcpy(&m_WorldMatrix.m[eState], &vTmp, sizeof(_float4));
}

void CTransform::Set_TransformDesc(const TRANSFORMDESC & TransformDesc)
{
	m_TransformDesc = TransformDesc;
}

_vector CTransform::Get_State_Vec(STATE eState)
{
	return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState]);
}

_float4 CTransform::Get_State_Flt(STATE eState)
{
	return *(_float4*)&m_WorldMatrix.m[eState];
}

HRESULT CTransform::NativeConstruct_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	return S_OK;
}

HRESULT CTransform::Bind_OnShader(CVIBuffer * pVIBuffer, const char * pConstantName)
{
	if (nullptr == pVIBuffer)
		return E_FAIL;

	_matrix			WorldMatrixTransPose = XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix));

	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, WorldMatrixTransPose);

	pVIBuffer->Set_RawValue(pConstantName, &WorldMatrix, sizeof(_float4x4));

	return S_OK;
}

HRESULT CTransform::Bind_OnShader(CModel* pModel, const char* pConstantName)
{
	if (nullptr == pModel)
		return E_FAIL;

	_matrix			WorldMatrixTransPose = XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix));

	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, WorldMatrixTransPose);

	pModel->Set_RawValue(pConstantName, &WorldMatrix, sizeof(_float4x4));

	return S_OK;
}

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (nullptr != pNaviCom)
	{
		if (false == pNaviCom->isMove(vPosition))
			return;
	}

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_matrix		RotationMatrix;

	RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * Get_Scale(CTransform::STATE_RIGHT);
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * Get_Scale(CTransform::STATE_UP);
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * Get_Scale(CTransform::STATE_LOOK);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);
}


void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix		RotationMatrix;

	RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * fTimeDelta);

	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);
}


// 현재 각도에서 fDegreeGoal까지 회전한다
void CTransform::TurnTo_AxisY_Degree(_float fDegreeGoal/*Always Plus*/, _float fTimeDelta /*Always Plus*/)
{
	// angleDegreesYaw 
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	vLook = XMVector3Normalize(vLook);
	_vector vLookXZ = XMVector3Normalize(XMVectorSet(XMVectorGetX(vLook), 0.f, XMVectorGetZ(vLook), 0.f));
	XMVECTOR curVecAngleVec = XMVector3AngleBetweenVectors(vLookXZ, XMVectorSet(0.f, 0.f, 1.f, 0.f)) * (XMVectorGetX(vLook) < 0.f ? -1.f:1.f);
	_float curAngle = XMConvertToDegrees(XMVectorGetX(curVecAngleVec));

	if (XMVectorGetX(vLook) < 0) // 현재 각도 범위를 [0, 360] 으로
		curAngle = 360 + curAngle;

	if (curAngle > 180.f)
		if (fDegreeGoal == 0.f) fDegreeGoal = 360.f;

	_float goalToRotateDegree = fDegreeGoal - curAngle;
	if (goalToRotateDegree > 180.f) // 시계방향보다는 반시계방향이 빠르다.
		goalToRotateDegree -= 360.f;

	if (goalToRotateDegree < -180.f) // 반시계 방향보다는 시계방향이 빠르다. 
		goalToRotateDegree += 360.f;

	if (fabs(goalToRotateDegree) < XMConvertToDegrees(0.1f)) // 더이상 회전할 필요없으면 하지말자
		return; 

	//cout << "curAngle : " << curAngle << endl;
	//cout << "fDegreeGoal : " << fDegreeGoal << endl;
	//cout << "goalToRotateDegree : " << goalToRotateDegree << endl;
	//cout << "fabs(goalToRotateDegree) : " << fabs(goalToRotateDegree) << endl;
	//cout << "m_TransformDesc.fRotationPerSec * fTimeDelta" << m_TransformDesc.fRotationPerSec * fTimeDelta << endl;
	//cout << endl;
	//cout << endl;
	//cout << endl;

	// 시계방향, 반시계방향 set
	bool isClockWise = true;
	if (goalToRotateDegree < 0)
		isClockWise = false;

	_matrix		RotationMatrix;
	RotationMatrix = XMMatrixRotationAxis(XMLoadFloat4(&_float4(0.f, 1.f, 0.f, 0.f)), m_TransformDesc.fRotationPerSec * (isClockWise == false? -fTimeDelta:fTimeDelta) );

	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);
}

void CTransform::LookAt(_fvector vTargetPos)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);

	_vector		vLook = vTargetPos - vPosition;
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);	

	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * Get_Scale(CTransform::STATE_RIGHT));
	Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * Get_Scale(CTransform::STATE_UP));
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * Get_Scale(CTransform::STATE_LOOK));

}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTransform*		pInstance = new CTransform(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CTransform");
		Safe_Release(pInstance);
	}

	return pInstance; 
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*		pInstance = new CTransform(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CTransform");
		Safe_Release(pInstance);
	}

	return pInstance; 
}

void CTransform::Free()
{
	__super::Free();
}
