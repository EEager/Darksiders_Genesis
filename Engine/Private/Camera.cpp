#include "..\public\Camera.h"
#include "Transform.h"
#include "PipeLine.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCamera::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (nullptr != pArg)

		memcpy(&m_CameraDesc, pArg, sizeof(CCamera::CAMERADESC));

	m_pTransform = CTransform::Create(m_pDevice, m_pDeviceContext);
	if (nullptr == m_pTransform)
		return E_FAIL;

	m_pTransform->Set_TransformDesc(m_CameraDesc.TransformDesc);

	_vector		vEye = XMLoadFloat3(&m_CameraDesc.vEye);
	vEye = XMVectorSetW(vEye, 1.f);
	
	m_pTransform->Set_State(CTransform::STATE_POSITION, vEye);

	_vector		vAt = XMLoadFloat3(&m_CameraDesc.vAt);
	vAt = XMVectorSetW(vAt, 1.f);

	_vector		vLook = vAt - vEye;
	m_pTransform->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));

	_vector		vRight = XMVector3Cross(XMLoadFloat3(&m_CameraDesc.vAxisy), vLook);	
	m_pTransform->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight));

	_vector		vUp = XMVector3Cross(vLook, vRight);	
	m_pTransform->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp));

	return S_OK;
}

_int CCamera::Tick(_float fTimeDelta)
{
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	pPipeLine->Set_Transform(CPipeLine::TS_VIEW, m_pTransform->Get_WorldMatrixInverse());

	_matrix			ProjMatrix = {};
	pPipeLine->Set_Transform(CPipeLine::TS_PROJ, XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));

	RELEASE_INSTANCE(CPipeLine);

	return _int();
}

_int CCamera::LateTick(_float fTimeDelta)
{
	return _int();
}

HRESULT CCamera::Render(_uint iPassIndex)
{
	return S_OK;
}



void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);


}
