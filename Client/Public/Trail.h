#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTexture;
class CVIBuffer_Trail;
END

BEGIN(Client)

class CTrail final : public CGameObject
{
private:
	explicit CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTrail(const CTrail& rhs);
	virtual ~CTrail() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	_int CTrail::MyTick(_float fTimeDelta, _fmatrix* pBonemMat = nullptr); // Trail�� �����ϰ� �ִ� ��ü���� pBonemMat�� ä���ֵ�������
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

private:	
	ComPtr<CRenderer>			m_pRendererCom = nullptr;
	ComPtr<CTexture>			m_pTrailTextureCom = nullptr;
	ComPtr<CVIBuffer_Trail>		m_pTrail = nullptr;

	int m_TrailTextureIdx = 5; // Ʈ���� �ؽ���

public:
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

private:
	class CTransform* m_pTargetTransform = nullptr;
	_float3 m_vUpPosOffset = _float3(0.f, 1.f, 0.f); // ���� �󸶸�ŭ �ö󰥰�����
	_float3 m_vDownPosOffset = _float3(0.f, -1.f, 0.f); // ������ �󸶸�ŭ ������������

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(_uint iPassIndex);	

public:	
	static CTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END