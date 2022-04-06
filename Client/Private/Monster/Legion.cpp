#include "stdafx.h"
#include "..\public\Monster\Legion.h"
#include "GameInstance.h"

#include "MapObject\Ballista.h"

#ifdef USE_IMGUI
#include "imgui_Manager.h"
#endif


CLegion::CLegion(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{
}

CLegion::CLegion(const CLegion & rhs)
	: CMonster(rhs)
{
}

HRESULT CLegion::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CLegion::NativeConstruct(void * pArg)
{
	// 죽기직전에 날아가는 방향과 힘. y는 위쪽으로만 날아가게하자.
	m_vFloatingDir = _float4(MathHelper::RandF(-1.f, 1.f), MathHelper::RandF(-0.1f, 1.f), MathHelper::RandF(-1.f, 1.f), 0.f);
	m_fFloatingPwr = MathHelper::RandF(5.f, 7.f);

	// GameInfo Init
	m_tGameInfo.iAtt = 2;
	m_tGameInfo.iEnergy = rand() % 10 + 1;
	m_tGameInfo.iMaxHp = 8;
	m_tGameInfo.iHp = m_tGameInfo.iMaxHp;
	m_tGameInfo.iSoul = rand() % 10 + 1;

	m_fSpeed = 8.f;
	// 모든 몬스터는 m_pTransformCom, m_pRendererCom, m_pNaviCom를 가진다. 
	if (CMonster::NativeConstruct(pArg))
		return E_FAIL;	

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_MonsterHp_PointGS"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIHpBarGsBufferCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legion"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Collider */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vPivot = _float3(0.f, 1.0f, 0.f);
	ColliderDesc.vSize = _float3(1.5f, 2.0f, 1.5f);
	ColliderDesc.eColType = CCollider::COL_TYPE::COL_TYPE_AABB;
	__super::Add_Collider(&ColliderDesc, COL_MONSTER_BODY1);


	// For Weapon
	{ 
		/* For.Com_Model_Legion_WeaponL */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legion_Axe_L"), TEXT("Com_Model_Legion_WeaponL"), (CComponent**)&m_pModelWeaponLCom)))
			return E_FAIL;
		/* For.Com_Model_Legion_WeaponR */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legion_Axe_R"), TEXT("Com_Model_Legion_WeaponR"), (CComponent**)&m_pModelWeaponRCom)))
			return E_FAIL;

		/* For.LegionWeapon */
		ColliderDesc.vPivot = _float3(0.f, 0.0f, -.75f); // For Test
		ColliderDesc.fRadius = 0.5f;

		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, COL_MONSTER_WEAPON, true);

		ZeroMemory(&m_WarSwordDesc, sizeof(SWORDDESC));
		m_WarSwordDesc.pBoneMatrix = m_pModelCom->Get_CombinedMatrixPtr("Bone_LE_Weapon");
		m_WarSwordDesc.OffsetMatrix = m_pModelCom->Get_OffsetMatrix("Bone_LE_Weapon");
		m_WarSwordDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix_Bones();
		m_WarSwordDesc.pTargetWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();
	}


	// pArg는 보통 위치이다. w가 1인 _float4이다.
	if (pArg)
	{
		_float4* ArgPos = (_float4*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(ArgPos));
	}
	else
		// Init test
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(85.f + rand() % 30, 0.f, 431.f + rand() % 20, 1.f));

	m_pCurState = "Legion_Mesh.ao|Legion_Idle";
	m_pNextState = "Legion_Mesh.ao|Legion_Idle";
	m_pImpactState_F = "Legion_Mesh.ao|Legion_Impact_F";
	m_pImpactState_B = "Legion_Mesh.ao|Legion_Impact_B";
	m_pModelCom->SetUp_Animation(m_pCurState);

	// 모든 몬스터는 Navigation 초기 인덱스를 잡아줘야한다
	m_pNaviCom->SetUp_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));
	
	return S_OK;
}

