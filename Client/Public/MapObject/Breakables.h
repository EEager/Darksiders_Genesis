#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CBreakableBase  : public CGameObject
{
protected:
	explicit CBreakableBase(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBreakableBase(const CBreakableBase& rhs);
	virtual ~CBreakableBase() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

public:
	// Collider
	virtual void OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	virtual void OnCollision_Stay(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	virtual void OnCollision_Leave(CCollider* pSrc, CCollider* pDst, float fTimeDelta);

protected:
	ComPtr<CRenderer>					m_pRendererCom = nullptr;
	ComPtr<CTransform>					m_pTransformCom = nullptr;
	ComPtr<CModel>						m_pModelCom = nullptr;

protected:
	_bool m_bHitted = false;
	_float m_fHitPower = 0.f; 

protected:
	HRESULT SetUp_Component(const _tchar* pModelTag);
	HRESULT SetUp_ConstantTable(_uint iPassIndex);	

public:	
	static CBreakableBase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


class CBreakable1 final : public CBreakableBase
{
private:
	explicit CBreakable1(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBreakable1(const CBreakable1& rhs);
	virtual ~CBreakable1() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

public:
	static CBreakable1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

class CBreakable2 final : public CBreakableBase
{
private:
	explicit CBreakable2(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBreakable2(const CBreakable2& rhs);
	virtual ~CBreakable2() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

public:
	static CBreakable2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

class CBreakable3 final : public CBreakableBase
{
private:
	explicit CBreakable3(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBreakable3(const CBreakable3& rhs);
	virtual ~CBreakable3() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

public:
	static CBreakable3* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

class CBreakable4 final : public CBreakableBase
{
private:
	explicit CBreakable4(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBreakable4(const CBreakable4& rhs);
	virtual ~CBreakable4() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

public:
	static CBreakable4* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

class CBreakable5 final : public CBreakableBase
{
private:
	explicit CBreakable5(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBreakable5(const CBreakable5& rhs);
	virtual ~CBreakable5() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

public:
	static CBreakable5* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END