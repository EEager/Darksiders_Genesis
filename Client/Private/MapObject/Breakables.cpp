#include "stdafx.h"
#include "..\public\MapObject\Breakables.h"

#include "GameInstance.h"
#include "Transform.h"

#ifdef USE_IMGUI
#include "imgui_Manager.h"
#endif

CBreakableBase::CBreakableBase(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBreakableBase::CBreakableBase(const CBreakableBase& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBreakableBase::NativeConstruct_Prototype()
{	
	return S_OK;
}

HRESULT CBreakableBase::NativeConstruct(void * pArg)
{
	// GameInfo Init
	m_tGameInfo.iMaxHp = 4; 
	m_tGameInfo.iEnergy = rand() % 10 + 1;
	m_tGameInfo.iHp = m_tGameInfo.iMaxHp;
	m_tGameInfo.iSoul = rand() % 10 + 1;

	if (CBreakableBase::SetUp_Component(TEXT("Prototype_Component_Model_Breakable1")))
		return E_FAIL;

	// Test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(643.f, 10.f, 243.f, 1.f));

	return S_OK;
}

_int CBreakableBase::Tick(_float fTimeDelta)
{
	if (m_isDead)
		return -1;

	// 피격 중이다.
	if (m_bHitted)
	{
		// 피격중이면 셰이더에 m_fHitPower를 던져, 피격 효과를 주자.
		m_fHitPower -= 0.01f;
		if (m_fHitPower < 0)
		{
			m_fHitPower = 0.f;
			m_bHitted = false;
		}
	}

	// Collider 
	__super::Update_Colliders(m_pTransformCom->Get_WorldMatrix());

	return _int();
}

_int CBreakableBase::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	// 피가없으면 죽자~
	if (m_tGameInfo.iHp <= 0)
		m_isDead = true;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// AddRenderGroup
	{
		bool AddRenderGroup = false;
		if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 7.f))
			AddRenderGroup = true;

#ifdef USE_IMGUI
		// 디버깅이 필요하면 이거 넣어줘야한다
		if (m_bUseImGui)
			AddRenderGroup = true;
#endif

		if (AddRenderGroup)
		{
			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
				assert(0);
			if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
				assert(0);
		}
	}

	// Collider 
	pGameInstance->Add_Collision(this, true, m_pTransformCom.Get(), L"Layer_War", 8.f);


	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

HRESULT CBreakableBase::Render(_uint iPassIndex)
{
	if (FAILED(SetUp_ConstantTable(iPassIndex)))
		return E_FAIL;

	_uint	iNumMeshContainer = m_pModelCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainer; ++i)
	{
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Render(i, iPassIndex);
	}

	return S_OK;
}

HRESULT CBreakableBase::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
#ifdef _DEBUG
	// Collider 
	__super::Render_Colliders();
#endif // _DEBUG

#ifdef USE_IMGUI
	if (m_bUseImGui) 
	{
		CImguiManager::GetInstance()->Transform_Control(m_pTransformCom.Get(), m_CloneIdx, &m_bUseImGui);
	}
#endif

	return S_OK;
}


void CBreakableBase::OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
	// 플레이어 칼과 충돌했다.
	if (m_bHitted == false && pSrc->Get_ColliderTag() == COL_BREAKABLE_BODY &&
		pDst->Get_ColliderTag() == COL_WAR_WEAPON)
	{
		// 피격 당했다. 
		m_bHitted = true;
		m_fHitPower = 1.0f;
		m_tGameInfo.iHp -= pDst->Get_Owner()->m_tGameInfo.iAtt;
		return;
	}
}

void CBreakableBase::OnCollision_Stay(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{

}

void CBreakableBase::OnCollision_Leave(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{

}

HRESULT CBreakableBase::SetUp_Component(const _tchar* pModelProtoTag)
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 7.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(10.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)m_pTransformCom.GetAddressOf(), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)m_pRendererCom.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, pModelProtoTag, TEXT("Com_Model"), (CComponent**)m_pModelCom.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}