_int CLegion::Tick(_float fTimeDelta)
{
	{
		// 모든 몬스터는 죽으면 m_Objects 에서 제거 당해야한다
		if (m_isDead)
		{
			// 바리스타 있는 경우, 죽기전에 바리스타 상태 초기화
			if (m_pBallista)
			{
				static_cast<CBallista*>(m_pBallista)->m_bLegionOn = false;
				static_cast<CBallista*>(m_pBallista)->m_pNextState = "Ballista_A.ao|Balliista_A_Idle";
			}
			return -1;
		}

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
	}

	// FSM
	CMonster::DoGlobalState(fTimeDelta);
	UpdateState();
	DoState(fTimeDelta);

	// anim update : 로컬이동값 -> 월드이동반영
	m_pModelCom->Update_Animation(fTimeDelta, static_cast<CTransform*>(m_pTransformCom)->Get_WorldMatrix_4x4(), "_Ctrl_World", m_pNaviCom, m_eDir);

	return _int();
}

_int CLegion::LateTick(_float fTimeDelta)
{
	// 모든 몬스터는 Height, Renderer, Add_Collider
	if (CMonster::LateTick(fTimeDelta) < 0)
		return -1;

	// 체력이 0이하가 되면 죽자. 
	// 바로 죽이지말고 죽는 모션 다 끝나면 죽이자
	if (m_tGameInfo.iHp <= 0 && m_bWillDead == false)
	{
		m_bWillDead = true;
		m_pNextState = "Legion_Mesh.ao|Legion_Knockback_Start";
		//m_isDead = true;
	}

	return _int();
}

HRESULT CLegion::Render(_uint iPassIndex)
{
	// 모든 몬스터는 SetUp_ConstantTable, RenderColliders
	if (CMonster::Render(iPassIndex) < 0)
		return -1;

	// Weapon Render : ToDo 최적화

	if (m_pBallista == nullptr)
	{
		Render_Weapon(m_pModelWeaponLCom, XMConvertToRadians(-90), iPassIndex);
		Render_Weapon(m_pModelWeaponRCom, XMConvertToRadians(-90), iPassIndex);
	}

	return S_OK;
}

HRESULT CLegion::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	// HP Bar Render
	if (m_tGameInfo.iHp != m_tGameInfo.iMaxHp) // 피가 좀 달면 보여주자
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		// Bind Transform
		m_pTransformCom->Bind_OnShader(m_pVIHpBarGsBufferCom, "g_WorldMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pVIHpBarGsBufferCom, "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pVIHpBarGsBufferCom, "g_ProjMatrix");

		// Bind Position
		m_pVIHpBarGsBufferCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_vector));


		// Bind 몬스터 현재 체력비율을 UV x 좌표로 넘겨주자. ex) 0.01 ~ 0.99 이니깐 80% 면 (0.99 - 0.01) * 0.8;
#define MAX_LEN_HP (0.99f - 0.01f)
		_float fCurHpRatio = (_float)m_tGameInfo.iHp / (_float)m_tGameInfo.iMaxHp;
		_float fUVx = MAX_LEN_HP * fCurHpRatio;
		m_pVIHpBarGsBufferCom->Set_RawValue("g_fMonsterHpUVX", &fUVx, sizeof(_float));

		_float fHpBarHeight = 2.4f;
		m_pVIHpBarGsBufferCom->Set_RawValue("g_fHpBarHeight", &fHpBarHeight, sizeof(_float));

		m_pVIHpBarGsBufferCom->Render(0);
		RELEASE_INSTANCE(CGameInstance);
	}

	m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);

#ifdef _DEBUG
	// 모든 몬스터는 Collider를 render한다
	__super::Render_Colliders();
#endif

#ifdef USE_IMGUI
	if (m_bUseImGui) // IMGUI 툴로 배치할거다
	{
		CImguiManager::GetInstance()->Transform_Control(m_pTransformCom, m_CloneIdx, &m_bUseImGui);
	}
#endif

	return S_OK;
}


