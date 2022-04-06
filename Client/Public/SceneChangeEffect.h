#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CSceneChangeEffect final : public CGameObject
{
private:
	explicit CSceneChangeEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSceneChangeEffect(const CSceneChangeEffect& rhs);
	virtual ~CSceneChangeEffect() = default;
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

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(_float positionX, _float positionY);

public:
	static CSceneChangeEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END