#include "stdafx.h"
#include "..\public\MapObject/Ballista.h"
#include "GameInstance.h"

#ifdef USE_IMGUI
#include "imgui_Manager.h"
#endif

#include "ParticleSystem\ParticleSystem_Manager.h"

CBallista::CBallista(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBallista::CBallista(const CBallista& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBallista::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBallista::NativeConstruct(void* pArg)
{
	if (SetUp_Component())
		return E_FAIL;

	// 발리스타 몸통 충돌체
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ColliderDesc.fRadius = 3.0f;
	ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
	__super::Add_Collider(&ColliderDesc, COL_BALLISTA_BODY);

	m_pModelCom->SetUp_Animation("Ballista_A.ao|Balliista_A_Idle");

	// GameInfo Init
	m_tGameInfo.iAtt = 2;
	m_tGameInfo.iEnergy = rand() % 10 + 1;
	m_tGameInfo.iMaxHp = 15; //5대
	m_tGameInfo.iHp = m_tGameInfo.iMaxHp;
	m_tGameInfo.iSoul = rand() % 10 + 1;

	return S_OK;
}

_int CBallista::Tick(_float fTimeDelta)
{

	// CBallista는 죽으면 죽은 상태 유지하자
	if (m_isDead)
	{
		return 0;
	}

	// 키프레임 1일때 화살을 생성하자.
	// 그리고 고블린도 생성하자
	// 그러나 죽을라고 할때는 하지말자
	if (m_bWillDead == false)
	{
		if (boltOnce == false && m_pModelCom->Get_Current_KeyFrame_Index("Ballista_A.ao|Ballista_A_Full") == 1)
		{
			if (FAILED(CObject_Manager::GetInstance()->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Ballista_Bolt",
				L"Prototype_GameObject_Ballista_Bolt", this)))
				assert(0);
			boltOnce = true;
		}

		// 키프레임 대충 10 보다 클때 다시 락 풀고 
		if (boltOnce)
		{
			if (m_pModelCom->Get_Current_KeyFrame_Index("Ballista_A.ao|Ballista_A_Full") > 10)
			{
				boltOnce = false;
			}
		}
	}

	m_pModelCom->Update_Animation(fTimeDelta, nullptr, nullptr, nullptr,DIR_F, 0);
	if (m_pCurState == "Ballista_A.ao|Ballista_A_Full") // 바리스타 발사 동작중일때만 고블린 애니메이션 업데이트 하자
	{
		m_pModelGoblinCom->Update_Animation(fTimeDelta);
	}

	// GlobalState
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

	// UpdateState
	{
		if (m_pCurState != m_pNextState)
		{
			_bool isLoop = true;

			if (m_pNextState == "Ballista_A.ao|Balliista_A_Idle" ||
				m_pNextState == "Ballista_A.ao|Ballista_A_Full"
				)
			{
				isLoop = true;
			}
			else if (m_pNextState == "Ballista_A.ao|Ballista_A_Impact")
			{
				isLoop = false;
			}
			
			else if (m_pNextState == "Ballista_A_Destroyed.ao|Ballista_A_Explode")
			{
				isLoop = false;

				// 사운드
				SoundManager::Get_Instance()->ForcePlay(L"prop_ballista_sequence_wall_explosion.ogg", SoundManager::CHANNELID::BREAKABLE2, 0.5f);
			}
			if (m_pNextState == "Ballista_A.ao|Ballista_A_Full") // Idle일때는 대충 아무거나 고블린이 던져서 발리스타 밑에 깔리게 끔하자.
				m_pModelGoblinCom->SetUp_Animation("Goblin_Armor_Mesh.ao|Goblin_Ballista_Full", isLoop);

			m_pModelCom->SetUp_Animation(m_pNextState, isLoop);
			m_pCurState = m_pNextState;
		}
	}

	// DoState
	{
		// 아이들 상태에서 피격당하면 히트파워 주고, Impact로 보내자. 
		if (m_pCurState == "Ballista_A.ao|Ballista_A_Impact")
		{
			if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
			{
				m_pNextState = "Ballista_A.ao|Balliista_A_Idle";
			}
		}
		else if (m_pCurState == "Ballista_A_Destroyed.ao|Ballista_A_Explode")
		{
			if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
			{
				m_isDead = true;
			}
		}
		else if (m_pCurState == "Ballista_A.ao|Ballista_A_Full")
		{
			if (forSoundBool2 == false || forSoundBool2 == false)
				forSoundTimeAcc += fTimeDelta;

#ifdef _DEBUG
			if (CInput_Device::GetInstance()->Key_Down(DIK_0))
			{
				forSoundBool2 = false;
				forSoundBool1 = false;
				forSoundTimeAcc = 0.f;
			}
#endif
			if (forSoundBool1 == false && forSoundTimeAcc > 1.3f)
			{
				SoundManager::Get_Instance()->ForcePlay(L"prop_ballista_load.ogg", SoundManager::BREAKABLE7, 5.f);
				forSoundBool1 = true;
			}

			if (forSoundBool2 == false && forSoundTimeAcc > 7.75f)
			{
				SoundManager::Get_Instance()->ForcePlay(L"prop_ballista_fleamag_shoot.ogg", SoundManager::BREAKABLE6, 5.f);
				forSoundBool2 = true;
			}
		}
	}

	// Collider Update
	Update_Colliders(m_pTransformCom->Get_WorldMatrix());

	return _int();
}

_int CBallista::LateTick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 체력이 0이하가 되면 죽자. 
	// 바로 죽이지말고 죽는 모션 다 끝나면 죽이자
	if (m_tGameInfo.iHp <= 0 && m_bWillDead == false)
	{
		m_bWillDead = true;
		Safe_Release(m_pModelCom);
		m_pModelCom = m_pModelDestroyedCom; // 터지는 모델로 변경
		m_pNextState = "Ballista_A_Destroyed.ao|Ballista_A_Explode";
	}

	// AddRenderGroup
	bool AddRenderGroup = false;
	if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 150.f))
		AddRenderGroup = true;

