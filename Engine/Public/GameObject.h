#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;
public:
	class CComponent* Get_ComponentPtr(const _tchar* pComponentTag);
	_float Get_CamDistance() const {
		return m_fCamDistance;
	}
public:
	/* 원형객체를 초기화하기위한 함수. (서버,파일로부터 데이터를 읽어서 저장ㅎ나다. ) */
	virtual HRESULT NativeConstruct_Prototype();

	/* 사본객체를 초기화하기위한 함수. (원형객체의 데이터를 메모리 복사하낟. ) */
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);
protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;

	unordered_map<const _tchar*, class CComponent*>				m_Components;
	typedef unordered_map<const _tchar*, class CComponent*>		COMPONENTS;

protected:
	_float					m_fCamDistance = 0.f;

private:
	CComponent* Find_Component(const _tchar* pComponentTag);

protected:
	MTRLDESC m_tMtrlDesc;


	// Used In ImGUI
public:
	const _tchar* Get_LayerTag() { return m_pLayerTag; }
	void Set_LayerTag(const _tchar* pPrototypeTag) { m_pLayerTag = pPrototypeTag; }

	const _tchar* Get_PrototypeTag() { return m_pPrototypeTag; }
	void Set_PrototypeTag(const _tchar* pPrototypeTag) { m_pPrototypeTag = pPrototypeTag; }

protected:
	const _tchar* m_pPrototypeTag = nullptr;
	const _tchar* m_pLayerTag = nullptr;

public:
	/* 원형객체가 호출한다. */
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END