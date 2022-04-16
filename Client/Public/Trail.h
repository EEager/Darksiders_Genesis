#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTexture;
class CVIBuffer_Trail;
END

BEGIN(Client)

//-------------------------------------------------
// CTrail
//------------------------------------------------- 
class CTrail : public CGameObject
{
public:
	explicit CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTrail(const CTrail& rhs);
	virtual ~CTrail() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

	// �����ϰ� �ִ� ��ü���� �ݵ�� ȣ�����־���ϴ� �޼����
	virtual _int MyTick(_float fTimeDelta, _fmatrix* pBonemMat = nullptr); // Trail�� �����ϰ� �ִ� ��ü���� pBonemMat�� ä���ֵ�������
	void Set_TrailTextureIdx(int idx) { m_TrailTextureIdx = idx; }
	void Set_Transform(class CTransform* pTransform) // ��� native����.
	{
		m_pTargetTransform = pTransform;
		Safe_AddRef(pTransform);
	}
	void Set_Trail_Up_PositionOffset(_float3 vPos)  // ��� native����.
	{
		m_vUpPosOffset = vPos;
	}
	void Set_Trail_Down_PositionOffset(_float3 vPos) // ��� native����.
	{
		m_vDownPosOffset = vPos;
	}

protected:
	class CTransform* m_pTargetTransform = nullptr;
	int m_TrailTextureIdx = 5; // Ʈ���� �ؽ���
	_float3 m_vUpPosOffset = _float3(0.f, 1.f, 0.f); // ���� �󸶸�ŭ �ö󰥰�����
	_float3 m_vDownPosOffset = _float3(0.f, -1.f, 0.f); // ������ �󸶸�ŭ ������������

public:
	static CTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};






//-------------------------------------------------
// CTrail_War_Sword
//-------------------------------------------------
class CTrail_War_Sword final : public CTrail
{
private:
	explicit CTrail_War_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTrail_War_Sword(const CTrail_War_Sword& rhs);
	virtual ~CTrail_War_Sword() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	_int CTrail_War_Sword::MyTick(_float fTimeDelta, _fmatrix* pBonemMat = nullptr); // Trail�� �����ϰ� �ִ� ��ü���� pBonemMat�� ä���ֵ�������
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

private:	
	ComPtr<CRenderer>			m_pRendererCom = nullptr;
	ComPtr<CTexture>			m_pTrailTextureCom = nullptr;
	ComPtr<CTexture>			m_pDistortionNoiseTextureCom = nullptr;
	ComPtr<CVIBuffer_Trail>		m_pTrail = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(_uint iPassIndex);	

public:	
	static CTrail_War_Sword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


//-------------------------------------------------
// CTrail_War_Dash
//-------------------------------------------------
END