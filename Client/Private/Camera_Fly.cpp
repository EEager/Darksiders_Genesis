#include "stdafx.h"
#include "..\public\Camera_Fly.h"

#include "GameInstance.h"

CCamera_Fly::CCamera_Fly(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{

}

CCamera_Fly::CCamera_Fly(const CCamera & rhs)
	: CCamera(rhs)
{

}

HRESULT CCamera_Fly::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Fly::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CCamera_Fly::Tick(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* 카메라의 움직임을 주면서 카메라 월드행렬을 갱신한다. */
	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_pTransform->Go_Straight(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		m_pTransform->Go_Backward(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_pTransform->Go_Left(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_pTransform->Go_Right(fTimeDelta);
	}

	_long	MouseMove = 0;

 	if (MouseMove = pGameInstance->Get_DIMouseMoveState(CInput_Device::DIMM_X))
	{
		m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.1f);
	}

	if (MouseMove = pGameInstance->Get_DIMouseMoveState(CInput_Device::DIMM_Y))
	{
		m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.1f);
	}

	RELEASE_INSTANCE(CGameInstance);

	return __super::Tick(fTimeDelta);
}

_int CCamera_Fly::LateTick(_float fTimeDelta)
{
	return _int();
}

HRESULT CCamera_Fly::Render()
{
	return S_OK;
}

CCamera_Fly * CCamera_Fly::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CCamera_Fly*		pInstance = new CCamera_Fly(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CCamera_Fly");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Fly::Clone(void * pArg)
{
	CCamera_Fly*		pInstance = new CCamera_Fly(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CCamera_Fly");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CCamera_Fly::Free()
{
	__super::Free();
}
