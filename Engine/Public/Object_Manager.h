#pragma once

/* 1. 게임내에서 생성되고 사용ㅇ되는 객체(사본)들을 너희들 기준에 따라 분류하여 저장한다. */
/* 2. 원형객체을 보관한다. */



#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)

private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;
public:
	class CComponent* Get_ComponentPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex);
public:
	HRESULT Reserve_Container(_uint iNumLevels);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObjectToLayer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg);
	HRESULT Add_GameObjectToLayer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg, OUT CGameObject** pGameObject);
	_int Tick(_float fTimeDelta);
	_int LateTick(_float fTimeDelta);
	HRESULT Clear_LevelLayers(_uint iLevelIndex);

public:
	class CGameObject* Get_War(int iLevelIndex = -1); // For.Client

public: // Used in ImGUI
	list<CGameObject*>* Get_GameObject_CloneList(const _tchar* pLayerTag, int iLevelIndex = -1);
	unordered_map<const _tchar*, class CGameObject*>* Get_GameObject_PrototypeUMap();
	unordered_map<const _tchar*, class CLayer*>* Get_GameObject_LayerUMap();


private:
	/* 원형객체들을 저장한다. */
	unordered_map<const _tchar*, class CGameObject*>			m_Prototypes;
	typedef unordered_map<const _tchar*, class CGameObject*>	PROTOTYPES;

private:
	/* 사본객체들을 보관한다. (내 기준에 따라 나누어서. + 레벨별로 구분하여.  )*/
	unordered_map<const _tchar*, class CLayer*>*			m_pLayers = nullptr;
	typedef unordered_map<const _tchar*, class CLayer*>		LAYERS;

private:
	_uint				m_iNumLevels = 0;

private:
	CGameObject* Find_Prototype(const _tchar* pPrototypeTag);
	CLayer*	Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

public:
	HRESULT Save_ObjectsToFile(const _tchar* pFilePath, _uint iLevelIndex);
	HRESULT Load_ObjectsFromFile(const _tchar* pFilePath, _uint iLevelIndex);

public:
	virtual void Free() override;
};

END