#ifdef USE_IMGUI
	if (m_bUseImGui) // IMGUI 툴로 배치할거다
		AddRenderGroup = true;
#endif

	if (AddRenderGroup)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			assert(0);
#ifdef _DEBUG
		if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
			assert(0);
#endif
	}

	// 플레이어가 근처에 있으면 Collider를 실행시키자.
	if (m_isDead == false)
		pGameInstance->Add_Collision(this, true, m_pTransformCom, L"Layer_War", 25.f);

	RELEASE_INSTANCE(CGameInstance);
	return 0;
}

HRESULT CBallista::Render(_uint iPassIndex)
{
	// Render Ballista
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

	// Render Goblin
	if (m_pCurState == "Ballista_A.ao|Ballista_A_Full")
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		// Bind Transform
		_float4x4	modelWorldMat;
		_matrix goblinMat = XMMatrixRotationY(XMConvertToRadians(180)) * XMLoadFloat4x4(m_pTransformCom->Get_WorldFloat4x4Ptr());
		XMStoreFloat4x4(&modelWorldMat, XMMatrixTranspose(goblinMat)); // 180도 y축으로 회전해서 바인드하자
		m_pModelGoblinCom->Set_RawValue("g_WorldMatrix", &modelWorldMat, sizeof(_float4x4));
		if (iPassIndex == 3) // shadow map
		{
			m_pModelGoblinCom->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_View())), sizeof(_float4x4));
			m_pModelGoblinCom->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_Proj())), sizeof(_float4x4));
		}
		else
		{
			pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelGoblinCom, "g_ViewMatrix");
			pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelGoblinCom, "g_ProjMatrix");
		}

		// Branch to Use Normal Mapping
		m_pModelGoblinCom->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));

		// Branch to Use Emissive Mapping
		m_pModelGoblinCom->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));

		// 피격시 색상은 없다.
		m_pModelGoblinCom->Set_RawValue("g_vHitPower", &XMVectorSet(0.f, 0.f, 0.f, 0.f), sizeof(_vector));

		_float tempFloat = 0.f;
		m_pModelGoblinCom->Set_RawValue("g_DissolvePwr", &tempFloat, sizeof(_float));

		RELEASE_INSTANCE(CGameInstance);

		// Render
		_uint	iNumMeshContainer = m_pModelGoblinCom->Get_NumMeshContainer();

		for (_uint i = 0; i < iNumMeshContainer; ++i)
		{
			m_pModelGoblinCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
			m_pModelGoblinCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
			m_pModelGoblinCom->Set_ShaderResourceView("g_EmissiveTexture", i, aiTextureType_EMISSIVE);
			m_pModelGoblinCom->Render(i, iPassIndex);
		}
	}

	return S_OK;
}

