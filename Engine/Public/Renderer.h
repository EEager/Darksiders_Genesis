#pragma once

#include "Component.h"

/* 화면에 그려져야할 객체들을 모아서 보관한다. */
/* 그리는 순서에따라 모아서 관리하자. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDER { RENDER_PRIORITY, RENDER_NONALPHA_TERRAIN, RENDER_NONALPHA, RENDER_NONALPHA_WAR, RENDER_NONLIGHT, RENDER_ALPHA, RENDER_UI, RENDER_MOUSE, RENDER_END };
private:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderer() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
public:
	HRESULT Add_RenderGroup(RENDER eRenderGroup, class CGameObject* pGameObject);
	HRESULT Add_PostRenderGroup(class CGameObject* pGameObject);
	HRESULT Draw(); /*모아놓은객체(화면에 그려질객체)들의 렌더함수를 호출해준다. */
	HRESULT PostDraw(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);
	HRESULT ClearRenderStates();
private:
	list<class CGameObject*>			m_RenderObjects[RENDER_END];
	typedef list<class CGameObject*>	RENDEROBJECT;

	list<class CGameObject*>			m_PostRenderObjects;
	typedef list<class CGameObject*>	POSTRENDEROBJECT;

private:
	class CTarget_Manager*				m_pTarget_Manager = nullptr;
	class CLight_Manager*				m_pLight_Manager = nullptr;

	class CVIBuffer_Rect*				m_pVIBuffer = nullptr;
	_float4x4							m_TransformMatrix;
	_float4x4							m_OrthoMatrix;


private:
	HRESULT Render_Priority();

	HRESULT Render_Shadow();
	
	HRESULT Render_NonAlpha();
	HRESULT Render_NonAlpha_War();
	HRESULT Render_NonLight();

	HRESULT Render_Alpha();
	HRESULT Render_UI();

	HRESULT Render_LightAcc();
	HRESULT Render_Blend();

	HRESULT Render_Mouse();

private:
	_float m_fDistortionTimeAcc = 0.f;

public:
	CTexture* m_pTextureNoise = nullptr;

#if 1 // Test Particles
	bool mFirstRun = true;
	float mTimeStep = 0.f;
	float mAge;

	_float3 mEmitPosW;
	_float3 mEmitDirW;

	ID3D11Buffer* mInitVB;
	ID3D11Buffer* mDrawVB;
	ID3D11Buffer* mStreamOutVB;

	ID3DX11Effect*		m_pEffect = nullptr;
	ID3D11InputLayout* m_ParticleLayout = 0;
	ID3DX11EffectTechnique* m_StreamOutTech = nullptr;
	ID3DX11EffectTechnique* m_DrawTech = nullptr;

	HRESULT Set_RawValue(const char* pConstantName, void* pData, _uint iSize);
#endif

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END