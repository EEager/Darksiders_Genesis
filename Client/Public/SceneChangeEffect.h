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
// Logo �������� ���� ������ �Ѿ�� ����ϴ� �� ��ȯ ����Ʈ�̴�
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
	// ũ��� ȭ��� ��µ� �ȼ��� ũ���Դϴ�
	const _float fSizeX = 600 * (900.f / 3600.f);
	const _float fSizeY = 1350.f;
	_float	m_fTexturePosX = fSizeX / 2.f; // �ʱ� ��ġ�� 950, -424.f �ϸ� �� ������
	_float	m_fTexturePosY = fSizeY / 2.f + 950.f; // �ʱ� ��ġ�� +950, -424.f �ϸ� �� ������
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
// Ŀư�� �ö󰬴ٰ�. ��������. �ƹ��� ����� ���� �ʴ´�. �׳� ȭ�� ������ �뵵�̴�. 
class CSceneChangeEffect2 final : public CGameObject
{
private:
	enum TYPE { INCREASE, DESCENT };

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

private:
	// ũ��� ȭ��� ��µ� �ȼ��� ũ���Դϴ�
	const _float fSizeX = 600 * (900.f / 3600.f);
	const _float fSizeY = 1350.f;
	_float	m_fTexturePosX = fSizeX / 2.f; // �ʱ� ��ġ
	_float	m_fTexturePosY = fSizeY / 2.f + 950.f; // �ʱ� ��ġ��
	const _float MAX_TEXTURE_POS_Y = 300.f; // ȭ�鿡 ��ä���� ���� Y ��ġ.
	enum TYPE m_eType = INCREASE; // �ʱ⿣ ���.
	bool m_bWillDead = false; // ������ �ѹ��� ����ϰ� ����

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(_float positionX, _float positionY);

public:
	static CSceneChangeEffect2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END