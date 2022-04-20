#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

// CDecal : �÷��̾� 1�� ��ų�� �ؿ� ���ʿ� ����� �տ� �ڱ�. �Ǵ� ���� �Ѽ� ���ݽ� �Ʒ� �ڱ�.
// Dissolve�� ������ ������.
class CDecal final : public CGameObject
{
private:
	explicit CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CDecal(const CDecal& rhs);
	virtual ~CDecal() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);

private:	
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CTexture*					m_pDissolveTextureCom = nullptr;
	CVIBuffer_Rect*				m_pModelCom = nullptr;

private:
	_float m_fDissolvePower = 0.001f;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();	

public:	
	static CDecal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END