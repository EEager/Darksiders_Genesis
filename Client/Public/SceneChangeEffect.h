#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

// CSceneChangeEffect1
// Logo 레벨에서 다음 레벨로 넘어갈때 사용하는 씬 전환 이펙트이다
class CSceneChangeEffect1 final : public CGameObject
{
private:
	explicit CSceneChangeEffect1(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSceneChangeEffect1(const CSceneChangeEffect1& rhs);
	virtual ~CSceneChangeEffect1() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

private:
	ComPtr<CTexture>					m_pTextureCom = nullptr;
	ComPtr<CRenderer>					m_pRendererCom = nullptr;
	ComPtr<CVIBuffer_Rect>				m_pVIBufferCom = nullptr;

private:
	_float4x4	m_ProjMatrix;
	_float4x4	m_ViewMatrix;

private:
	// 크기는 화면상에 출력될 픽셀들 크기입니다
	const _float fSizeX = 600 * (900.f / 3600.f);
	const _float fSizeY = 1350.f;
	_float	m_fTexturePosX = fSizeX / 2.f; // 초기 위치는 950, -424.f 하면 싹 가려짐
	_float	m_fTexturePosY = fSizeY / 2.f + 950.f; // 초기 위치는 +950, -424.f 하면 싹 가려짐
	const _float MAX_TEXTURE_POS_Y = 300.f;
	
	bool m_bWillDead = false;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(_float positionX, _float positionY);

public:
	static CSceneChangeEffect1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

// CSceneChangeEffect2
// 커튼이 올라갔다가. 내려간다. 아무런 기능을 하지 않는다. 그냥 화면 가리기 용도이다. 
class CSceneChangeEffect2 final : public CGameObject
{
public:
	enum EFFECT2_TYPE { INCREASE, DESCENT };

private:
	explicit CSceneChangeEffect2(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSceneChangeEffect2(const CSceneChangeEffect2& rhs);
	virtual ~CSceneChangeEffect2() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

private:
	ComPtr<CTexture>					m_pTextureCom = nullptr;
	ComPtr<CRenderer>					m_pRendererCom = nullptr;
	ComPtr<CVIBuffer_Rect>				m_pVIBufferCom = nullptr;

private:
	_float4x4	m_ProjMatrix;
	_float4x4	m_ViewMatrix;

public:
	EFFECT2_TYPE Get_Type() { return m_eType; }

private:
	// 크기는 화면상에 출력될 픽셀들 크기입니다
	const _float fSizeX = 600 * (900.f / 3600.f);
	const _float fSizeY = 1350.f;
	_float	m_fTexturePosX = fSizeX / 2.f; // 초기 위치
	_float	m_fTexturePosY = fSizeY / 2.f + 950.f; // 초기 위치는
	const _float MAX_TEXTURE_POS_Y = 300.f; // 화면에 꽉채웠을 때의 Y 위치.
	enum EFFECT2_TYPE m_eType = INCREASE; // 초기엔 상승.
	bool m_bWillDead = false; // 마지막 한번을 출력하고 죽자

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(_float positionX, _float positionY);

public:
	static CSceneChangeEffect2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


// CSceneChangeEffect3
// 영화관 모드로 변경하자. 위아래에 검은색 사각형을 띄울 뿐이다. 
// 누군가가 m_bWillDead를 set해주면, 서서히 사라지면서 죽는다.
class CSceneChangeEffect3 final : public CGameObject
{
private:
	explicit CSceneChangeEffect3(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSceneChangeEffect3(const CSceneChangeEffect3& rhs);
	virtual ~CSceneChangeEffect3() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(_float positionY, _float alpha);

public:
	void Set_Will_Dead(bool b) { m_bWillDead = b; }  
	bool m_bWillDead = false; // 마지막 한번을 출력하고 죽자



private:
	ComPtr<CTexture>					m_pTextureCom = nullptr;
	ComPtr<CRenderer>					m_pRendererCom = nullptr;
	ComPtr<CVIBuffer_Rect>				m_pVIBufferCom = nullptr;

private:
	_float4x4	m_ProjMatrix;
	_float4x4	m_ViewMatrix;

private:
	// 크기는 화면상에 출력될 픽셀들 크기입니다
	// X크기는 화면에 꽉차게
	const _float fSizeX = g_iWinCX;
	// Y크기는 영화관모드 느낌있게
	const _float fSizeY = 85.f;

	_float m_fTexturePosY = 0.f;
	_float m_fTextureAlpha = 0.f;


public:
	static CSceneChangeEffect3* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END