HRESULT CLegion::Render_Weapon(CModel* pModel, _float fRadian, _uint iPassIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Bind Directional Light
	LIGHTDESC		dirLightDesc = *pGameInstance->Get_LightDesc(0);
	DirectionalLight mDirLight;
	mDirLight.Ambient = dirLightDesc.vAmbient;
	mDirLight.Diffuse = dirLightDesc.vDiffuse;
	mDirLight.Specular = dirLightDesc.vSpecular;
	mDirLight.Direction = dirLightDesc.vDirection;
	pModel->Set_RawValue("g_DirLight", &mDirLight, sizeof(DirectionalLight));
	// Bind Material
	pModel->Set_RawValue("g_Material", &m_tMtrlDesc, sizeof(MTRLDESC));

	_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_WarSwordDesc.pBoneMatrix);
	_matrix		PivotMatrix = XMLoadFloat4x4(&m_WarSwordDesc.PivotMatrix);
	_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_WarSwordDesc.pTargetWorldMatrix);
	_matrix		TransformationMatrix = XMMatrixRotationX(fRadian) * CombinedTransformationMatrix * PivotMatrix * TargetWorldMatrix;
	_float4x4 modelWeaponWorldMat;
	XMStoreFloat4x4(&modelWeaponWorldMat, XMMatrixTranspose(TransformationMatrix));
	pModel->Set_RawValue("g_WorldMatrix", &modelWeaponWorldMat, sizeof(_float4x4));

	if (iPassIndex == 3) // shadow map
	{
		pModel->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_View())), sizeof(_float4x4));
		pModel->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_Proj())), sizeof(_float4x4));
	}
	else
	{
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, pModel, "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, pModel, "g_ProjMatrix");
	}

	// Bind Position
	_float4			vCamPosition;
	XMStoreFloat4(&vCamPosition, pGameInstance->Get_CamPosition());
	pModel->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));

	// Branch to Use Normal Mapping 
	pModel->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));
	pModel->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));

	RELEASE_INSTANCE(CGameInstance);

	_uint	iNumMaterials = pModel->Get_NumMaterials();
	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		pModel->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		pModel->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
		pModel->Render(i, iPassIndex);
	}

	return S_OK;
}


void CLegion::UpdateState()
{
	if (m_pCurState == m_pNextState)
		return;

	_bool isLoop = false;

	// --------------------------
	// --------------------------
	// --------------------------
	// m_pCurState Exit
	if (
		m_pCurState == "Legion_Mesh.ao|Legion_Atk_Flurry" ||
		m_pCurState == "Legion_Mesh.ao|Legion_Atk_Heavy" ||
		m_pCurState == "Legion_Mesh.ao|Legion_Attack_02" ||
		m_pCurState == "Legion_Mesh.ao|Legion_Atk_Slam"
		)
	{
		// 해당 상태에서 무기 콜라이더 끄자
		Set_Collider_Attribute(COL_MONSTER_WEAPON, true);

		// 공격 상태 exit시 슈아 상태 끄자.
		m_bSuperArmor = false;
	}



	// --------------------------
	// --------------------------
	// --------------------------
	// m_eNextState Enter
	if (m_pNextState == "Legion_Mesh.ao|Legion_Idle" ||
		m_pNextState == "Legion_Mesh.ao|Legion_Run_F" ||
		m_pNextState == "Legion_Mesh.ao|Legion_Ballista_Full" 

		)
	{
		isLoop = true;
		m_eDir = OBJECT_DIR::DIR_F;
	}
	else if (
		m_pNextState == "Legion_Mesh.ao|Legion_Taunt_01" ||
		m_pNextState == "Legion_Mesh.ao|Legion_Taunt_02" ||
		m_pNextState == "Legion_Mesh.ao|Legion_Ballista_Idle" 
		)
	{
		m_eDir = OBJECT_DIR::DIR_F;
		isLoop = false;
	}
	// 죽을때는 슈퍼아머상태로 둘까? TODO : 콤보 시험해보자
	else if (
		m_pNextState == "Legion_Mesh.ao|Legion_Knockback_Start" ||
		m_pNextState == "Legion_Mesh.ao|Legion_Knockback_Land" ||
		m_pNextState == "Legion_Mesh.ao|Legion_Knockback_Loop1" ||
		m_pNextState == "Legion_Mesh.ao|Legion_Knockback_Loop2"
		)
	{
		m_bSuperArmor = true;
		m_eDir = OBJECT_DIR::DIR_F;
		isLoop = false;
	}
	// Atk States
	else if (
		m_pNextState == "Legion_Mesh.ao|Legion_Atk_Flurry" ||
		m_pNextState == "Legion_Mesh.ao|Legion_Atk_Heavy" ||
		m_pNextState == "Legion_Mesh.ao|Legion_Attack_02" ||
		m_pNextState == "Legion_Mesh.ao|Legion_Atk_Slam"
		)
	{
		// 해당 상태에서 무기 콜라이더 키자
		Set_Collider_Attribute(COL_MONSTER_WEAPON, false);
		// 공격 상태 enter시 슈아 상태 켜자.
		m_bSuperArmor = true;

		m_eDir = OBJECT_DIR::DIR_F;
		isLoop = false;
	}
	else if (m_pNextState == "Legion_Mesh.ao|Legion_Evade_Left")
	{
		m_eDir = OBJECT_DIR::DIR_L;
		isLoop = false;
	}
	else if (m_pNextState == "Legion_Mesh.ao|Legion_Evade_Right")
	{
		m_eDir = OBJECT_DIR::DIR_R;
		isLoop = false;
	}
	// Impack States
	else if (m_pNextState == m_pImpactState_B)
	{
		m_eDir = OBJECT_DIR::DIR_B;
		isLoop = false;
	}
	else if (m_pNextState == m_pImpactState_F)
	{
		m_eDir = OBJECT_DIR::DIR_F;
		isLoop = false;
	}

	m_pModelCom->SetUp_Animation(m_pNextState, isLoop);
	//// 하지만 현재 상태가 피격 상태라면, 이전상태 업데이트는 하지 않는다. F->B->F->B 반복 이슈
	//if (m_pCurState != m_pImpactState_B && m_pCurState != m_pImpactState_F)
	//	m_pPreState = m_pCurState; // 피격 상태가 끝나면 이전상태(m_pPreState)로 다시 돌아간다. 
	m_pCurState = m_pNextState;
}

