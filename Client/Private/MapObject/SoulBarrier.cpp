#include "stdafx.h"
#include "..\public\MapObject/SoulBarrier.h"
#include "GameInstance.h"

#ifdef USE_IMGUI
#include "imgui_Manager.h"
#endif

CSoulBarrier::CSoulBarrier(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSoulBarrier::CSoulBarrier(const CSoulBarrier& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSoulBarrier::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CSoulBarrier::NativeConstruct(void* pArg)
{
	if (SetUp_Component())
		return E_FAIL;

	// SoulBarrier ���� �浹ü
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vPivot = _float3(0.f, 1.25f, 0.f);
	ColliderDesc.fRadius = 2.5f;
	ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
	__super::Add_Collider(&ColliderDesc, COL_BALLISTA_BODY);

	m_pCurState = "Dn_SoulBarrier.ao|Dn_SoulBarrier_Idle";
	m_pNextState = "Dn_SoulBarrier.ao|Dn_SoulBarrier_Activate"; // Activate�ϴ� ������ ����.

	// GameInfo Init
	m_tGameInfo.iMaxHp = 14; // 5��
	m_tGameInfo.iHp = m_tGameInfo.iMaxHp;

	// Test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(614.f, 11.f, 142.f, 1.f));

	// ũ��� 2��� Ű����
	m_pTransformCom->Set_Scale(_float3(2.f, 2.f, 2.f));


	return S_OK;
}

_int CSoulBarrier::Tick(_float fTimeDelta)
{
	// ��� ���ʹ� ������ -1�� ��ȯ�Ѵ�
	if (m_isDead)
	{
		return -1;
	}

	// GlobalState
	{
		// �ǰ� ���̴�.
		if (m_bHitted)
		{
			// �ǰ����̸� ���̴��� m_fHitPower�� ����, �ǰ� ȿ���� ����.
			m_fHitPower -= 0.01f;
			if (m_fHitPower < 0)
			{
				m_fHitPower = 0.f;
				m_bHitted = false;
			}
		}
	}

	// UpdateState
	{
		if (m_pCurState != m_pNextState)
		{
			_bool isLoop = true;
			if (m_pNextState == "Dn_SoulBarrier.ao|Dn_SoulBarrier_Death" || 
				m_pNextState == "Dn_SoulBarrier.ao|Dn_SoulBarrier_Activate" ||
				m_pNextState == "Dn_SoulBarrier.ao|Dn_SoulBarrier_Impact")
			{
				isLoop = false;
			}

			m_pModelCom->SetUp_Animation(m_pNextState, isLoop);
			m_pCurState = m_pNextState;
		}
	}

	// Update Animation
	if (m_bInitAnimation) // �̺�Ʈ ������ set������.
		m_pModelCom->Update_Animation(fTimeDelta);


	// DoState
	{
		if (m_pCurState == "Dn_SoulBarrier.ao|Dn_SoulBarrier_Activate")
		{
			if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
			{
				m_pNextState = "Dn_SoulBarrier.ao|Dn_SoulBarrier_Idle";
			}
		}
		else if (m_pCurState == "Dn_SoulBarrier.ao|Dn_SoulBarrier_Death")
		{
			if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
			{
				m_isDead = true;
			}
		}
		else if (m_pCurState == "Dn_SoulBarrier.ao|Dn_SoulBarrier_Impact")
		{
			if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
			{
				m_pNextState = "Dn_SoulBarrier.ao|Dn_SoulBarrier_Idle";
			}
		}
	}

	//ColliderUpdate
	Update_Colliders(m_pTransformCom->Get_WorldMatrix());

	return _int();
}

_int CSoulBarrier::LateTick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// ü���� 0���ϰ� �Ǹ� ����. 
	// �ٷ� ���������� �״� ��� �� ������ ������
	if (m_tGameInfo.iHp <= 0 && m_bWillDead == false)
	{
		m_bWillDead = true;
		m_pNextState = "Dn_SoulBarrier.ao|Dn_SoulBarrier_Death";
	}

	// AddRenderGroup
	bool AddRenderGroup = false;
	if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 7.f))
		AddRenderGroup = true;

#ifdef USE_IMGUI
	if (m_bUseImGui) // IMGUI ���� ��ġ�ҰŴ�
		AddRenderGroup = true;
#endif

	if (AddRenderGroup)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			assert(0);
		if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
			assert(0);
	}

	// �÷��̾ ��ó�� ������ Collider�� �����Ű��.
	pGameInstance->Add_Collision(this, true, m_pTransformCom, L"Layer_War", 7.f);

	RELEASE_INSTANCE(CGameInstance);
	return 0;
}

HRESULT CSoulBarrier::Render(_uint iPassIndex)
{
	// Render SoulBarrier
	{
		if (FAILED(SetUp_ConstantTable(iPassIndex)))
			return E_FAIL;

		// Render
		_uint	iNumMeshContainer = m_pModelCom->Get_NumMeshContainer();

		for (_uint i = 0; i < iNumMeshContainer; ++i)
		{
			m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
			m_pModelCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
			m_pModelCom->Set_ShaderResourceView("g_EmissiveTexture", i, aiTextureType_EMISSIVE);

			m_pModelCom->Render(i, iPassIndex);
		}
	}

	return S_OK;
}

HRESULT CSoulBarrier::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	CGameObject::Render_Colliders();

#ifdef USE_IMGUI
	if (m_bUseImGui) // IMGUI ���� ��ġ�ҰŴ�
	{
		CImguiManager::GetInstance()->Transform_Control(m_pTransformCom, m_CloneIdx, &m_bUseImGui);
	}
#endif

	return S_OK;
}

HRESULT CSoulBarrier::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 7.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(10.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SoulBarrier"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSoulBarrier::SetUp_ConstantTable(_uint iPassIndex)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

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

	// �ǰݽ� ���� �����Ҳ���. ����������
	m_pModelCom->Set_RawValue("g_vHitPower", &XMVectorSet(m_fHitPower, 0.f, 0.f, 0.f), sizeof(_vector));

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CSoulBarrier::OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
	// SoulBarrier ����� �÷��̾� ���� �浹�� ���. 
	if (m_bHitted == false && pSrc->Get_ColliderTag() == COL_BALLISTA_BODY &&
		pDst->Get_ColliderTag() == COL_WAR_WEAPON)
	{
		// �ǰ� ���ߴ�. 
		m_bHitted = true;
		m_fHitPower = 1.f;
		m_tGameInfo.iHp -= pDst->Get_Owner()->m_tGameInfo.iAtt;
		if (false == m_bWillDead) 
			// �������� �ִϸ��̼� ������������.
			m_pNextState = "Dn_SoulBarrier.ao|Dn_SoulBarrier_Impact";

		return;
	}
}

void CSoulBarrier::OnCollision_Stay(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
}

void CSoulBarrier::OnCollision_Leave(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
}


CSoulBarrier* CSoulBarrier::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CSoulBarrier* pInstance = new CSoulBarrier(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CSoulBarrier");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CSoulBarrier::Clone(void* pArg)
{
	CSoulBarrier* pInstance = new CSoulBarrier(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CSoulBarrier");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSoulBarrier::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

