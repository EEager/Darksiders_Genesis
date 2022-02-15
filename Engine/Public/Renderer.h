#pragma once

#include "Component.h"

/* 화면에 그려져야할 객체들을 모아서 보관한다. */
/* 그리는 순서에따라 모아서 관리하자. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDER { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END };
private:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderer() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
public:
	HRESULT Add_RenderGroup(RENDER eRenderGroup, class CGameObject* pGameObject);
	HRESULT Draw(); /*모아놓은객체(화면에 그려질객체)들의 렌더함수를 호출해준다. */
private:
	list<class CGameObject*>			m_RenderObjects[RENDER_END];
	typedef list<class CGameObject*>	RENDEROBJECT;
private:
	HRESULT Render_Priority();
	HRESULT Render_NonAlpha();
	HRESULT Render_Alpha();
	HRESULT Render_UI();
public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END