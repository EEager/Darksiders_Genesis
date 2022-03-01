#pragma once

#include "Component.h"

/* ȭ�鿡 �׷������� ��ü���� ��Ƽ� �����Ѵ�. */
/* �׸��� ���������� ��Ƽ� ��������. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDER { RENDER_PRIORITY, RENDER_NONALPHA_TERRAIN, RENDER_NONALPHA, RENDER_NONALPHA_WAR, RENDER_ALPHA, RENDER_UI, RENDER_MOUSE, RENDER_END };
private:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderer() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
public:
	HRESULT Add_RenderGroup(RENDER eRenderGroup, class CGameObject* pGameObject);
	HRESULT Add_PostRenderGroup(class CGameObject* pGameObject);
	HRESULT Draw(); /*��Ƴ�����ü(ȭ�鿡 �׷�����ü)���� �����Լ��� ȣ�����ش�. */
	HRESULT PostDraw(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);
	HRESULT ClearRenderStates();
private:
	list<class CGameObject*>			m_RenderObjects[RENDER_END];
	typedef list<class CGameObject*>	RENDEROBJECT;

	list<class CGameObject*>			m_PostRenderObjects;
private:
	HRESULT Render_Priority();
	
	HRESULT Render_Priority_Terrain();
	HRESULT Render_NonAlpha();
	HRESULT Render_NonAlpha_War();

	HRESULT Render_Alpha();
	HRESULT Render_UI();
	HRESULT Render_Mouse();

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END