#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Cube;
END

BEGIN(Client)

class CSky final : public CGameObject
{
private:
	explicit CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSky(const CSky& rhs);
	virtual ~CSky() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);

private:
	CTexture*					m_pTextureCom =  nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	CVIBuffer_Cube*				m_pModelCom = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:	
	static CSky* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END