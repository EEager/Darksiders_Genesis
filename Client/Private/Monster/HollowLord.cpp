#include "stdafx.h"
#include "..\public\Monster\HollowLord.h"
#include "GameInstance.h"

// 아래 3개정도만 수정하면되네 
// 1. 모델Com
// 2. master bone name
// 3. collider 

CHollowLord::CHollowLord(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{
}

CHollowLord::CHollowLord(const CHollowLord& rhs)
	: CMonster(rhs)
{
}

HRESULT CHollowLord::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CHollowLord::NativeConstruct(void* pArg)
{
	// GameInfo Init
	m_tGameInfo.iAtt = 2;
	m_tGameInfo.iEnergy = rand() % 10 + 10;
	m_tGameInfo.iMaxHp = 140;
	m_tGameInfo.iHp = m_tGameInfo.iMaxHp;
	m_tGameInfo.iSoul = rand() % 10 + 10;

	// 모든 몬스터는 m_pTransformCom, m_pRendererCom, m_pNaviCom를 가진다
	if (CMonster::NativeConstruct(pArg))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HollowLord"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Collider */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ZeroMemory(&m_Lord_RightHandDesc, sizeof(BONEDESC));
	m_Lord_RightHandDesc.pBoneMatrix = m_pModelCom->Get_CombinedMatrixPtr("Bone_HL_Hand_R");
	m_Lord_RightHandDesc.OffsetMatrix = m_pModelCom->Get_OffsetMatrix("Bone_HL_Hand_R");
	m_Lord_RightHandDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix_Bones();
	m_Lord_RightHandDesc.pTargetWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();
	ZeroMemory(&m_Lord_LeftHandDesc, sizeof(BONEDESC));
	m_Lord_LeftHandDesc.pBoneMatrix = m_pModelCom->Get_CombinedMatrixPtr("Bone_HL_Hand_L");
	m_Lord_LeftHandDesc.OffsetMatrix = m_pModelCom->Get_OffsetMatrix("Bone_HL_Hand_L");
	m_Lord_LeftHandDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix_Bones();
	m_Lord_LeftHandDesc.pTargetWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();
	ZeroMemory(&m_Lord_HeadDesc, sizeof(BONEDESC));
	m_Lord_HeadDesc.pBoneMatrix = m_pModelCom->Get_CombinedMatrixPtr("Bone_HL_Face_ForeHead");
	m_Lord_HeadDesc.OffsetMatrix = m_pModelCom->Get_OffsetMatrix("Bone_HL_Face_ForeHead");
	m_Lord_HeadDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix_Bones();
	m_Lord_HeadDesc.pTargetWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();

	// For Hit Box
	{
		// 보스 히트박스는 COL_BALLISTA_BODY로 만들어 플레이어가 공격하였을때 go back 하도록하자
		
		// 0
		/* For.LordBody */
		ColliderDesc.vPivot = _float3(0.f, 15.f, 0.f);
		ColliderDesc.vSize = _float3(20.f, 30.f, 20.f);
		ColliderDesc.eColType = CCollider::COL_TYPE::COL_TYPE_AABB;
		__super::Add_Collider(&ColliderDesc, COL_BALLISTA_BODY);

		// 1
		/* For.LordHead */
		ColliderDesc.vPivot = _float3(0.f, 0.0f, 0.f);
		ColliderDesc.fRadius = 5.f;
		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, COL_BALLISTA_BODY);

		// 2
		/* For.LordRightHand */
		ColliderDesc.vPivot = _float3(0.f, 0.0f, 0.f);
		ColliderDesc.fRadius = 5.f;
		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, COL_BALLISTA_BODY);

		// 3
		/* For.LordLeftHand */
		ColliderDesc.vPivot = _float3(0.f, 0.0f, 0.f);
		ColliderDesc.fRadius = 5.f;
		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, COL_BALLISTA_BODY);
	}

	// For Weapon
	{
		// 4
		/* For.LordRightHand */
		ColliderDesc.vPivot = _float3(0.f, 0.0f, 0.f); 
		ColliderDesc.fRadius = 5.f;
		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, COL_MONSTER_WEAPON, true);

		// 5
		/* For.LordLeftHand */
		ColliderDesc.vPivot = _float3(0.f, 0.0f, 0.f);
		ColliderDesc.fRadius = 5.f;
		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, COL_MONSTER_WEAPON, true);
	}

	// Init test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(614.f, 11.f, 142.f, 1.f));

	// Init Anim State
	m_pCurState = "HollowLord.ao|HollowLord_Emerge";
	m_pNextState = "HollowLord.ao|HollowLord_Emerge";
	m_pModelCom->SetUp_Animation("HollowLord.ao|HollowLord_Emerge", false, false);

	// 모든 몬스터는 Navigation 초기 인덱스를 잡아줘야한다
	m_pNaviCom->SetUp_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));

	// 높이 태우지말자
	m_bHeight = false;

	m_pTransformCom->Set_Scale(_float3(0.5f * 1.5f, 0.5f * 1.5f, 0.5f * 1.5f));

	// Hp Bar를 가지게하자.
	m_pHpBar = CUI_HollowLord_HpBar::Create(m_pDevice, m_pDeviceContext);
	m_pHpBar->m_pHollowLord = this; 
	Safe_AddRef(m_pHpBar->m_pHollowLord); // 이건 나중에 hpBar에서 해제해주자

	// BGM은 여기서 바꿉니다. ^_^
	SoundManager::Get_Instance()->StopSound(SoundManager::BGM);
	SoundManager::Get_Instance()->ForcePlayBGM(L"mus_level01_hollowlord.ogg");

	// 처음엔 Emerge이다.
	SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_emerge_01.ogg", SoundManager::BREAKABLE5, 0.8f);

	return S_OK;
}