// FSM
void CLegion::DoState(float fTimeDelta)
{
	//-----------------------------------------------------
	if (m_pCurState == "Legion_Mesh.ao|Legion_Idle")
	{
		// 바리스타 리스트 돌면서 근처에 있는 바리스타가 있는 경우 조종하러가자
		if (m_pBallista == nullptr) // 이미 바리스타를 들고 있다면 무시~
		{
			auto pList = CObject_Manager::GetInstance()->Get_GameObject_CloneList(L"Layer_Ballista");
			if (pList && pList->empty() == false)
			{
				for (auto pGameObject : *pList)
				{
					// 하지만 해당 바리스타에 누가 이미 탑승했다면 무시 
					if (static_cast<CBallista*>(pGameObject)->m_bLegionOn)
						continue;

					// 근처에 있으면 그쪽으로 그냥 걸어가자 
					CTransform* pBallistaTransformCom = static_cast<CTransform*>(pGameObject->Get_ComponentPtr(L"Com_Transform"));
					if (pBallistaTransformCom == nullptr)
						assert(0);

					// 일정거리에 있는 발리스타를 탐색한다
					if (XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - pBallistaTransformCom->Get_State(CTransform::STATE_POSITION)))
						< 45.f)
					{
						m_pBallista = pGameObject;
						static_cast<CBallista*>(m_pBallista)->m_bLegionOn = true;
						Safe_AddRef(m_pBallista);
						break;
					}
				}
			}
		}

		if (m_pBallista) // 바리스타가 있다면 그쪽으로 일단 걸어가자 
		{
			m_pNextState = "Legion_Mesh.ao|Legion_Run_F";
		}
		else
		{
			// 플레이어가 공격 반경 내에 있다면 공격. 근데 바로 하지는 말고 좀 쉬었다하자
			_float disToTarget = Get_Target_Dis(m_pTargetTransform);
			if (m_fTimeIdle < fTimeDelta && (disToTarget < ATK_RANGE))
			{
				m_pTransformCom->LookAt(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION))));
				int randNextState = rand() % 4;
				if (randNextState == 0)	m_pNextState = "Legion_Mesh.ao|Legion_Atk_Flurry";
				else if (randNextState == 1) m_pNextState = "Legion_Mesh.ao|Legion_Atk_Heavy";
				else if (randNextState == 2) m_pNextState = "Legion_Mesh.ao|Legion_Attack_02";
				else if (randNextState == 3) m_pNextState = "Legion_Mesh.ao|Legion_Atk_Slam";

			}
			else if (disToTarget < CHASE_RANGE)
			{
				// 플레이어가 추적 반경 안에 있다면 추적 
				// 그러나 바로 추적하지 말고 1초정도 쉬었다가 추적하자
				m_fTimeIdle += fTimeDelta;

				if (m_fTimeIdle > IDLE_TIME_TO_ATK_DELAY)
				{
					m_pNextState = "Legion_Mesh.ao|Legion_Run_F";
					m_fTimeIdle = 0.f;
				}
			}
		}

	}
	//-----------------------------------------------------
	else if (
		m_pCurState == "Legion_Mesh.ao|Legion_Atk_Flurry" || 
		m_pCurState == "Legion_Mesh.ao|Legion_Atk_Heavy" ||
		m_pCurState == "Legion_Mesh.ao|Legion_Attack_02" ||
		m_pCurState == "Legion_Mesh.ao|Legion_Atk_Slam")
	{

		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{ 
			// 확률로 도발 모션 ㅋ
			int randTaunt = rand() % 5;
			if (randTaunt == 0)
				m_pNextState = "Legion_Mesh.ao|Legion_Taunt_01";
			else if (randTaunt == 1)
				m_pNextState = "Legion_Mesh.ao|Legion_Taunt_02";
			else if (randTaunt == 2)// 공격 애니메이션 끝나면 Idle로 
				m_pNextState = "Legion_Mesh.ao|Legion_Idle";
			else if (randTaunt == 3)
				m_pNextState = "Legion_Mesh.ao|Legion_Evade_Left";
			else if (randTaunt == 4)
				m_pNextState = "Legion_Mesh.ao|Legion_Evade_Right";
		}
	}
	//-----------------------------------------------------
	else if (m_pCurState == "Legion_Mesh.ao|Legion_Run_F")
	{
		if (m_pBallista) // 바리스타가 있다면 그쪽으로 추적하자. 
		{
			CTransform* pBallistaTransformCom = static_cast<CTransform*>(m_pBallista->Get_ComponentPtr(L"Com_Transform"));

			// 추적하다가 공격 반경 내에 있다면 발사 애니메이션 시작
			if (Get_Target_Dis(pBallistaTransformCom) < 2.5f)
			{
				// 발사하기전 바리스타와 Legion 상태를 초기화 해주자
				{
					// Legion 방향은 바리스타가 바라보고 있는 방향으로
					m_pTransformCom->Set_Look(pBallistaTransformCom->Get_State(CTransform::STATE_LOOK));
					// Legion 위치는 바리스타를 바라Oc본뒤
					auto toPosition = pBallistaTransformCom->Get_State(CTransform::STATE_POSITION)
						+ m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * -2.5f /*왼쪽으로 조금옮기자*/
						+ XMVectorSet(0.f, 1.f, 0.f, 0.f)/*위로 조금 옮기자*/
						+ m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -1.5f /*뒤쪽으로 조금 옮기자.*/;
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, toPosition);

					// 바리스타도 애니메이션 바꿔주자. 
					static_cast<CBallista*>(m_pBallista)->m_pNextState = "Ballista_A.ao|Ballista_A_Full";

					// 바리스타 탈때는 높이 태우지말자
					m_bHeight = false;
				}

				// Legion 애니메이션도 변경해줘야지
				m_pNextState = "Legion_Mesh.ao|Legion_Ballista_Full";
			}
			// 추적은 바리스타 방향으로 돌면서 go Staright 
			else
			{
				m_pTransformCom->TurnTo_AxisY_Degree(GetDegree_Target(pBallistaTransformCom), fTimeDelta * 10);
				m_pTransformCom->Go_Straight(fTimeDelta, m_pNaviCom);
			}
		}
		else
		{
			// 추적하다가 공격 반경 내에 있다면 공격  
			if (Get_Target_Dis(m_pTargetTransform) < ATK_RANGE)
			{
				m_pTransformCom->LookAt(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION))));
				int randAtk = rand() % 4;
				if (randAtk == 0) m_pNextState = "Legion_Mesh.ao|Legion_Atk_Flurry";
				else if (randAtk == 1) m_pNextState = "Legion_Mesh.ao|Legion_Atk_Heavy";
				else if (randAtk == 2) m_pNextState = "Legion_Mesh.ao|Legion_Attack_02";
				else if (randAtk == 3) m_pNextState = "Legion_Mesh.ao|Legion_Atk_Slam";
			}
			// 추적은 War 방향으로 돌면서 go Straight
			else
			{
				m_pTransformCom->TurnTo_AxisY_Degree(GetDegree_Target(m_pTargetTransform), fTimeDelta * 10);
				m_pTransformCom->Go_Straight(fTimeDelta, m_pNaviCom);
			}
		}
	}
	//-----------------------------------------------------
	else if (m_pCurState == "Legion_Mesh.ao|Legion_Taunt_01" || m_pCurState == "Legion_Mesh.ao|Legion_Taunt_02")
	{
		// 도발 애니메이션 끝나면 
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			// 확률로 와가리 털자 ㅎ 
			m_pNextState = "Legion_Mesh.ao|Legion_Idle";
		}
	}
	//-----------------------------------------------------
	else if (m_pCurState == "Legion_Mesh.ao|Legion_Evade_Right" || m_pCurState == "Legion_Mesh.ao|Legion_Evade_Left")
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			_float disToTarget = Get_Target_Dis(m_pTargetTransform);
			if (disToTarget < ATK_RANGE)
			{
				m_pTransformCom->LookAt(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION))));
				int randNextState = rand() % 4;
				if (randNextState == 0)	m_pNextState = "Legion_Mesh.ao|Legion_Atk_Flurry";
				else if (randNextState == 1) m_pNextState = "Legion_Mesh.ao|Legion_Atk_Heavy";
				else if (randNextState == 2) m_pNextState = "Legion_Mesh.ao|Legion_Attack_02";
				else if (randNextState == 3) m_pNextState = "Legion_Mesh.ao|Legion_Atk_Slam";
			}
			else if (disToTarget < CHASE_RANGE)
			{
				m_pNextState = "Legion_Mesh.ao|Legion_Run_F";
			}
		}
	}
	//-----------------------------------------------------
	else if (m_pCurState == "Legion_Mesh.ao|Legion_Knockback_Start")
	{
		m_bHeight = false;
		// 죽기직전에 날가가도록하자 0.15f 높이만큼의 땅에 닿으면() 그 다음 상태로 천이한다
		if (m_pTransformCom->MomentumWithGravity(XMLoadFloat4(&m_vFloatingDir), m_fFloatingPwr, fTimeDelta, 0.15f) == false)
		{
			m_bHeight = true;
			m_pNextState = "Legion_Mesh.ao|Legion_Knockback_Land";
		}
		else
		{
			if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
			{
				if (rand() % 2)
					m_pNextState = "Legion_Mesh.ao|Legion_Knockback_Loop1";
				else
					m_pNextState = "Legion_Mesh.ao|Legion_Knockback_Loop2";
			}
		}
	}
	else if (m_pCurState == "Legion_Mesh.ao|Legion_Knockback_Loop1")
	{
		// 죽기직전에 날가가도록하자 0.15f 높이만큼의 땅에 닿으면() 그 다음 상태로 천이한다
		if (m_pTransformCom->MomentumWithGravity(XMLoadFloat4(&m_vFloatingDir), m_fFloatingPwr, fTimeDelta, 0.15f) == false)
		{
			m_bHeight = true;
			m_pNextState = "Legion_Mesh.ao|Legion_Knockback_Land";
		}
	}
	else if (m_pCurState == "Legion_Mesh.ao|Legion_Knockback_Loop2")
	{
		// 죽기직전에 날가가도록하자 0.15f 높이만큼의 땅에 닿으면() 그 다음 상태로 천이한다
		if (m_pTransformCom->MomentumWithGravity(XMLoadFloat4(&m_vFloatingDir), m_fFloatingPwr, fTimeDelta, 0.15f) == false)
		{
			m_bHeight = true;
			m_pNextState = "Legion_Mesh.ao|Legion_Knockback_Land";
		}
	}
	else if (m_pCurState == "Legion_Mesh.ao|Legion_Knockback_Land")
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			// Todo : 바로 죽이지 말고 디졸브 끝나면 죽이자 
			m_isDead = true;
		}
	}
	// -----------------------------------------------------------------
	else if (m_pCurState == "Legion_Mesh.ao|Legion_Ballista_Full")
	{
		// 플레이어를 계속해서 추적하면서 위치를 변경한다. 돌리기 모션전까지 추적하다가 그 다음은 위치 변경안한다
		_uint iKeyFrameIdx = m_pModelCom->Get_Current_KeyFrame_Index(m_pCurState);
		if (34 < iKeyFrameIdx && iKeyFrameIdx < 135) // 방향돌리는 애니메이션 
		{
			// 먼저 바리스타 방향을 바꿔주자.
			CTransform* pBallistaTransformCom = static_cast<CTransform*>(m_pBallista->Get_ComponentPtr(L"Com_Transform")); 
			pBallistaTransformCom->TurnTo_AxisY_Degree(GetDegree_Target_Ballista(m_pTargetTransform, pBallistaTransformCom), fTimeDelta*5.f);

			// Legion 방향은 바리스타가 바라보고 있는 방향으로
			m_pTransformCom->Set_Look(pBallistaTransformCom->Get_State(CTransform::STATE_LOOK));
			// Legion 위치는 바리스타를 바라본뒤
			auto toPosition = pBallistaTransformCom->Get_State(CTransform::STATE_POSITION)
				+ m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * -2.5f /*왼쪽으로 조금옮기자*/
				+ XMVectorSet(0.f, 1.f, 0.f, 0.f)/*위로 조금 옮기자*/
				+ m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -1.5f /*뒤쪽으로 조금 옮기자.*/;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, toPosition);
		}
	}
	//-------------------------------------------------------
	// 피격 모션
	else if (m_pCurState == m_pImpactState_F || m_pCurState == m_pImpactState_B)
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			//m_pNextState = m_pPreState; // 피격애니메이션 끝나면 이전상태로 돌려놓자.
			m_pNextState = "Legion_Mesh.ao|Legion_Idle"; // 아.. 그냥 Idle로 가자..
		}
	}
}

