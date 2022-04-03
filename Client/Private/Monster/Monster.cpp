#include "stdafx.h"
#include "..\public\Monster\Monster.h"

#include "GameInstance.h"


CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonster::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CMonster::NativeConstruct(void * pArg)
{
	// 모든 몬스터는 m_pTransformCom, m_pRendererCom, m_pNaviCom를 가진다
	if (SetUp_Component())
		return E_FAIL;	

	return S_OK;
}

_int CMonster::Tick(_float fTimeDelta)
{
	// 모든 몬스터는 죽으면 m_Objects 에서 제거 당해야한다
	if (m_isDead)
		return -1;

	// 모든 몬스터는 타겟팅을 설정한다
	if (!m_bTargetingOnce)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		auto pLayer_War = pGameInstance->Get_GameObject_CloneList(TEXT("Layer_War"));
		if (pLayer_War)
		{
			m_pTarget = pLayer_War->front();
			Safe_AddRef(m_pTarget);
			m_pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_ComponentPtr(L"Com_Transform"));
			m_bTargetingOnce = true;
		}
		else
		{
			RELEASE_INSTANCE(CGameInstance);
			return 0;
		}
		RELEASE_INSTANCE(CGameInstance)
	}

	// 모든 몬스터는 Collider list 를 update해야한다
	Update_Colliders(m_pTransformCom->Get_WorldMatrix());

	return _int();
}

_int CMonster::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 모든 몬스터는 Navi 따라서 Height를 타야한다. 하지만 죽기직전 날아갈때에는 태우지말자.
	if (m_bHeight)
	{
		_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float curFloorHeight = m_pNaviCom->Compute_Height(vPosition);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, curFloorHeight));
	}

	// 모든 몬스터는 Nonalpha 그룹에서 render한다
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		return 0;

	// 모든 몬스터는 Post Render를 진행한다 그룹에서 render한다
	if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
		return 0;

	// 모든 몬스터는 자기가 가지고 있는 Collider list를 collider manager에 등록하여 충돌처리를 진행한다
	pGameInstance->Add_Collision(this);

	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

HRESULT CMonster::Render(_uint iPassIndex)
{
	// 모든 몬스터는 아래와 같이 Render를 진행한다
	if (FAILED(SetUp_ConstantTable(iPassIndex)))
		return E_FAIL;

	_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
		m_pModelCom->Set_ShaderResourceView("g_EmissiveTexture", i, aiTextureType_EMISSIVE);

		m_pModelCom->Render(i, iPassIndex);
	}

	return S_OK;
}

HRESULT CMonster::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);
#ifdef _DEBUG
	// 모든 몬스터는 Collider를 render한다
	__super::Render_Colliders();
#endif

	return S_OK;
}

HRESULT CMonster::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Navi */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navi"), (CComponent**)&m_pNaviCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::SetUp_ConstantTable(_uint iPassIndex)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);	
	
	// Bind Transform
	m_pTransformCom->Bind_OnShader(m_pModelCom, "g_WorldMatrix");
	if (iPassIndex == 3) // shadow map
	{
		m_pModelCom->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_View())), sizeof(_float4x4));
		m_pModelCom->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_Proj())), sizeof(_float4x4));
	}
	else
	{
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom, "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom, "g_ProjMatrix");
	}

	// Branch to Use Normal Mapping
	m_pModelCom->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));

	// Branch to Use Emissive Mapping
	m_pModelCom->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));

	// 피격시 색상 변경할꺼다.
	m_pModelCom->Set_RawValue("g_vHitPower", &XMVectorSet(m_fHitPower, 0.f, 0.f, 0.f), sizeof(_vector));


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

bool CMonster::isTargetFront(CTransform* pTargetTransform)
{
	_vector toTarget = pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	if (XMVectorGetX(XMVector3Dot(toTarget, m_pTransformCom->Get_State(CTransform::STATE_LOOK))) >= 0) // 타겟이 내 앞에 있다. 
		return true;
	return false;
}

void CMonster::OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
	// 몬스터 몸통과 플레이어 검이 충돌한 경우. 
	if (m_bHitted == false && pSrc->Get_ColliderTag() == COL_MONSTER_BODY1 &&
		pDst->Get_ColliderTag() == COL_WAR_WEAPON)
	{
		// 피격 당했다. 
		m_bHitted = true;
		m_fHitPower = .8f;

		m_tGameInfo.iHp -= pDst->Get_Owner()->m_tGameInfo.iAtt;
#ifdef _DEBUG
		cout << DXString::WideToChar(this->m_pLayerTag) << ": " << m_tGameInfo.iHp << endl;
#endif

		// 피격시 피격모션으로 천이하자.
		// 하지만 슈퍼아머 상태에서는 피격상태로 천이가 안된다. 데미지만 입는다. 
		if (m_bSuperArmor == false)
		{
			if (m_pImpactState_B != nullptr || m_pImpactState_F != nullptr) // 피격 애니메이션 없으면 무시~
			{
				assert(m_pTargetTransform); // Something Wrong...
				if (isTargetFront(m_pTargetTransform))
					// 플레이어가 내 앞에 있으면 m_pImpactState_B, 아닌경우 m_pImpactState_F
					m_pNextState = m_pImpactState_B;
				else
					m_pNextState = m_pImpactState_F;
			}
		}

		return;
	}
}

void CMonster::OnCollision_Stay(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{

}

void CMonster::OnCollision_Leave(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{

}

void CMonster::DoGlobalState(float fTimeDelta)
{
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
}


CMonster * CMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMonster*		pInstance = new CMonster(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMonster::Clone(void* pArg)
{
	CMonster*		pInstance = new CMonster(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster::Free()
{

	__super::Free();

	Safe_Release(m_pTarget);
	Safe_Release(m_pNaviCom);
	Safe_Release(m_pTransformCom);	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
