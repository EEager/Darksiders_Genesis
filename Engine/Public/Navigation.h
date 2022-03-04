#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
protected:
	explicit CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pNavigationDataFile);
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	_bool isMove(_fvector vPosition);
	HRESULT Render();

private:
	vector<class CCell*>					m_Cells;
	typedef vector<class CCell*>			CELLS;

private:
	static _float4x4* m_pWorldMatrixPtr;

private:
	_uint			m_iCurrentIndex = 0;

private:
	HRESULT SetUp_Neighbor();


public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pNavigationDataFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END