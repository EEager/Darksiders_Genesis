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
	void BuildVB(ID3D11Device* pDevice); // mMaxParticles �� �����ϸ�ȴ�.
	HRESULT Set_RawValue(const char* pConstantName, void* pData, _uint iSize);
	HRESULT SetUp_Component();
	virtual HRESULT SetUp_ConstantTable(_uint iPassIndex);

protected: // �Ʒ��� �ڽĵ��� ���� ���������̴�. 
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

	UINT mMaxParticles; // ��� ��ƼŬ ������ ���������?
	bool mFirstRun; // ù��° ������ emitter�����Ѵ�.
	float mTimeStep = 0.f; // GS���� �����ð��� ���ϱ� ���� ��.
	float mAge; // �ش� ��ƼŬ �ý��۸Ŵ����� ���� mEmitLoop�� �ƴ϶��, mMaxAge������ ȣ���ϰ� �׾���Ѵ�.
	// ����� �ױ����� m_isAvailable�� ��밡������ �������� �ųʴ� ���� �ʾҰ�����?
	float mMaxAge; // �� ��ƼŬ �ý����� �������� ��� ����������.
	float mGameTime = 0.f; // ������ ����ϱ� ���Ѱ�.

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
			mFirstRun = true; // ó������ emitter�� �ִ´�.
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