_float CLegion::Get_Target_Dis(class CTransform* pTargetTransform)
{
	// 타겟간의 거리를 구한다
	return XMVectorGetX(XMVector3Length(
		pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION) -
		m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION)));
}

// targetPos과 내 위치간의 각도를 구하자.
_float CLegion::GetDegree_Target(class CTransform* pTargetTransform)
{
	_vector targetPos = pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION);
	_vector myPos = m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
	_vector toTarget = XMVector4Normalize(targetPos - myPos);

	XMVECTOR curVecAngleVec = XMVector3AngleBetweenVectors(toTarget,XMVectorSet(0.f, 0.f, 1.f, 0.f)) 
		* (XMVectorGetX(toTarget) < 0.f ? -1.f : 1.f);
	return XMConvertToDegrees(XMVectorGetX(curVecAngleVec));
}

// Legion 이 가지고 있는 바리스타 중점<-->타겟 중점간의 각도를 구한다. 기준축은 -z축이다
_float CLegion::GetDegree_Target_Ballista(class CTransform* pTargetTransform, class CTransform* pBallistaTransformCom)
{
	_vector targetPos = pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION);
	_vector myPos = pBallistaTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
	_vector toTarget = XMVector4Normalize(targetPos - myPos);

	XMVECTOR curVecAngleVec = XMVector3AngleBetweenVectors(toTarget, XMVectorSet(0.f, 0.f, -1.f, 0.f))
		* (XMVectorGetX(toTarget) < 0.f ? 1.f : -1.f);
	return XMConvertToDegrees(XMVectorGetX(curVecAngleVec));
}

