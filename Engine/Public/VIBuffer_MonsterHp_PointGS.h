#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

// 몬스터 Hp 바는 PointList를 사용하여 PS로만 표현해보자
class ENGINE_DLL CVIBuffer_MonsterHp_PointGS final : public CVIBuffer
{
protected:
	CVIBuffer_MonsterHp_PointGS(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_MonsterHp_PointGS(const CVIBuffer_MonsterHp_PointGS& rhs);
	virtual ~CVIBuffer_MonsterHp_PointGS() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	static CVIBuffer_MonsterHp_PointGS* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END
