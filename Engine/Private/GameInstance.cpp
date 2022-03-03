#include "..\Public\GameInstance.h"
#include "Timer_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::GetInstance())
	, m_pInput_Device(CInput_Device::GetInstance())
	, m_pTimer_Manager(CTimer_Manager::GetInstance())
	, m_pObject_Manager(CObject_Manager::GetInstance())
	, m_pComponent_Manager(CComponent_Manager::GetInstance())
	, m_pLevel_Manager(CLevel_Manager::GetInstance())
	, m_pPipeLine(CPipeLine::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())

{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pGraphic_Device);

}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, CGraphic_Device::GRAPHICDEVDESC GraphicDesc, ID3D11Device ** ppDevice, ID3D11DeviceContext ** ppDeviceContext)
{
	if (nullptr == m_pComponent_Manager || 
		nullptr == m_pObject_Manager)
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iWinCX, GraphicDesc.iWinCY, ppDevice, ppDeviceContext)))
		return E_FAIL;

	if (FAILED(m_pInput_Device->Ready(hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	

	return S_OK;
}

_int CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pObject_Manager || 
		nullptr == m_pInput_Device)
		return -1;

	_int		iProgress = 0;

	if (FAILED(m_pInput_Device->Update()))
		return -1;

	if (0 > (iProgress = m_pObject_Manager->Tick(fTimeDelta)))
		return -1;

	m_pPipeLine->Tick();

	if (0 > (iProgress = m_pObject_Manager->LateTick(fTimeDelta)))
		return -1;

	if (0 > (iProgress = m_pLevel_Manager->Tick(fTimeDelta)))
		return -1;

	return _int();
}

HRESULT CGameInstance::Render_Engine()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

#ifdef _DEBUG
	/* Dummy의 기능. (디버깅적 요소의 출력.) */
	m_pLevel_Manager->Render();
#endif

	return S_OK;

}

HRESULT CGameInstance::PostRender_Engine(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

#ifdef _DEBUG
	/* Dummy의 기능. (디버깅적 요소의 출력.) */
	m_pLevel_Manager->PostRender(m_spriteBatch, m_spriteFont);
#endif

	return S_OK;

}

HRESULT CGameInstance::Clear_Level(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pObject_Manager->Clear_LevelLayers(iLevelIndex);
	m_pComponent_Manager->Clear(iLevelIndex);


	return S_OK;
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Add_Timers(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timers(pTimerTag);
}

_double CGameInstance::Compute_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0;

	return m_pTimer_Manager->Compute_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_GameObjectToLayer(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObjectToLayer(iLevelIndex, pLayerTag, pPrototypeTag, pArg);
}

CComponent * CGameInstance::Get_ComponentPtr(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_ComponentPtr(iLevelIndex, pLayerTag, pComponentTag, iIndex);
}

list<CGameObject*>* CGameInstance::Get_GameObject_CloneList(const _tchar* pLayerTag, int iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_GameObject_CloneList(pLayerTag, iLevelIndex);
}

unordered_map<const _tchar*, class CGameObject*>* CGameInstance::Get_GameObject_PrototypeUMap()
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_GameObject_PrototypeUMap();
}

unordered_map<const _tchar*, class CLayer*>* CGameInstance::Get_GameObject_LayerUMap()
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_GameObject_LayerUMap();
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

_ubyte CGameInstance::Get_DIKeyState(_ubyte eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(eKeyID);
}

_long CGameInstance::Get_DIMouseMoveState(CInput_Device::MOUSEMOVESTATE eMouseMoveState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMoveState(eMouseMoveState);
}

_byte CGameInstance::Get_DIMouseButtonState(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseButtonState(eMouseButtonState);
}

_vector CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return XMVectorZero();

	return m_pPipeLine->Get_CamPosition();
}

_vector CGameInstance::Get_CamLook()
{
	if (nullptr == m_pPipeLine)
		return XMVectorZero();

	return m_pPipeLine->Get_CamLook();
}

HRESULT CGameInstance::Bind_Transform_OnShader(CPipeLine::TRANSFORMTYPE eType, CVIBuffer * pVIBuffer, const char * pConstantName)
{
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	_matrix	TransformMatrix = m_pPipeLine->Get_Transform(eType);
	TransformMatrix = XMMatrixTranspose(TransformMatrix);

	_float4x4	TransformFloat4x4;
	XMStoreFloat4x4(&TransformFloat4x4, TransformMatrix);

	pVIBuffer->Set_RawValue(pConstantName, &TransformFloat4x4, sizeof(_float4x4));

	return S_OK;
}

HRESULT CGameInstance::Bind_Transform_OnShader(CPipeLine::TRANSFORMTYPE eType, CModel* pModel, const char* pConstantName)
{
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	_matrix	TransformMatrix = m_pPipeLine->Get_Transform(eType);
	TransformMatrix = XMMatrixTranspose(TransformMatrix);

	_float4x4	TransformFloat4x4;
	XMStoreFloat4x4(&TransformFloat4x4, TransformMatrix);

	pModel->Set_RawValue(pConstantName, &TransformFloat4x4, sizeof(_float4x4));

	return S_OK;
}

LIGHTDESC * CGameInstance::Get_LightDesc(_uint iIndex) 
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);	
}

HRESULT CGameInstance::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(pDevice, pDeviceContext, LightDesc);
}

list<class CLight*>* CGameInstance::Get_LightList_Addr()
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightList_Addr();
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pNextLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNextLevel);
}
void CGameInstance::Release_Engine()
{
	if (0 != CGameInstance::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Release CGameInstance ");

	if (0 != CObject_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Release CObject_Manager");

	if (0 != CComponent_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Release CComponent_Manager");

	if (0 != CPipeLine::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Release CPipeLine");

	if (0 != CLevel_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Release CLevel_Manager ");

	if (0 != CTimer_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Release CTimer_Manager ");

	if (0 != CLight_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Release CLight_Manager ");

	if (0 != CInput_Device::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Release CInput_Device ");

	if (0 != CGraphic_Device::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Release CGraphic_Device ");
	

}

void CGameInstance::Free()
{
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pGraphic_Device);
	
}

