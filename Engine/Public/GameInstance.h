#pragma once

// System
#include "Graphic_Device.h"
#include "Input_Device.h"

// Utility
#include "Timer_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Level_Manager.h"
#include "PipeLine.h"
#include "Light_Manager.h"
#include "Picking.h"
#include "Target_Manager.h"
#include "Collider_Manager.h"
#include "Frustum.h"


// DirectX Utils
#include "DXString.h"
#include "MathHelper.h"
#include "EasingLerp.h"


BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;
public:
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, CGraphic_Device::GRAPHICDEVDESC GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContext);
	_int Tick_Engine(_float fTimeDelta);
	HRESULT Render_Engine();
	HRESULT PostRender_Engine(unique_ptr<SpriteBatch>&m_spriteBatch, unique_ptr<SpriteFont>&m_spriteFont);

	HRESULT Clear_Level(_uint iLevelIndex);

public: /* For.Graphic_Device */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public: /* For.Timer_Manager*/
	HRESULT Add_Timers(const _tchar* pTimerTag);
	_double Compute_TimeDelta(const _tchar* pTimerTag);

public: /* For.Object_Manager */
	class CGameObject* Get_War(int iLevelIndex = -1);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObjectToLayer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);	
	HRESULT Add_GameObjectToLayer(OUT class CGameObject **pGameObjectClone, _uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void* pArg = nullptr);
	class CComponent* Get_ComponentPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);

	// Used In ImGUI
	list<CGameObject*>* Get_GameObject_CloneList(const _tchar * pLayerTag, int iLevelIndex = -1);
	unordered_map<const _tchar*, class CGameObject*>* Get_GameObject_PrototypeUMap();
	unordered_map<const _tchar*, class CLayer*>* Get_GameObject_LayerUMap();

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);

public: /* For.Input_Device */
	_ubyte Get_DIKeyState(_ubyte eKeyID);
	_long Get_DIMouseMoveState(CInput_Device::MOUSEMOVESTATE eMouseMoveState);
	_byte Get_DIMouseButtonState(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState);

public: /* For.PipeLine */
	_vector Get_CamPosition();
	_vector Get_CamLook();
	HRESULT Bind_Transform_OnShader(CPipeLine::TRANSFORMTYPE eType, CVIBuffer* pVIBuffer, const char* pConstantName);
	HRESULT Bind_Transform_OnShader(CPipeLine::TRANSFORMTYPE eType, CModel * pModel, const char* pConstantName);

public: /* For.Light_Manager */
	LIGHTDESC* Get_LightDesc(_uint iIndex = 0);
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);
	list<class CLight*>* Get_LightList_Addr();


public: /* For.Collider_Manager */
	void Add_Collision(class CGameObject* pGameObject, bool optimize = false, CTransform * pMyTransform = nullptr, const _tchar* pTargetObj = nullptr, _float lengthToAddCollider = 0.f);

public: /* For.Level_Manager */
	int Get_CurrentLevel() // Used in ImGUI
	{
		if (m_pLevel_Manager)
			return m_pLevel_Manager->Get_CurrentLevelIdx();
		return -1;
	}
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNextLevel);

public: /* For.Picking */
	HRESULT Transform_WorldSpaceToLocalSpace(class CTransform* pTransform);

public: /* For.Frustum */
	_bool isIn_WorldSpace(_fvector vWorldPos, _float fRadius = 0.f);

public: /* For.Target_Manager */
	ID3D11ShaderResourceView* Get_RenderTarget_SRV(const _tchar* pTargetTag);

private:
	CGraphic_Device*			m_pGraphic_Device = nullptr;
	CInput_Device*				m_pInput_Device = nullptr;
	CTimer_Manager*				m_pTimer_Manager = nullptr;
	CLevel_Manager*				m_pLevel_Manager = nullptr;
	CObject_Manager*			m_pObject_Manager = nullptr;
	CComponent_Manager*			m_pComponent_Manager  = nullptr;
	CPipeLine*					m_pPipeLine = nullptr;
	CLight_Manager*				m_pLight_Manager = nullptr;
	CTarget_Manager*			m_pTarget_Manager = nullptr;
	CPicking*					m_pPicking = nullptr;
	CCollider_Manager*			m_pCollider_Manager = nullptr;
	CFrustum*					m_pFrustum = nullptr;


public:
	static void Release_Engine();
	virtual void Free() override;
};

END