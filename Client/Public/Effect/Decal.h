#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

// CDecal : 플레이어 1번 스킬시 밑에 땅쪽에 생기는 균열 자국. 또는 보스 한손 공격시 아래 자국.
// Dissolve로 서서히 없애자.
class CDecal final : public CGameObject
{
private:
	explicit CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CDecal(const CDecal& rhs);
	virtual ~CDecal() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);

private:	
	ComPtr<CRenderer>			m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CTexture*					m_pDissolveTextureCom = nullptr;
	CVIBuffer_Rect*				m_pModelCom = nullptr;

private:
	_float m_fDissolvePower = 0.001f;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();	

public:	
	static CDecal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

// CDecal2 : 몬스터 소환시 아래에 나오는 것. 첨에 서서히 커지다가 빙글빙글돌면서 시작지나면서, 알파값이 사라진다.
class CDecal2 final : public CGameObject
{
private:
	explicit CDecal2(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CDecal2(const CDecal2& rhs);
	virtual ~CDecal2() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);

private:
	ComPtr<CRenderer> m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_Rect* m_pModelCom = nullptr;

private:
	_float m_fScale = 1.f;
	_float m_TimeAcc = 0.f;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CDecal2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END