void Add_SlamImpact(_vector vPos)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	// 링팡
	if (pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_MeshEffect", TEXT("Prototype_GameObject_MeshEffect_Ring"), &vPos))
		assert(0);
	// 폭발.
	if (pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Explosion", TEXT("Prototype_GameObject_Explosion"), &vPos))
		assert(0);
	// 공기팡
	if (pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_MeshEffect", TEXT("Prototype_GameObject_MeshEffect_Sphere"), &vPos))
		assert(0);

	// 사운드
	auto randSound = rand() % 4;
	if (randSound == 0)
		SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_atk_slam_explosion_01.ogg", SoundManager::BREAKABLE1, WAR_ATK_VOLUME);
	else if (randSound == 1)
		SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_atk_slam_explosion_02.ogg", SoundManager::BREAKABLE2, WAR_ATK_VOLUME);
	else if (randSound == 2)
		SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_atk_slam_explosion_03.ogg", SoundManager::BREAKABLE3, WAR_ATK_VOLUME);
	else if (randSound == 3)
		SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_atk_slam_explosion_04.ogg", SoundManager::BREAKABLE4, WAR_ATK_VOLUME);
	RELEASE_INSTANCE(CGameInstance);
}

_int CHollowLord::Tick(_float fTimeDelta)
{
	// 모든 몬스터는 Collider list 를 update해야한다
	if (CMonster::Tick(fTimeDelta) < 0)
		return -1;

	if (m_isDead) // CMonster::Tick에서 -1을 리턴해줄것이다.
		return 0; 

	// FSM
	CMonster::DoGlobalState(fTimeDelta); // 히트파워 감소
	UpdateState();
	// 로컬위치변화를 월행에 적용시키자
	m_pModelCom->Update_Animation(fTimeDelta, static_cast<CTransform*>(m_pTransformCom)->Get_WorldMatrix_4x4(), "Bone_HL_Root", m_pNaviCom, m_eDir, 0);
	DoState(fTimeDelta);

	// Hp바 Tick
	m_pHpBar->Tick(fTimeDelta);

	// Impact를 채워넣었으면 확인하고 하나씩 꺼내서 실행하자.

	// 먼저 오른손에 있는 폭발물 체크
	if (m_qSlam_R.empty() == false)
	{
		if (m_bSlamed_R_First == false) 
		{
			// 처음것은 바로 꺼내자.
			Add_SlamImpact(m_qSlam_R.front());
			m_qSlam_R.pop();
			m_bSlamed_R_First = true;
		}
		else
		{
			if (m_fLastSlamTimeAcc_R > .5f)
			{
				Add_SlamImpact(m_qSlam_R.front());
				m_qSlam_R.pop();
				m_fLastSlamTimeAcc_R = 0.f;
			}
			m_fLastSlamTimeAcc_R += fTimeDelta;
		}
	}

	// 왼손에 있는 폭발물을 체크한다.
		// 먼저 오른손에 있는 폭발물 체크
	if (m_qSlam_L.empty() == false)
	{
		if (m_bSlamed_L_First == false)
		{
			// 처음것은 바로 꺼내자.
			Add_SlamImpact(m_qSlam_L.front());
			m_qSlam_L.pop();
			m_bSlamed_L_First = true;
		}
		else
		{
			if (m_fLastSlamTimeAcc_L > .5f)
			{
				Add_SlamImpact(m_qSlam_L.front());
				m_qSlam_L.pop();
				m_fLastSlamTimeAcc_L = 0.f;
			}
			m_fLastSlamTimeAcc_L += fTimeDelta;
		}
	}

	return _int();
}

_int CHollowLord::LateTick(_float fTimeDelta)
{
	// 모든 몬스터는 Height, Renderer, Add_Collider
	if (CMonster::LateTick(fTimeDelta) < 0)
		return -1;

	// 체력이 0이하가 되면 죽는 모션을 진행하자.
	if (m_tGameInfo.iHp <= 0 && m_bWillDead == false)
	{
		m_bWillDead = true;
		m_pNextState = "HollowLord.ao|HollowLord_Death";
	}
	
	// Hp바 LateTick
	m_pHpBar->LateTick(fTimeDelta);

	return _int();
}

HRESULT CHollowLord::Render(_uint iPassIndex)
{
	// 모든 몬스터는 SetUp_ConstantTable
	if (CMonster::Render(iPassIndex) < 0)
		return -1;

	return S_OK;
}


HRESULT CHollowLord::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	// collider Render And ImGUI Render
	CMonster::PostRender(m_spriteBatch, m_spriteFont);

	return S_OK;
}


void CHollowLord::UpdateState()
{
	if (m_pCurState == m_pNextState)
		return;

	_bool isLoop = false;

	// m_eCurState Exit
	if (
		m_pCurState == "HollowLord.ao|HollowLord_Atk_Barrage" ||
		m_pCurState == "HollowLord.ao|HollowLord_Atk_Swipe_L" ||
		m_pCurState == "HollowLord.ao|HollowLord_Atk_DoubleSlam" ||
		m_pCurState == "HollowLord.ao|HollowLord_Atk_Swipe_R"
		)
	{
		// 해당 상태에서 무기 콜라이더 끄자
		Set_Collider_Attribute(COL_MONSTER_WEAPON, true);
		m_bSlamed_L = false;
		m_bSlamed_R = false;
	}
	else if (m_pCurState == "HollowLord.ao|HollowLord_Atk_Slam_L")
	{
		// 해당 상태에서 무기 콜라이더 끄자
		Set_Collider_Attribute(COL_MONSTER_WEAPON, true);
		m_bSlamed_L = false;

		SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_atk_slam_L_01.ogg", SoundManager::HOLLOLORD, HOLLOWLORD_VOLUME);
		SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_atk_slam_L_vo_01.ogg", SoundManager::HOLLOLORD_VO, HOLLOWLORD_VOLUME);
	}
	else if (m_pCurState == "HollowLord.ao|HollowLord_Atk_Slam_R")
	{
		// 해당 상태에서 무기 콜라이더 끄자
		Set_Collider_Attribute(COL_MONSTER_WEAPON, true);
		m_bSlamed_R = false;

		SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_atk_slam_R_01.ogg", SoundManager::HOLLOLORD, HOLLOWLORD_VOLUME);
		SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_atk_slam_R_vo_01.ogg", SoundManager::HOLLOLORD_VO, HOLLOWLORD_VOLUME);
	}

	// m_eNextState Enter
	if (m_pNextState == "HollowLord.ao|HollowLord_Idle" ||
		m_pNextState == "HollowLord.ao|Legion_Run_F"
		)
	{
		isLoop = true;
	}
	else if (
		m_pNextState == "HollowLord.ao|HollowLord_Atk_Barrage" ||
		m_pNextState == "HollowLord.ao|HollowLord_Atk_DoubleSlam" ||
		m_pNextState == "HollowLord.ao|HollowLord_Atk_Swipe_L" ||
		m_pNextState == "HollowLord.ao|HollowLord_Atk_Swipe_R"
		)
	{
		// 해당 상태에서 무기 콜라이더 키자
		Set_Collider_Attribute(COL_MONSTER_WEAPON, false);
		isLoop = false;
		m_bSlamed_R_First = false; 
		m_bSlamed_L_First = false; 

		// 사운드
		if (m_pNextState == "HollowLord.ao|HollowLord_Atk_Barrage")
		{
			SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_atk_barrage_01.ogg", SoundManager::HOLLOLORD, HOLLOWLORD_VOLUME);
			SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_atk_barrage_vo_01.ogg", SoundManager::HOLLOLORD_VO, HOLLOWLORD_VOLUME);
		}
		else if (m_pNextState == "HollowLord.ao|HollowLord_Atk_DoubleSlam")
		{
			SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_atk_doubleslam_01.ogg", SoundManager::HOLLOLORD, HOLLOWLORD_VOLUME);
			SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_atk_doubleslam_vo_01.ogg", SoundManager::HOLLOLORD_VO, HOLLOWLORD_VOLUME);
		}
		else if (m_pNextState == "HollowLord.ao|HollowLord_Atk_Swipe_L")
		{
			SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_atk_swipe_L_01.ogg", SoundManager::HOLLOLORD, HOLLOWLORD_VOLUME);
			SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_atk_swipe_L_vo_01.ogg", SoundManager::HOLLOLORD_VO, HOLLOWLORD_VOLUME);
		}
		else if (m_pNextState == "HollowLord.ao|HollowLord_Atk_Swipe_R")
		{
			SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_atk_swipe_R_01.ogg", SoundManager::HOLLOLORD, HOLLOWLORD_VOLUME);
			SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_atk_swipe_R_vo_01.ogg", SoundManager::HOLLOLORD_VO, HOLLOWLORD_VOLUME);
		}
		else if (m_pNextState == "HollowLord.ao|HollowLord_Atk_Swipe_R")
		{
			SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_atk_swipe_R_01.ogg", SoundManager::HOLLOLORD, HOLLOWLORD_VOLUME);
			SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_atk_swipe_R_vo_01.ogg", SoundManager::HOLLOLORD_VO, HOLLOWLORD_VOLUME);
		}
	}
	else if (m_pNextState == "HollowLord.ao|HollowLord_Atk_Slam_L")
	{
		// 해당 상태에서 무기 콜라이더 키자
		Set_Collider_Attribute(COL_MONSTER_WEAPON, false);
		isLoop = false;
		m_bSlamed_L_First = false;
	}
	else if (m_pNextState == "HollowLord.ao|HollowLord_Atk_Slam_R")
	{
		// 해당 상태에서 무기 콜라이더 키자
		Set_Collider_Attribute(COL_MONSTER_WEAPON, false);
		isLoop = false;
		m_bSlamed_R_First = false;
	}
	else if (m_pNextState == "HollowLord.ao|HollowLord_Emerge" || 
			 m_pNextState == "HollowLord.ao|HollowLord_Death" ||
			 m_pNextState == "HollowLord.ao|HollowLord_Impact_F")
	{
		isLoop = false;

		// 사운드
		if (m_pNextState == "HollowLord.ao|HollowLord_Death")
		{
			// BGM은 일단 좀 다 끄자.
			SoundManager::Get_Instance()->StopSound(SoundManager::BGM);

			SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_death_01.ogg", SoundManager::HOLLOLORD, HOLLOWLORD_VOLUME);
			SoundManager::Get_Instance()->ForcePlay(L"general_death_boss.ogg", SoundManager::BREAKABLE4, 0.68f);
		}
		else if (m_pNextState == "HollowLord.ao|HollowLord_Impact_F")
		{
			SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_impact_01.ogg", SoundManager::HOLLOLORD, HOLLOWLORD_VOLUME);
			SoundManager::Get_Instance()->ForcePlay(L"en_hollowlord_impact_vo_01.ogg", SoundManager::HOLLOLORD_VO, HOLLOWLORD_VOLUME);
		}
	}

	m_pModelCom->SetUp_Animation(m_pNextState, isLoop);
	m_pCurState = m_pNextState;
}


int randNextState;
void CHollowLord::DoState(float fTimeDelta)
{
	//-----------------------------------------------------
	if (m_pCurState == "HollowLord.ao|HollowLord_Idle")
	{
		m_fTimeIdle += fTimeDelta;
		// idle 상태에서 잠시 쉬었다가 공격하자.
		if (m_fTimeIdle > IDLE_TIME_TO_ATK_DELAY)
		{
			if (randNextState == 0)	m_pNextState = "HollowLord.ao|HollowLord_Atk_Barrage";
			else if (randNextState == 1) m_pNextState = "HollowLord.ao|HollowLord_Atk_DoubleSlam";
			else if (randNextState == 2)	m_pNextState = "HollowLord.ao|HollowLord_Atk_Slam_L";
			else if (randNextState == 3) m_pNextState = "HollowLord.ao|HollowLord_Atk_Slam_R";
			else if (randNextState == 4) m_pNextState = "HollowLord.ao|HollowLord_Atk_Swipe_L";
			else if (randNextState == 5) m_pNextState = "HollowLord.ao|HollowLord_Atk_Swipe_R";
			randNextState = (randNextState + 1) % 6; // 순차적으로 하나씩 수행하자.
		}
	}

	// =========================================================================
	// 공격 패턴 시작
	// "HollowLord.ao|HollowLord_Atk_Barrage"
	else if (
		m_pCurState == "HollowLord.ao|HollowLord_Atk_Barrage")
	{
		// 몇초동안 랜덤하게 고블린과 Legion 을 생성하자.
		auto curKeyFrameIdx = m_pModelCom->Get_Current_KeyFrame_Index(m_pCurState);
		if (70 <= curKeyFrameIdx && curKeyFrameIdx <= 202)
		{
			m_fDoubleSlamTimeAcc += fTimeDelta;
			if (m_fDoubleSlamTimeAcc > .6f) 
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Legion", TEXT("Prototype_GameObject_Legion"), &_float4(MathHelper::RandF(553.f, 590.f), 9.5f, MathHelper::RandF(43.f, 80.f), 1.f));
				pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Goblin", TEXT("Prototype_GameObject_Goblin_Armor"), &_float4(MathHelper::RandF(553.f, 595.f), 9.5f, MathHelper::RandF(43.f, 80.f), 1.f));
				RELEASE_INSTANCE(CGameInstance);
				m_fDoubleSlamTimeAcc = 0.f;
			}
		}

		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_pNextState = "HollowLord.ao|HollowLord_Idle";
		}
	}
	// "HollowLord.ao|HollowLord_Atk_DoubleSlam"
	else if (
		m_pCurState == "HollowLord.ao|HollowLord_Atk_DoubleSlam")
	{
		if (m_bSlamed_R == false && m_pModelCom->Get_Current_KeyFrame_Index(m_pCurState) == 80)
		{
			// 더블슬램시 소리 웅장하게
			SoundManager::Get_Instance()->ForcePlay(L"en_scarab_shockwave.ogg", SoundManager::HOLLOLORD_ATK, HOLLOWLORD_VOLUME);
			Slam_R(fTimeDelta);
			Slam_L(fTimeDelta);
			m_bSlamed_R = true;
		}

		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_pNextState = "HollowLord.ao|HollowLord_Idle";
		}
	}
	//-----------------------------------------------------
	// "HollowLord.ao|HollowLord_Atk_Slam_L"
	else if (
		m_pCurState == "HollowLord.ao|HollowLord_Atk_Slam_L")
	{
		if (m_bSlamed_L == false && m_pModelCom->Get_Current_KeyFrame_Index(m_pCurState) == 62)
		{
			Slam_L(fTimeDelta);
			m_bSlamed_L = true;
		}

		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_pNextState = "HollowLord.ao|HollowLord_Idle";
		}
	}
	//-----------------------------------------------------
	// "HollowLord.ao|HollowLord_Atk_Slam_R"
	else if (
		m_pCurState == "HollowLord.ao|HollowLord_Atk_Slam_R")
	{
		if (m_bSlamed_R == false && m_pModelCom->Get_Current_KeyFrame_Index(m_pCurState) == 62) 
		{
			Slam_R(fTimeDelta);
			m_bSlamed_R = true;
		}

		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_pNextState = "HollowLord.ao|HollowLord_Idle";
		}
	}
	// 공격패턴 종료
	// =========================================================================
	else if (m_pCurState == "HollowLord.ao|HollowLord_Emerge" || 
		m_pCurState == "HollowLord.ao|HollowLord_Atk_Swipe_L" ||
		m_pCurState == "HollowLord.ao|HollowLord_Atk_Swipe_R")
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_pNextState = "HollowLord.ao|HollowLord_Idle";
		}
	}
	//-----------------------------------------------------
	else if (m_pCurState == "HollowLord.ao|HollowLord_Death")
	{
		m_bWillDead = true;
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_isDead = true;
		}
	}
	//-----------------------------------------------------
	else if (m_pCurState == "HollowLord.ao|HollowLord_Impact_F")
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			// 그로기 끝나면 Idle로.
			m_pNextState = "HollowLord.ao|HollowLord_Idle";
		}
	}
}


