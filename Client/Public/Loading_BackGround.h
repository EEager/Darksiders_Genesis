#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CLoading_BackGround final : public CGameObject
{
private:
	explicit CLoading_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLoading_BackGround(const CLoading_BackGround& rhs);
	virtual ~CLoading_BackGround() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CTexture*					m_pTextureCom_BackGround = nullptr;
	CTexture*					m_pTextureCom_LoadingCircle = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer_Rect*				m_pVIBufferCom = nullptr;

private:
	_float		m_fBGX, m_fBGY, m_fBGSizeX, m_fBGSizeY;
	_float		m_fCircleX, m_fCircleY, m_fCircleSizeX, m_fCircleSizeY;

	_float4x4	m_matCircle;

	_float4x4	m_ProjMatrix;

private:

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable_BackGround();
	HRESULT SetUp_ConstantTable_LoadingCircle();
	void Rotate_CircleZ(_float fTimeDelta);
	void Set_Circle_State(int state, _fvector vState);


public:	
	static CLoading_BackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END