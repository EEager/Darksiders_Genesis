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
	void SetUp_CurrentIdx(_fvector fvector);

public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pNavigationDataFile);
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	int isMove(_vector vPosition, OUT _vector* vDstPnt = nullptr);
	_vector Get_Nearest_Point(_float3 vPickingPt);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

public:
	vector<class CCell*>					m_Cells;
	typedef vector<class CCell*>			CELLS;

private:
	static _float4x4* m_pWorldMatrixPtr;

public:
	_uint			m_iCurrentIndex = 0;

public:
	HRESULT SetUp_Neighbor();
	_float Compute_Height(_fvector vPos);


public:
	HRESULT Save_Cells();
	HRESULT Load_Cells();
	void Remove_Cell(int idx);

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pNavigationDataFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END