HRESULT CBallista::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
#ifdef _DEBUG
	CGameObject::Render_Colliders();
#endif

#ifdef USE_IMGUI
	if (m_bUseImGui) // IMGUI 툴로 배치할거다
	{
		CImguiManager::GetInstance()->Transform_Control(m_pTransformCom, m_CloneIdx, &m_bUseImGui);
	}
#endif

	return S_OK;
}

HRESULT CBallista::SetUp_Component()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Ballista"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Model_Destroyed */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Ballista_Destroyed"), TEXT("Com_Model_Destroyed"), (CComponent**)&m_pModelDestroyedCom)))
		return E_FAIL;

	/* For.Com_Model_Goblin */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Goblin_Armor"), TEXT("Com_Model_Goblin"), (CComponent**)&m_pModelGoblinCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBallista::SetUp_ConstantTable(_uint iPassIndex)
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

	// 피격시 색상 변경할꺼다.
	m_pModelCom->Set_RawValue("g_vHitPower", &XMVectorSet(m_fHitPower, 0.f, 0.f, 0.f), sizeof(_vector));

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CBallista::OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
	// 바리스타 몸통과 플레이어 검이 충돌한 경우. 
	if (m_bHitted == false && pSrc->Get_ColliderTag() == COL_BALLISTA_BODY &&
		pDst->Get_ColliderTag() == COL_WAR_WEAPON)
	{
		// 사운드 
		SoundManager::Get_Instance()->ForcePlay(L"imp_sword_01.ogg", SoundManager::CHANNELID::HOLLOLORD, MONSTER_HIT_VOLUME);

		m_bHitted = true;
		m_fHitPower = 1.f;

		m_tGameInfo.iHp -= pDst->Get_Owner()->m_tGameInfo.iAtt;
#ifdef _DEBUG
		cout << DXString::WideToChar(this->m_pLayerTag) << ": " << m_tGameInfo.iHp << endl;
#endif
		if (false == m_bWillDead)
			m_pNextState = "Ballista_A.ao|Ballista_A_Impact";

		return;
	}
}

void CBallista::OnCollision_Stay(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
}

void CBallista::OnCollision_Leave(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
}


