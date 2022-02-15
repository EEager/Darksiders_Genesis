#pragma once

#include "Component.h"

/* ȭ�鿡 �׷������� ��ü���� ��Ƽ� �����Ѵ�. */
/* �׸��� ���������� ��Ƽ� ��������. */

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
	HRESULT Draw(); /*��Ƴ�����ü(ȭ�鿡 �׷�����ü)���� �����Լ��� ȣ�����ش�. */
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