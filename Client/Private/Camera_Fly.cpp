#include "stdafx.h"
#include "..\public\Camera_Fly.h"

#include "GameInstance.h"

#include "imgui_Manager.h"


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

	auto pInput_Device = CInput_Device::GetInstance();
#if defined(USE_IMGUI)
	if (CImguiManager::GetInstance()->GetCursorEnable() == false)
	{
		/* 카메라의 움직임을 주면서 카메라 월드행렬을 갱신한다. */
#define CONST_TIME_DELTA_F 0.016f
		if (pInput_Device->Key_Pressing(DIK_UP))
		{
			m_pTransform->Go_Straight(CONST_TIME_DELTA_F);
		}

		if (pInput_Device->Key_Pressing(DIK_DOWN))
		{
			m_pTransform->Go_Backward(CONST_TIME_DELTA_F);
		}
		
		if (pInput_Device->Key_Pressing(DIK_LEFT))
		{
			m_pTransform->Go_Left(CONST_TIME_DELTA_F);
		}
		
		if (pInput_Device->Key_Pressing(DIK_RIGHT))
		{
			m_pTransform->Go_Right(CONST_TIME_DELTA_F);
		}

		_long	MouseMove = 0;

		if (MouseMove = pInput_Device->Get_DIMouseMoveState(CInput_Device::DIMM_X))
		{
			m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), CONST_TIME_DELTA_F * MouseMove * 0.1f);
		}

		if (MouseMove = pInput_Device->Get_DIMouseMoveState(CInput_Device::DIMM_Y))
		{
			m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), CONST_TIME_DELTA_F * MouseMove * 0.1f);
		}
	}
#endif

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