CBallista* CBallista::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBallista* pInstance = new CBallista(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CBallista");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CBallista::Clone(void* pArg)
{
	CBallista* pInstance = new CBallista(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CBallista");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBallista::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelDestroyedCom);
	Safe_Release(m_pModelGoblinCom);
}







































// --------------------------------------------------------------------------------------
// Ballista_Bolt


CBallista_Bolt::CBallista_Bolt(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBallista_Bolt::CBallista_Bolt(const CBallista& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBallista_Bolt::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBallista_Bolt::NativeConstruct(void* pArg)
{
	if (SetUp_Component())
		return E_FAIL;

	// pArg는 pOwner가 될것이다. 
	if (pArg)
	{
		m_pOwner = static_cast<CGameObject*>(pArg);
	}

	// 발리스타 볼트 충돌체
	{
		CModel* pBallistaModel = (CModel*)m_pOwner->Get_ComponentPtr(L"Com_Model");
		if (nullptr == pBallistaModel)
			assert(0);

		CCollider::COLLIDERDESC		ColliderDesc;
		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vPivot = _float3(0.f, -6.5f, -2.5f);
		ColliderDesc.fRadius = 2.0f;
		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, COL_MONSTER_WEAPON);

		ZeroMemory(&m_spearDesc, sizeof(SPEARDESC));
		m_spearDesc.pBoneMatrix = pBallistaModel->Get_CombinedMatrixPtr("Bone_BB_Bolt");
		m_spearDesc.OffsetMatrix = pBallistaModel->Get_OffsetMatrix("Bone_BB_Bolt");
		m_spearDesc.PivotMatrix = pBallistaModel->Get_PivotMatrix_Bones();

		CTransform* pBallistaTransform = (CTransform*)m_pOwner->Get_ComponentPtr(L"Com_Transform");
		if (nullptr == pBallistaTransform)
			assert(0);
		m_spearDesc.pTargetWorldMatrix = pBallistaTransform->Get_WorldFloat4x4Ptr();
	}

	return S_OK;
}

_int CBallista_Bolt::Tick(_float fTimeDelta)
{
	if (m_isDead)
		return -1;

	m_fLifeTime += fTimeDelta;
	// 충돌체 위치 Update
	{
		// For Weapon Collider
		for (auto pCollider : m_ColliderList)
		{
			if (pCollider->Get_ColliderTag() == COL_MONSTER_WEAPON)
			{
				_matrix		OffsetMatrix = XMLoadFloat4x4(&m_spearDesc.OffsetMatrix);
				_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_spearDesc.pBoneMatrix);
				_matrix		PivotMatrix = XMLoadFloat4x4(&m_spearDesc.PivotMatrix);
				_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_spearDesc.pTargetWorldMatrix);
				_matrix		TransformationMatrix =
					(OffsetMatrix * CombinedTransformationMatrix * PivotMatrix) *
					TargetWorldMatrix;
				pCollider->Update(TransformationMatrix);
			}
			else
			{
				pCollider->Update(m_pTransformCom->Get_WorldMatrix());
			}
		}
	}

	return _int();
}

_int CBallista_Bolt::LateTick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_fLifeTime > 15) // 대충 3초 이상 살았으면 죽이자 
		m_isDead = true;

	if (1)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			assert(0);
#ifdef _DEBUG
		if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
			assert(0);
#endif
	}

	// Collider 
	pGameInstance->Add_Collision(this, true, m_pTransformCom, L"Layer_War", 20.f);

	

	RELEASE_INSTANCE(CGameInstance);
	return 0;
}

HRESULT CBallista_Bolt::Render(_uint iPassIndex)
{
	if (FAILED(SetUp_ConstantTable(iPassIndex)))
		return E_FAIL;

	// Render
	_uint	iNumMeshContainer = m_pModelCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainer; ++i)
	{
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);

		m_pModelCom->Render(i, iPassIndex);
	}

	return S_OK;
}

HRESULT CBallista_Bolt::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
#ifdef _DEBUG
	CGameObject::Render_Colliders();
#endif

	return S_OK;
}

HRESULT CBallista_Bolt::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 20.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(10.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Ballista_Bolt"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBallista_Bolt::SetUp_ConstantTable(_uint iPassIndex)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Bind Matrix
	_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_spearDesc.pBoneMatrix);
	_matrix		OffsetMatrix = XMLoadFloat4x4(&m_spearDesc.OffsetMatrix);
	_matrix		PivotMatrix = XMLoadFloat4x4(&m_spearDesc.PivotMatrix);
	_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_spearDesc.pTargetWorldMatrix);

	// 이게 정답인가 보네...
	_matrix		TransformationMatrix = XMMatrixRotationX(XMConvertToRadians(-90)) /** OffsetMatrix*/ * CombinedTransformationMatrix * PivotMatrix * TargetWorldMatrix;
	_float4x4	modelWorldMat;
	XMStoreFloat4x4(&modelWorldMat, XMMatrixTranspose(TransformationMatrix));
	m_pModelCom->Set_RawValue("g_WorldMatrix", &modelWorldMat, sizeof(_float4x4));

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
	m_pModelCom->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}


CBallista_Bolt* CBallista_Bolt::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBallista_Bolt* pInstance = new CBallista_Bolt(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CBallista_Bolt");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CBallista_Bolt::Clone(void* pArg)
{
	CBallista_Bolt* pInstance = new CBallista_Bolt(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CBallista_Bolt");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBallista_Bolt::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}