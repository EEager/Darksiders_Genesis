#include "stdafx.h"
#include "..\public\MapObject\Breakables.h"

#include "GameInstance.h"
#include "Transform.h"

#ifdef USE_IMGUI
#include "imgui_Manager.h"
#endif

CBreakable1::CBreakable1(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBreakable1::CBreakable1(const CBreakable1& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBreakable1::NativeConstruct_Prototype()
{	
	return S_OK;
}

HRESULT CBreakable1::NativeConstruct(void * pArg)
{
	// GameInfo Init
	m_tGameInfo.iMaxHp = 4; // 2방맞고 펑
	m_tGameInfo.iEnergy = rand() % 10 + 1;
	m_tGameInfo.iHp = m_tGameInfo.iMaxHp;
	m_tGameInfo.iSoul = rand() % 10 + 1;

	if (SetUp_Component())
		return E_FAIL;

	// Test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(230.f + 2.f, 1.f, 430.f, 1.f));

	return S_OK;
}

_int CBreakable1::Tick(_float fTimeDelta)
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

	// Init
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	m_pTransformCom->Turn(XMVectorSet(1.f, 0.f, 0.f, 0.f), fTimeDelta*2);
	m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta*3);

	// Collider 
	__super::Update_Colliders(m_pTransformCom->Get_WorldMatrix());

	return _int();
}

_int CBreakable1::LateTick(_float fTimeDelta)
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
		if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
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
	pGameInstance->Add_Collision(this);


	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

HRESULT CBreakable1::Render(_uint iPassIndex)
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

HRESULT CBreakable1::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
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


void CBreakable1::OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
	// 플레이어 칼과 충돌했다.
	if (m_bHitted == false && pSrc->Get_ColliderTag() == COL_MONSTER_BODY1 &&
		pDst->Get_ColliderTag() == COL_WAR_WEAPON)
	{
		// 피격 당했다. 
		m_bHitted = true;
		m_fHitPower = 1.0f;
		m_tGameInfo.iHp -= pDst->Get_Owner()->m_tGameInfo.iAtt;
		return;
	}
}

void CBreakable1::OnCollision_Stay(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{

}

void CBreakable1::OnCollision_Leave(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{

}

HRESULT CBreakable1::SetUp_Component()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Breakable1"), TEXT("Com_Model"), (CComponent**)m_pModelCom.GetAddressOf())))
		return E_FAIL;


	/* For.Com_Sphere */
	CCollider::COLLIDERDESC		ColliderDesc;
	ColliderDesc.vPivot = _float3(0.f, 1.f, 0.f);
	ColliderDesc.fRadius = 0.75f;
	ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
	__super::Add_Collider(&ColliderDesc, COL_MONSTER_BODY1);


	return S_OK;
}

HRESULT CBreakable1::SetUp_ConstantTable(_uint iPassIndex)
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


CBreakable1 * CBreakable1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBreakable1*		pInstance = new CBreakable1(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CBreakable1");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBreakable1::Clone(void* pArg)
{
	CBreakable1*		pInstance = new CBreakable1(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CBreakable1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBreakable1::Free()
{
	__super::Free();
}
