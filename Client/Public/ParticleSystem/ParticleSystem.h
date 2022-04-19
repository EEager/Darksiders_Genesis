#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "Component.h"
#include "ParticleSystem\ParticleSystem_Manager.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CTexture;
END

BEGIN(Client)

// ---------------------------------
// CParticleSystem
// ---------------------------------
class CParticleSystem : public CGameObject
{
	friend class CParticleSystem_Manager;

protected:
	explicit CParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CParticleSystem(const CParticleSystem& rhs);
	virtual ~CParticleSystem() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, int maxParticleNum);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

protected:
	void BuildVB(ID3D11Device* pDevice); // mMaxParticles 만 유의하면된다.
	HRESULT Set_RawValue(const char* pConstantName, void* pData, _uint iSize);
	HRESULT SetUp_Component();
	virtual HRESULT SetUp_ConstantTable(_uint iPassIndex);

protected: // 아래는 자식들이 각자 가져갈것이다. 
	ComPtr<CRenderer>			m_pRendererCom = nullptr;

	// Particle
	ComPtr<CTexture>			m_pTextureParticle = nullptr;
	ComPtr<CTexture>			m_pTextureDecayPuffs = nullptr;
	ComPtr<CTexture>			m_pTextureDust = nullptr;
	ComPtr<CTexture>			m_pTextureFogCloud = nullptr;
	ComPtr<CTexture>			m_pTextureRockChips = nullptr;


	ID3D11ShaderResourceView*	mRandomTexSRV;
	ComPtr<ID3DX11Effect>		m_pEffect = nullptr;
	vector<PASSDESC*>			m_PassesDesc;
	ID3D11InputLayout*			m_ParticleLayout = 0;
	ID3DX11EffectTechnique*		m_StreamOutTech = nullptr;
	ID3DX11EffectTechnique*		m_DrawTech = nullptr;
	ID3D11Buffer*				mInitVB;
	ID3D11Buffer*				mDrawVB;
	ID3D11Buffer*				mStreamOutVB;

	UINT mMaxParticles; // 몇개의 파티클 정점을 만들것인지?
	bool mFirstRun; // 첫번째 정점은 emitter여야한다.
	float mTimeStep = 0.f; // GS에서 생존시간에 더하기 위한 것.
	float mAge; // 해당 파티클 시스템매니져가 만약 mEmitLoop가 아니라면, mMaxAge까지만 호출하고 죽어야한다.
	// 참고로 죽기전에 m_isAvailable를 사용가능으로 돌려놓는 매너는 있지 않았겠지용?
	float mMaxAge; // 이 파티클 시스템은 언제까지 살아 남을것인지.
	float mGameTime = 0.f; // 랜덤값 사용하기 위한것.

	_float3 mEmitPosW;
	_float3 mEmitColor;
	_float3 mEmitInitAccel;
	_float2 mEmitSize;
	_float mEmitRandomPower;
	int	m_iTextureIdx = 0;
	int mTextureTagIdx = 0;

	int streamPassIdx = 0;
	int vertexPassIdx = 0;

protected:
	_bool m_isAvailable = true;

public:	
	static CParticleSystem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, int maxParticleNum);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


// ---------------------------------
// CParticle_Sword
// ---------------------------------
class CParticle_Sword final : public CParticleSystem
{
private:
	explicit CParticle_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CParticle_Sword(const CParticle_Sword& rhs);
	virtual ~CParticle_Sword() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, int maxParticleNum);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);
	virtual HRESULT SetUp_ConstantTable(_uint iPassIndex);	
private:
	class CTransform* m_pTargetTransform = nullptr;
	class CGameObject* m_pTarget = nullptr;
	_bool targetingOnce = false;

public:
	static CParticle_Sword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, int maxParticleNum);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

// ---------------------------------
// CParticle_Blood
// ---------------------------------
class CParticle_Blood final : public CParticleSystem
{
private:
	explicit CParticle_Blood(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CParticle_Blood(const CParticle_Blood& rhs);
	virtual ~CParticle_Blood() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, int maxParticleNum);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);
	virtual HRESULT SetUp_ConstantTable(_uint iPassIndex);

private:
	class CTransform* m_pTargetTransform = nullptr;
	class CGameObject* m_pTarget = nullptr;
	_bool targetingOnce = false;

public:
	static CParticle_Blood* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, int maxParticleNum);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


// ---------------------------------
// CParticle_LightAtk4
// ---------------------------------
class CParticle_LightAtk4 final : public CParticleSystem
{
private:
	explicit CParticle_LightAtk4(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CParticle_LightAtk4(const CParticle_LightAtk4& rhs);
	virtual ~CParticle_LightAtk4() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, int maxParticleNum);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);
	virtual HRESULT SetUp_ConstantTable(_uint iPassIndex);
private:
	class CTransform* m_pTargetTransform = nullptr;
	class CGameObject* m_pTarget = nullptr;
	_bool targetingOnce = false;

public:
	static CParticle_LightAtk4* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, int maxParticleNum);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
}; 


// ---------------------------------
// CParticle_War_Dash_Horse
// ---------------------------------
class CParticle_War_Dash_Horse final : public CParticleSystem
{
private:
	explicit CParticle_War_Dash_Horse(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CParticle_War_Dash_Horse(const CParticle_War_Dash_Horse& rhs);
	virtual ~CParticle_War_Dash_Horse() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, int maxParticleNum);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);
	virtual HRESULT SetUp_ConstantTable(_uint iPassIndex);

	void Toggle_Enable() {
		m_bEnable = !m_bEnable; 
		if (m_bEnable == true)
			mFirstRun = true; // 처음에는 emitter를 넣는다.
	}

private:
	class CTransform* m_pTargetTransform = nullptr;
	class CGameObject* m_pTarget = nullptr;
	_bool targetingOnce = false;
	_bool m_bEnable = false;

public:
	static CParticle_War_Dash_Horse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, int maxParticleNum);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END