_int CHollowLord::Update_Colliders(_matrix wolrdMatrix/*not used*/)
{
	int idx = 0;
	for (auto& pCollider : m_ColliderList)
	{
		if (idx == 2 || idx ==4) // m_Lord_RightHandDesc
		{
			_matrix		OffsetMatrix = XMLoadFloat4x4(&m_Lord_RightHandDesc.OffsetMatrix); // 뼈->정점
			_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_Lord_RightHandDesc.pBoneMatrix); // Root->뼈 
			_matrix		PivotMatrix = XMLoadFloat4x4(&m_Lord_RightHandDesc.PivotMatrix);
			_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_Lord_RightHandDesc.pTargetWorldMatrix);
			_matrix		TransformationMatrix =
				(CombinedTransformationMatrix * PivotMatrix) * //OffsetMatrix를 안곱하니간 뭔가 되는거 같다... 왜?
				TargetWorldMatrix;
			pCollider->Update(TransformationMatrix);
		}
		else if (idx == 3 || idx == 5) // m_Lord_LeftHandDesc
		{
			_matrix		OffsetMatrix = XMLoadFloat4x4(&m_Lord_LeftHandDesc.OffsetMatrix); // 뼈->정점
			_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_Lord_LeftHandDesc.pBoneMatrix); // Root->뼈 
			_matrix		PivotMatrix = XMLoadFloat4x4(&m_Lord_LeftHandDesc.PivotMatrix);
			_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_Lord_LeftHandDesc.pTargetWorldMatrix);
			_matrix		TransformationMatrix =
				(CombinedTransformationMatrix * PivotMatrix) * //OffsetMatrix를 안곱하니간 뭔가 되는거 같다... 왜?
				TargetWorldMatrix;
			pCollider->Update(TransformationMatrix);
		}
		else if (idx == 1) // m_Lord_HeadDesc 
		{
			_matrix		OffsetMatrix = XMLoadFloat4x4(&m_Lord_HeadDesc.OffsetMatrix); // 뼈->정점
			_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_Lord_HeadDesc.pBoneMatrix); // Root->뼈 
			_matrix		PivotMatrix = XMLoadFloat4x4(&m_Lord_HeadDesc.PivotMatrix);
			_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_Lord_HeadDesc.pTargetWorldMatrix);
			_matrix		TransformationMatrix =
				(CombinedTransformationMatrix * PivotMatrix) * //OffsetMatrix를 안곱하니간 뭔가 되는거 같다... 왜?
				TargetWorldMatrix;
			pCollider->Update(TransformationMatrix);
		}
		else // Body
		{
			pCollider->Update(m_pTransformCom->Get_WorldMatrix());
		}
		idx++;
	}

	return 0;
}
#include "ParticleSystem\ParticleSystem_Manager.h"
void CHollowLord::OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
	// 몬스터 몸통과 플레이어 검이 충돌한 경우. 
	if (m_bHitted == false && pSrc->Get_ColliderTag() == COL_BALLISTA_BODY &&
		pDst->Get_ColliderTag() == COL_WAR_WEAPON)
	{
		// 피격 당했다. 
		m_bHitted = true;
		m_fHitPower = .7f;

		CParticleSystem_Manager::GetInstance()->Add_Particle_To_Layer(L"Particle_Blood");

		auto DstAtkDmg = pDst->Get_Owner()->m_tGameInfo.iAtt;
		m_tGameInfo.iHp -= DstAtkDmg;
		m_fHitDmgAcc += DstAtkDmg;

		// 받은 피해량이 20이 넘어가면 그로기 상태로 만든다. 죽을라 할때는 그로기로 안만든다
		if (m_bWillDead == false && m_fHitDmgAcc >= 20.f)
		{
			m_pNextState = "HollowLord.ao|HollowLord_Impact_F";
			m_fHitDmgAcc = 0.f;
		}
		return;
	}
}

