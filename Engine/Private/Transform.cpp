#include "..\Public\Transform.h"
#include "VIBuffer.h"

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

_vector CTransform::Get_State(STATE eState)
{
	return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState]);
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

void CTransform::Go_Straight(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

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
