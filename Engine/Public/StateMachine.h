#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CStateMachine final : public CComponent
{
public:
	typedef struct tagStateMachineDesc
	{
		tagStateMachineDesc() {
			ZeroMemory(this, sizeof(this));
			}
		class CGameObject* pOwner;
		class CState* pInitState;
	}STATEMACHINEDESC;

protected:
	explicit CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CStateMachine(const CStateMachine& rhs);
	virtual ~CStateMachine() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	HRESULT Set_GlobalState(class CState* pState);

private:
	class CGameObject* m_pOwner = nullptr;
	class CState* m_pCurrentState = nullptr;
	class CState* m_pPreviousState = nullptr; 
	class CState* m_pGlobalState = nullptr; // 어떤 상태에서도 발생할 수 있는 상태

public:
	HRESULT Tick(_float fDeltaTime);
	HRESULT ChangeState(class CState* pNewState);
	HRESULT RevertToPreviousState();

public:
	static CStateMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END