void CHollowLord::UI_Init()
{
	m_pHpBar->m_bInit = true;
}

// 현재 오른손 위치를 채워넣는다.
void CHollowLord::Slam_R(_float fTimeDelta)
{
	// vector에 vector를 채워넣는다.(웃음ㅎ) Tick에서 해당 위치 꺼내서 Ring + Explosion + Sphere를 실행할 것이다.
	_matrix		OffsetMatrix = XMLoadFloat4x4(&m_Lord_RightHandDesc.OffsetMatrix); // 뼈->정점
	_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_Lord_RightHandDesc.pBoneMatrix); // Root->뼈 
	_matrix		PivotMatrix = XMLoadFloat4x4(&m_Lord_RightHandDesc.PivotMatrix);
	_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_Lord_RightHandDesc.pTargetWorldMatrix);
	_matrix		TransformationMatrix =
		(CombinedTransformationMatrix * PivotMatrix) * //OffsetMatrix를 안곱하니간 뭔가 되는거 같다... 왜?
		TargetWorldMatrix;

	// 발사할때 손 위치
	auto CurHandPos = XMVector3TransformCoord(XMVectorSet(0.f, 0.f, 0.f, 1.f), TransformationMatrix);
	CurHandPos = XMVectorSetY(CurHandPos, 9.5f);

	// 가야할 방향
	_vector DirToWar = XMVector3Normalize(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - CurHandPos);
	DirToWar = XMVectorSetY(DirToWar, 0.f);

	// 1) 처음 것은 War 방향으로 조금 가도록 하자.
	auto firstPos = CurHandPos + DirToWar * 3.f;
	m_qSlam_R.push(firstPos);

	// 2) 2~4번째까지는  War 방향으로 조금식 앞으로 보내면됩니다. 
	for (int i = 1; i < 5; i++/*_*/)
	{
		auto nextPos = CurHandPos + DirToWar * 8.5f * i;
		m_qSlam_R.push(nextPos);
	}
}
void CHollowLord::Slam_L(_float fTimeDelta) 
{
	// vector에 vector를 채워넣는다.(웃음ㅎ) Tick에서 해당 위치 꺼내서 Ring + Explosion + Sphere를 실행할 것이다.
	_matrix		OffsetMatrix = XMLoadFloat4x4(&m_Lord_LeftHandDesc.OffsetMatrix); // 뼈->정점
	_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_Lord_LeftHandDesc.pBoneMatrix); // Root->뼈 
	_matrix		PivotMatrix = XMLoadFloat4x4(&m_Lord_LeftHandDesc.PivotMatrix);
	_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_Lord_LeftHandDesc.pTargetWorldMatrix);
	_matrix		TransformationMatrix =
		(CombinedTransformationMatrix * PivotMatrix) * //OffsetMatrix를 안곱하니간 뭔가 되는거 같다... 왜?
		TargetWorldMatrix;

	// 발사할때 손 위치
	auto CurHandPos = XMVector3TransformCoord(XMVectorSet(0.f, 0.f, 0.f, 1.f), TransformationMatrix);
	CurHandPos = XMVectorSetY(CurHandPos, 9.5f);

	// 가야할 방향
	_vector DirToWar = XMVector3Normalize(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - CurHandPos);
	DirToWar = XMVectorSetY(DirToWar, 0.f);

	// 1) 처음 것은 War 방향으로 조금 가도록 하자.
	auto firstPos = CurHandPos + DirToWar * 3.f;
	m_qSlam_L.push(firstPos);

	// 2) 2~4번째까지는  War 방향으로 조금식 앞으로 보내면됩니다. 
	for (int i = 1; i < 5; i++/*_*/)
	{
		auto nextPos = CurHandPos + DirToWar * 8.5f * i;
		m_qSlam_L.push(nextPos);
	}
}


CHollowLord* CHollowLord::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CHollowLord* pInstance = new CHollowLord(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CHollowLord");
		Safe_Release(pInstance);
	}

	return pInstance;
}




CGameObject* CHollowLord::Clone(void* pArg)
{
	CHollowLord* pInstance = new CHollowLord(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CHollowLord");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHollowLord::Free()
{
	CMonster::Free();

	Safe_Delete(m_pHpBar);
}
