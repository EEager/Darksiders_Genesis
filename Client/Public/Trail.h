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
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

private:	
	ComPtr<CRenderer>			m_pRendererCom = nullptr;
	ComPtr<CTexture>			m_pTrailTextureCom = nullptr;
	ComPtr<CVIBuffer_Trail>		m_pTrail = nullptr;

public:
	void Set_Transform(class CTransform* pTransform)
	{
		m_pTargetTransform = pTransform;
		Safe_AddRef(pTransform);
	}
private:
	class CTransform* m_pTargetTransform = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(_uint iPassIndex);	

public:	
	static CTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END