_int CLegion::Update_Colliders(_matrix wolrdMatrix/*not used*/)
{
	for (auto pCollider : m_ColliderList)
	{
		if (pCollider->Get_ColliderTag() == COL_MONSTER_WEAPON)
		{
			_matrix		OffsetMatrix = XMLoadFloat4x4(&m_WarSwordDesc.OffsetMatrix); // 뼈->정점
			_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_WarSwordDesc.pBoneMatrix); // Root->뼈 
			_matrix		PivotMatrix = XMLoadFloat4x4(&m_WarSwordDesc.PivotMatrix);
			_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_WarSwordDesc.pTargetWorldMatrix); // just legion's world matrix
			_matrix		TransformationMatrix =
				(CombinedTransformationMatrix * PivotMatrix) * //OffsetMatrix를 안곱하니간 뭔가 되는거 같다... 왜?
				TargetWorldMatrix;
			pCollider->Update(TransformationMatrix);
		}
		else
		{
			pCollider->Update(m_pTransformCom->Get_WorldMatrix());
		}
	}

	return 0;
}

CLegion * CLegion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLegion*		pInstance = new CLegion(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CLegion");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CLegion::Clone(void* pArg)
{
	CLegion*		pInstance = new CLegion(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CLegion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLegion::Free()
{
	Safe_Release(m_pVIHpBarGsBufferCom);
	Safe_Release(m_pBallista);
	Safe_Release(m_pModelWeaponLCom);
	Safe_Release(m_pModelWeaponRCom);
	CMonster::Free();
}
