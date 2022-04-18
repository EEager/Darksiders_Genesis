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
	HRESULT SetUp_ConstantTable(_uint iPassIndex);

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

	UINT mMaxParticles;
	bool mFirstRun;
	float mTimeStep = 0.f;
	float mAge; // �ش� ��ƼŬ �ý��۸Ŵ����� ���� mEmitLoop�� �ƴ϶��, mMaxAge������ ȣ���ϰ� �׾���Ѵ�.
	// ����� �ױ����� m_isAvailable�� ��밡������ �������� �ųʴ� ���� �ʾҰ�����?
	float mMaxAge;
	float mGameTime = 0.f;
	bool mEmitLoop = false;

	_float3 mEmitPosW;
	_float3 mEmitColor;
	_float3 mEmitInitAccel;
	_float2 mEmitSize;
	_float mEmitRandomPower;
	int	m_iTextureIdx = 0;
	int mTextureTagIdx = 0;

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

public:
	static CParticle_Sword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, int maxParticleNum);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END