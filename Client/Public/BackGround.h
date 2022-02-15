#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBackGround final : public CGameObject
{
private:
	explicit CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBackGround(const CBackGround& rhs);
	virtual ~CBackGround() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CTexture*					m_pTextureCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer_Rect*				m_pVIBufferCom = nullptr;

private:
	_float		m_fX, m_fY, m_fSizeX, m_fSizeY;	
	_float4x4	m_ProjMatrix;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:	
	static CBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END