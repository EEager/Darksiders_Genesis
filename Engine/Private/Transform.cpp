#include "..\Public\Transform.h"
#include "VIBuffer.h"
#include "Model.h"
#include "Navigation.h"
#include "PipeLine.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
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

void CTransform::Set_State_Lerp(STATE eState, _fvector vDst, _float fRatio, EasingLerp::EaseType eEaseType)
{
	_float4		vTmp;
	
	_vector srcPos = Get_State(eState);
	_float srcX = XMVectorGetX(srcPos);
	_float srcY = XMVectorGetY(srcPos);
	_float srcZ = XMVectorGetZ(srcPos);

	_float dstX = XMVectorGetX(vDst);
	_float dstY = XMVectorGetY(vDst);
	_float dstZ = XMVectorGetZ(vDst);

	srcX = EasingLerp::Lerp(srcX, dstX, fRatio, eEaseType);
	srcY = EasingLerp::Lerp(srcY, dstY, fRatio, eEaseType);
	srcZ = EasingLerp::Lerp(srcZ, dstZ, fRatio, eEaseType);

	XMStoreFloat4(&vTmp, XMLoadFloat4(&_float4(srcX, srcY, srcZ, 1.f)));
	memcpy(&m_WorldMatrix.m[eState], &vTmp, sizeof(_float4));
}

void CTransform::Set_WorldMatrix(_fmatrix fMat)
{
	XMStoreFloat4x4(&m_WorldMatrix, fMat);
}

void CTransform::Set_TransformDesc(const TRANSFORMDESC & TransformDesc)
{
	m_TransformDesc = TransformDesc;
}

void CTransform::Set_TransformDesc_Speed(const _float& fSpeed)
{
	m_TransformDesc.fSpeedPerSec = fSpeed;
}

CTransform::TRANSFORMDESC* CTransform::Get_TransformDesc_Ptr()
{
	return &m_TransformDesc;
}


_vector CTransform::Get_State(STATE eState)
{
	return Get_State_Vec(eState);
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
		//_vector cornerPos; // 모서리 처리는 TODO로 놔두자
		int ret = pNaviCom->isMove(vPosition);
		if (ret == 0) 
			return;
		//else if (ret == 2) // 모서리 
		//{
		//	//vPosition = cornerPos;
		//}
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

void CTransform::Go_Straight_OnCamera(_float fTimeDelta, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vCamLook = CPipeLine::GetInstance()->Get_CamLook();
	vPosition += XMVector3Normalize(XMVectorSetY(vCamLook, 0.f)) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (nullptr != pNaviCom)
	{
		if (false == pNaviCom->isMove(vPosition))
			return;
	}

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Backward_OnCamera(_float fTimeDelta, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vCamLook = CPipeLine::GetInstance()->Get_CamLook();
	vPosition -= XMVector3Normalize(XMVectorSetY(vCamLook, 0.f)) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (nullptr != pNaviCom)
	{
		if (false == pNaviCom->isMove(vPosition))
			return;
	}

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Left_OnCamera(_float fTimeDelta, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vCameRight = XMVector3Cross(XMLoadFloat4(&_float4(0.f, 1.f, 0.f, 0.f)), CPipeLine::GetInstance()->Get_CamLook());
	vPosition -= XMVector3Normalize(vCameRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (nullptr != pNaviCom)
	{
		if (false == pNaviCom->isMove(vPosition))
			return;
	}

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Right_OnCamera(_float fTimeDelta, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vCameRight = XMVector3Cross(XMLoadFloat4(&_float4(0.f, 1.f, 0.f, 0.f)), CPipeLine::GetInstance()->Get_CamLook());
	vPosition += XMVector3Normalize(vCameRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (nullptr != pNaviCom)
	{
		if (false == pNaviCom->isMove(vPosition))
			return;
	}
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


	const float rotateBlank = 1.5f; // 이정도회전했으면 회전하지 않도록하자
	if (fabs(goalToRotateDegree) < XMConvertToDegrees(m_TransformDesc.fRotationPerSec * fTimeDelta/rotateBlank)) // 더이상 회전할 필요없으면 하지말자
		return; 

#ifdef _DEBUG
	//cout << "curAngle : " << curAngle << endl;
	//cout << "fDegreeGoal : " << fDegreeGoal << endl;
	//cout << "goalToRotateDegree : " << goalToRotateDegree << endl;
	//cout << "fabs(goalToRotateDegree) : " << fabs(goalToRotateDegree) << endl;
	//cout << "m_TransformDesc.fRotationPerSec * fTimeDelta : " << m_TransformDesc.fRotationPerSec * fTimeDelta << endl;
	//cout << endl;
#endif

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

void CTransform::LookAt_Lerp(_fvector vTargetPos, _float fRatio)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLookCur = Get_State(CTransform::STATE_LOOK);

	_vector		vLookTo = XMVectorLerp(vLookCur, vTargetPos - vPosition, fRatio);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLookTo);
	_vector		vUp = XMVector3Cross(vLookTo, vRight);

	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * Get_Scale(CTransform::STATE_RIGHT));
	Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * Get_Scale(CTransform::STATE_UP));
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLookTo) * Get_Scale(CTransform::STATE_LOOK));
}

void CTransform::JumpY(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vToSky = XMLoadFloat4(&m_vJumpDir);

	vPosition += XMVector3Normalize(vToSky) * m_fJumpDy * fTimeDelta;

	m_fJumpDy = max(-9.8f, m_fJumpDy - GRAVITY * fTimeDelta);

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::ClearJumpVar()
{
	m_fJumpDy = INIT_JUMP_DY;
}


// 현재 위치를 vDir 방향으로 fMomentum 만큼 매 프레임 마다 움직이자.
_bool CTransform::Momentum(_fvector vDir, _float fMomentum, _float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);

	vPosition += vDir * fMomentum * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);

	return true;
}

// 현재 위치를 vDir 방향으로 fMomentum 만큼 매 프레임 마다 움직이자. 중력마냥 적용시키자.
#define MOMENTOM_GRAVITY 2.0f
_bool CTransform::MomentumWithGravity(_fvector vDir, _float fMomentum, _float fTimeDelta, _float GroundCheck)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	if (XMVectorGetY(vPosition) <= GroundCheck) // 땅에 닿았다.
		return false;

	vPosition += (vDir * fMomentum * fTimeDelta);
	vPosition += (XMVectorSet(0.f, -1.f, 0.f, 1.f) * fMomentum * 20.f * fTimeDelta);

	Set_State(CTransform::STATE_POSITION, vPosition);

	return true;
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