HRESULT CBreakableBase::SetUp_ConstantTable(_uint iPassIndex)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Bind Transform
	m_pTransformCom->Bind_OnShader(m_pModelCom.Get(), "g_WorldMatrix");
	if (iPassIndex == 3) // ShadowMap
	{
		m_pModelCom->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_View())), sizeof(_float4x4));
		m_pModelCom->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_Proj())), sizeof(_float4x4));
	}
	else
	{
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom.Get(), "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom.Get(), "g_ProjMatrix");
	}

	// Emissive, Normal Map 사용안함
	_bool falseTemp = false;
	m_pModelCom->Set_RawValue("g_UseEmissiveMap", &falseTemp, sizeof(_bool));
	m_pModelCom->Set_RawValue("g_UseNormalMap", &falseTemp, sizeof(_bool));

	// 피격시 색상 변경할꺼다. 노란색 ㄱ? 
	m_pModelCom->Set_RawValue("g_vHitPower", &XMVectorSet(m_fHitPower, 0.f, 0.f, 0.f), sizeof(_vector));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBreakableBase * CBreakableBase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBreakableBase*		pInstance = new CBreakableBase(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CBreakableBase");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBreakableBase::Clone(void* pArg)
{
	CBreakableBase*		pInstance = new CBreakableBase(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CBreakableBase");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBreakableBase::Free()
{
	__super::Free();
}



// ----------------------------------------------------------------
// CBreakable1

CBreakable1::CBreakable1(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CBreakableBase(pDevice, pDeviceContext)
{
}

CBreakable1::CBreakable1(const CBreakable1& rhs)
	: CBreakableBase(rhs)
{
}

HRESULT CBreakable1::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBreakable1::NativeConstruct(void* pArg)
{
	// GameInfo Init
	m_tGameInfo.iMaxHp = 4;
	m_tGameInfo.iEnergy = rand() % 10 + 1;
	m_tGameInfo.iHp = m_tGameInfo.iMaxHp;
	m_tGameInfo.iSoul = rand() % 10 + 1;

	if (CBreakableBase::SetUp_Component(TEXT("Prototype_Component_Model_Breakable1")))
		return E_FAIL;

	// Test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(643.f, 10.f, 243.f, 1.f));

	/* For.Com_Sphere */
	CCollider::COLLIDERDESC		ColliderDesc;
	ColliderDesc.vPivot = _float3(0.f, .0f, 0.f);
	ColliderDesc.fRadius = 0.75f;
	ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
	__super::Add_Collider(&ColliderDesc, COL_BREAKABLE_BODY);

	return S_OK;
}

_int CBreakable1::Tick(_float fTimeDelta)
{
	if (CBreakableBase::Tick(fTimeDelta) < 0)
		return -1;

	return _int();
}

_int CBreakable1::LateTick(_float fTimeDelta)
{
	if (CBreakableBase::LateTick(fTimeDelta) < 0)
		return -1;
	return 0;
}

HRESULT CBreakable1::Render(_uint iPassIndex)
{
	if (CBreakableBase::Render(iPassIndex) < 0)
		return -1;
	return S_OK;
}

HRESULT CBreakable1::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	CBreakableBase::PostRender(m_spriteBatch, m_spriteFont);

	return S_OK;
}

CBreakable1* CBreakable1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBreakable1* pInstance = new CBreakable1(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CBreakable1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBreakable1::Clone(void* pArg)
{
	CBreakable1* pInstance = new CBreakable1(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CBreakable1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBreakable1::Free()
{
	CBreakableBase::Free();
}

// --------------------------------------------------------------
// CBreakable2
CBreakable2::CBreakable2(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CBreakableBase(pDevice, pDeviceContext)
{
}

CBreakable2::CBreakable2(const CBreakable2& rhs)
	: CBreakableBase(rhs)
{
}

HRESULT CBreakable2::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBreakable2::NativeConstruct(void* pArg)
{
	// GameInfo Init
	m_tGameInfo.iMaxHp = 4;
	m_tGameInfo.iEnergy = rand() % 10 + 1;
	m_tGameInfo.iHp = m_tGameInfo.iMaxHp;
	m_tGameInfo.iSoul = rand() % 10 + 1;

	if (CBreakableBase::SetUp_Component(TEXT("Prototype_Component_Model_Breakable2")))
		return E_FAIL;

	// Test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(643.f, 10.f, 243.f, 1.f));

	/* For.Com_Sphere */
	CCollider::COLLIDERDESC		ColliderDesc;
	ColliderDesc.vPivot = _float3(0.f, .0f, 0.f);
	ColliderDesc.fRadius = 0.75f;
	ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
	__super::Add_Collider(&ColliderDesc, COL_BREAKABLE_BODY);

	return S_OK;
}

_int CBreakable2::Tick(_float fTimeDelta)
{
	if (CBreakableBase::Tick(fTimeDelta) < 0)
		return -1;

	return _int();
}

_int CBreakable2::LateTick(_float fTimeDelta)
{
	if (CBreakableBase::LateTick(fTimeDelta) < 0)
		return -1;
	return 0;
}

HRESULT CBreakable2::Render(_uint iPassIndex)
{
	if (CBreakableBase::Render(iPassIndex) < 0)
		return -1;
	return S_OK;
}

HRESULT CBreakable2::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	CBreakableBase::PostRender(m_spriteBatch, m_spriteFont);

	return S_OK;
}

CBreakable2* CBreakable2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBreakable2* pInstance = new CBreakable2(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CBreakable2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBreakable2::Clone(void* pArg)
{
	CBreakable2* pInstance = new CBreakable2(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CBreakable2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBreakable2::Free()
{
	CBreakableBase::Free();
}

// --------------------------------------------------------------
// CBreakable3
CBreakable3::CBreakable3(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CBreakableBase(pDevice, pDeviceContext)
{
}

CBreakable3::CBreakable3(const CBreakable3& rhs)
	: CBreakableBase(rhs)
{
}

HRESULT CBreakable3::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBreakable3::NativeConstruct(void* pArg)
{
	// GameInfo Init
	m_tGameInfo.iMaxHp = 4;
	m_tGameInfo.iEnergy = rand() % 10 + 1;
	m_tGameInfo.iHp = m_tGameInfo.iMaxHp;
	m_tGameInfo.iSoul = rand() % 10 + 1;

	if (CBreakableBase::SetUp_Component(TEXT("Prototype_Component_Model_Breakable3")))
		return E_FAIL;

	// Test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(643.f, 10.f, 243.f, 1.f));

	/* For.Com_Sphere */
	CCollider::COLLIDERDESC		ColliderDesc;
	ColliderDesc.vPivot = _float3(0.f, .5f, 0.f);
	ColliderDesc.fRadius = 1.25f;
	ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
	__super::Add_Collider(&ColliderDesc, COL_BREAKABLE_BODY);

	return S_OK;
}

_int CBreakable3::Tick(_float fTimeDelta)
{
	if (CBreakableBase::Tick(fTimeDelta) < 0)
		return -1;

	return _int();
}

_int CBreakable3::LateTick(_float fTimeDelta)
{
	if (CBreakableBase::LateTick(fTimeDelta) < 0)
		return -1;
	return 0;
}

HRESULT CBreakable3::Render(_uint iPassIndex)
{
	if (CBreakableBase::Render(iPassIndex) < 0)
		return -1;
	return S_OK;
}

HRESULT CBreakable3::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	CBreakableBase::PostRender(m_spriteBatch, m_spriteFont);

	return S_OK;
}

CBreakable3* CBreakable3::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBreakable3* pInstance = new CBreakable3(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CBreakable3");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBreakable3::Clone(void* pArg)
{
	CBreakable3* pInstance = new CBreakable3(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CBreakable3");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBreakable3::Free()
{
	CBreakableBase::Free();
}


// --------------------------------------------------------------
// CBreakable4
CBreakable4::CBreakable4(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CBreakableBase(pDevice, pDeviceContext)
{
}

CBreakable4::CBreakable4(const CBreakable4& rhs)
	: CBreakableBase(rhs)
{
}

HRESULT CBreakable4::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBreakable4::NativeConstruct(void* pArg)
{
	// GameInfo Init
	m_tGameInfo.iMaxHp = 4;
	m_tGameInfo.iEnergy = rand() % 10 + 1;
	m_tGameInfo.iHp = m_tGameInfo.iMaxHp;
	m_tGameInfo.iSoul = rand() % 10 + 1;

	if (CBreakableBase::SetUp_Component(TEXT("Prototype_Component_Model_Breakable4")))
		return E_FAIL;

	// Test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(643.f, 10.f, 243.f, 1.f));

	/* For.Com_Sphere */
	CCollider::COLLIDERDESC		ColliderDesc;
	ColliderDesc.vPivot = _float3(0.f, .5f, 0.f);
	ColliderDesc.fRadius = 2.0f;
	ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
	__super::Add_Collider(&ColliderDesc, COL_BREAKABLE_BODY);

	return S_OK;
}

_int CBreakable4::Tick(_float fTimeDelta)
{
	if (CBreakableBase::Tick(fTimeDelta) < 0)
		return -1;

	return _int();
}

_int CBreakable4::LateTick(_float fTimeDelta)
{
	if (CBreakableBase::LateTick(fTimeDelta) < 0)
		return -1;
	return 0;
}

HRESULT CBreakable4::Render(_uint iPassIndex)
{
	if (CBreakableBase::Render(iPassIndex) < 0)
		return -1;
	return S_OK;
}

HRESULT CBreakable4::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	CBreakableBase::PostRender(m_spriteBatch, m_spriteFont);

	return S_OK;
}

CBreakable4* CBreakable4::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBreakable4* pInstance = new CBreakable4(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CBreakable4");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBreakable4::Clone(void* pArg)
{
	CBreakable4* pInstance = new CBreakable4(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CBreakable4");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBreakable4::Free()
{
	CBreakableBase::Free();
}


// --------------------------------------------------------------
// CBreakable5
CBreakable5::CBreakable5(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CBreakableBase(pDevice, pDeviceContext)
{
}

CBreakable5::CBreakable5(const CBreakable5& rhs)
	: CBreakableBase(rhs)
{
}

HRESULT CBreakable5::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBreakable5::NativeConstruct(void* pArg)
{
	// GameInfo Init
	m_tGameInfo.iMaxHp = 4;
	m_tGameInfo.iEnergy = rand() % 10 + 1;
	m_tGameInfo.iHp = m_tGameInfo.iMaxHp;
	m_tGameInfo.iSoul = rand() % 10 + 1;

	if (CBreakableBase::SetUp_Component(TEXT("Prototype_Component_Model_Breakable5")))
		return E_FAIL;

	// Test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(643.f, 10.f, 243.f, 1.f));

	/* For.Com_Sphere */
	CCollider::COLLIDERDESC		ColliderDesc;
	ColliderDesc.vPivot = _float3(0.f, 1.f, 0.f);
	ColliderDesc.fRadius = 0.75f;
	ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
	__super::Add_Collider(&ColliderDesc, COL_BREAKABLE_BODY);

	return S_OK;
}

_int CBreakable5::Tick(_float fTimeDelta)
{
	if (CBreakableBase::Tick(fTimeDelta) < 0)
		return -1;

	return _int();
}

_int CBreakable5::LateTick(_float fTimeDelta)
{
	if (CBreakableBase::LateTick(fTimeDelta) < 0)
		return -1;
	return 0;
}

HRESULT CBreakable5::Render(_uint iPassIndex)
{
	if (CBreakableBase::Render(iPassIndex) < 0)
		return -1;
	return S_OK;
}

HRESULT CBreakable5::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	CBreakableBase::PostRender(m_spriteBatch, m_spriteFont);

	return S_OK;
}

CBreakable5* CBreakable5::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBreakable5* pInstance = new CBreakable5(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CBreakable5");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBreakable5::Clone(void* pArg)
{
	CBreakable5* pInstance = new CBreakable5(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CBreakable5");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBreakable5::Free()
{
	CBreakableBase::Free();
}

