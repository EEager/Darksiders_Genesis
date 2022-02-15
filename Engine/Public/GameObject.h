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
	/* ������ü�� �ʱ�ȭ�ϱ����� �Լ�. (����,���Ϸκ��� �����͸� �о ���夾����. ) */
	virtual HRESULT NativeConstruct_Prototype();

	/* �纻��ü�� �ʱ�ȭ�ϱ����� �Լ�. (������ü�� �����͸� �޸� �����ϳ�. ) */
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

public:
	/* ������ü�� ȣ���Ѵ�. */
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END