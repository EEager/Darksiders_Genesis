#include "stdafx.h"
#include "..\public\Camera_Fly.h"

#include "GameInstance.h"

#ifdef _DEBUG
#include "imgui_Manager.h"
#endif


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
	if (!m_bSetTargetOnce)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		Set_Target(static_cast<CWar*>(pGameInstance->Get_War(LEVEL_GAMEPLAY)));
		m_bSetTargetOnce = true;
		RELEASE_INSTANCE(CGameInstance);
	}
	CameraFly_Key(fTimeDelta);
	
	return __super::Tick(fTimeDelta);
}

_int CCamera_Fly::LateTick(_float fTimeDelta)
{
	return _int();
}

HRESULT CCamera_Fly::Render(_uint iPassIndex)
{
	return S_OK;
}

void CCamera_Fly::CameraFly_Key(_float fTimeDelta)
{
	auto pInput_Device = CInput_Device::GetInstance();

	// 디버그 모드일 때는 FreeMode로 동작하자. 키보드, 마우스로 카메라를 움직이자
#define CONST_TIME_DELTA_F 0.016f // 고정 시간값을 사용하는 이유 : Speed Factor 조정시에도 동일한 카메라 속도를 내기위함.
#if defined(USE_IMGUI)
	if (CImguiManager::GetInstance()->GetCursorEnable() == true) // 디버그 모드일때는 움직이지말자
	{
		return;
	}
#endif

	if (CInput_Device::GetInstance()->Key_Down(DIK_V))
	{
		m_eType = (CAMERA_MODE)!(bool)m_eType;
	}

	// 카메라 프리 모드일때는 자유롭게 움직여 다니자.
	if (m_eType == MODE_FREE)
	{
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
	else if (m_eType == MODE_TARGET)// 타겟팅 카메라를 실행한다
	{
		/* 카메라의 움직임을 주면서 카메라 월드행렬을 갱신한다. */
		if (pInput_Device->Key_Pressing(DIK_UP))
		{
			m_fRadius += 4.f * CONST_TIME_DELTA_F;
			m_fHeight = m_fRadius * 0.625f;
			if (m_fHeight >= 13.f)
				m_fHeight = 13.f;
		}

		if (pInput_Device->Key_Pressing(DIK_DOWN))
		{
			m_fRadius -= 4.f * CONST_TIME_DELTA_F;
			m_fHeight = m_fRadius * 0.625f;
			if (m_fHeight >= 13.f)
				m_fHeight = 13.f;
		}

		if (pInput_Device->Key_Pressing(DIK_LEFT))
		{
			m_fRadian -= .8f * CONST_TIME_DELTA_F;
			if (m_fRadian < 0)
				m_fRadian = 2 * XM_PI;
		}

		if (pInput_Device->Key_Pressing(DIK_RIGHT))
		{
			m_fRadian += .8f * CONST_TIME_DELTA_F;
			if (m_fRadian >= 2 * XM_PI)
				m_fRadian = 0.f;
		}

		// 카메라 룩백 Lerp 하게 
		_vector targetPos;
		if (m_bUseBoneMat) // 뼈행렬이 있는 경우 뼈행렬위치를 사용하자
		{
			targetPos = XMVector3TransformCoord(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4x4(&m_matTransformation));
		}
		else // 아닌경우, 타겟 위치 값 사용
		{
			targetPos = static_cast<CTransform*>(m_pTarget->Get_ComponentPtr(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION);
		}
		m_pTransform->LookAt_Lerp(targetPos, m_fLookAtRatio);

		// 위치 러프하게 
		_float posX = m_fRadius * cosf(m_fRadian) + XMVectorGetX(targetPos);
		_float posY = m_fHeight + XMVectorGetY(targetPos);
		_float posZ = m_fRadius * sinf(m_fRadian) + XMVectorGetZ(targetPos);
		m_pTransform->Set_State_Lerp(CTransform::STATE_POSITION, XMVectorSet(posX, posY, posZ, 1.f), m_fPositionRatio);
	}
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

	Safe_Release ( m_pTarget);
}
