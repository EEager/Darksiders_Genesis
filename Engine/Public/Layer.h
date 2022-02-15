#pragma once

#include "Base.h"

/* 사본객체들을 모아놓는다. */

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;
public:
	class CComponent* Get_ComponentPtr(const _tchar* pComponentTag, _uint iIndex);

public: // Used in ImGUI
	list<class CGameObject*>* Get_List_Adr() { return &m_Objects; }

public:
	HRESULT NativeConstruct();
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	_int Tick(_float fTimeDelta);
	_int LateTick(_float fTimeDelta);
private:
	list<class CGameObject*>			m_Objects;
	typedef list<class CGameObject*>	OBJECTS;
public:
	static CLayer* Create();
	virtual void Free() override;
};

END