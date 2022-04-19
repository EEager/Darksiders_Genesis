#include "stdafx.h"
#include "..\public\War.h"

#include "GameInstance.h"
#include "PipeLine.h"

#include "Light_Manager.h"
#include "Monster\Monster.h"

#include "Trail.h"

#include "ParticleSystem\ParticleSystem_Manager.h"

// In State_War.cpp
#include "State_War.h"
extern CWar* g_pWar;
extern CStateMachine* g_pWar_State_Context;
extern CModel* g_pWar_Model_Context;
extern CModel* g_pWar_Model_Gauntlet_Context;
extern CModel* g_pWar_Model_Ruin_Context;
extern CModel* g_pWar_Model_Sword_Context;
extern CTransform* g_pWar_Transform_Context;



CWar::CWar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CWar::CWar(const CWar & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWar::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CWar::NativeConstruct(void * pArg)
{

#ifdef _DEBUG
	CParticleSystem_Manager::GetInstance()->Add_Particle_To_Layer(L"Particle_Sword");
#endif

	


	// Init GameInfo
	m_tGameInfo.iAtt = 30.f;
	m_tGameInfo.iAtt = 3.f;
	m_tGameInfo.iEnergy = 10;
	m_tGameInfo.iMaxHp = 20;
	m_tGameInfo.iHp = m_tGameInfo.iMaxHp;
	m_tGameInfo.iSoul = 0;

	m_tMtrlDesc.vMtrlSpecular = { 1.f, 1.f, 1.f, 7.f };

	if (SetUp_Component())
		return E_FAIL;	

	XMStoreFloat4x4(&m_WarPivotMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));
	XMStoreFloat4x4(&m_WarRuinPivotMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	// �ʱ� ��ġ
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(17.f, 0.f, 430.f, 1.f));

	// �̺�Ʈ ����
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(670.f, 0.f, 306.f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(619.f, 0.f, 157.f, 1.f)); // Event3

	// Navigation
	m_pNaviCom->SetUp_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));

	// ���� �ݶ��̴��� ����
	if (SetUp_BoneMatrix())
		return E_FAIL;

	// Trail Sword Create
	m_pTrail = CTrail_War_Sword::Create(m_pDevice, m_pDeviceContext);
	m_pTrail->Set_Transform(m_pTransformCom);

	// Trail Dash Create
	m_pTrailDash = CTrail_War_Dash::Create(m_pDevice, m_pDeviceContext);
	m_pTrailDash->Set_Transform(m_pTransformCom);

	// ���� ��ƼŬ Create
	for (int i = 0; i < 4; i++)
	{
		m_pParticle[i] = static_cast<CParticle_War_Dash_Horse*>(CParticleSystem_Manager::GetInstance()->Get_Particle_Available(L"Particle_War_Dash_Horse", i));
	}

	return S_OK;
}

_int CWar::Tick(_float fTimeDelta)
{
	// War �ִϸ��̼� �ε��� ��ȯ - FSM �ӽ����� ����
	if (m_pStateMachineCom)
		m_pStateMachineCom->Tick(fTimeDelta);

	// War Ruin
	if (m_War_On_Ruin_State)
	{
		m_pModelCom[MODELTYPE_WAR]->Update_Animation(fTimeDelta);
		_float4x4 forDontMoveInWorld;
		XMStoreFloat4x4(&forDontMoveInWorld, XMMatrixIdentity());
		m_pModelCom_Ruin->Update_Animation(fTimeDelta, &forDontMoveInWorld, "_Master");
	}
	else // War �⺻
	{
		// War ���������͸� �����־�, �ִϸ��̼� ���� ��ġ�� ���࿡ �����ϵ������� 
		if (m_bDontMoveInWorld == false)
		{
			m_pModelCom[MODELTYPE_WAR]->Update_Animation(fTimeDelta, static_cast<CTransform*>(m_pTransformCom)->Get_WorldMatrix_4x4(), "Bone_War_Root", m_pNaviCom, m_eDir, 0/*ä���ε���*/, m_fOffsetMul);
		}
		else
		{
			m_pModelCom[MODELTYPE_WAR]->Update_Animation(fTimeDelta);
		}
	}

	// �ش� ���� ���´� m_pStateMachineCom->Tick���� ä������
	if (true == m_bJump)
	{
		// ���� �����̸�, ��� ���� �ö󰬴ٰ� �߷�����޾� ��������
		m_pTransformCom->JumpY(fTimeDelta);
	}

	// Collider Update
	Update_Colliders(m_pTransformCom->Get_WorldMatrix());

	// Trail
	{
		// �˿��ٰ� ����
		_matrix		OffsetMatrix = XMLoadFloat4x4(&m_WarSwordDesc.OffsetMatrix); // ��->����
		_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_WarSwordDesc.pBoneMatrix); // Root->�� 

		_matrix		PivotMatrix; // ������ �Ǻ�
		if (m_War_On_Ruin_State)
			PivotMatrix = Get_WarRuinPivot();
		else
			PivotMatrix = XMLoadFloat4x4(&m_WarSwordDesc.PivotMatrix);

		_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_WarSwordDesc.pTargetWorldMatrix); // War ����

		_matrix		TransformationMatrix =
			(OffsetMatrix * CombinedTransformationMatrix * PivotMatrix) *
			TargetWorldMatrix;
		m_pTrail->MyTick(fTimeDelta, &TransformationMatrix);

		// �뽬�� ��쿡�� �׳� ����
		m_pTrailDash->MyTick(fTimeDelta);
	}

	// ��ƼŬ ƽ
	for (auto& pParticle : m_pParticle)
	{
		pParticle->Tick(fTimeDelta);
	}

	return _int();
}

_int CWar::LateTick(_float fTimeDelta)
{
	// ��ƼŬ ����Ʈ ƽ
	for (auto& pParticle : m_pParticle)
	{
		pParticle->LateTick(fTimeDelta);
	}

	if (nullptr == m_pRendererCom)
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Height
	_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	curFloorHeight = m_pNaviCom->Compute_Height(vPosition);
	if (m_bJump) // �������̶�� ������ ���� �������� 
	{
		if (XMVectorGetY(vPosition) < curFloorHeight) // ���� ���� ��ġ�� �� �ؿ� �ִٸ� ������ ���� ���� 
		{
			m_bJump = false;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, curFloorHeight));
		} 
	}
	else // �������� �ƴ϶�� ��� ������ ���� ����  
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, curFloorHeight));
	}

	// Renderer 
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA_WAR, this)))
		goto _EXIT;
	if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
		goto _EXIT;


	// Collider 
	pGameInstance->Add_Collision(this);

	// Trail
	if (m_bTrailOn) // �����Ҷ��� Trail�� �������.
		m_pTrail->LateTick(fTimeDelta); // ���� �������� �ִ´�. 
	if (m_bDashTrailOn)
		g_pWar->m_pTrailDash->LateTick(fTimeDelta);

_EXIT:
	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

HRESULT CWar::Render(_uint iPassIndex)
{
	War_Render(iPassIndex); 
		 
	//War_Outline_Render(iPassIndex); // OutLine�� Deferred�� ó����

	return S_OK;
}



#ifdef _DEBUG
#ifdef USE_IMGUI
#include "imgui_Manager.h"
extern bool m_bshow_naviMesh_window;
#endif
#endif
HRESULT CWar::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{

#ifdef _DEBUG
#ifdef USE_IMGUI
	if (m_bshow_naviMesh_window)
		m_pNaviCom->Render();

	if (m_bUseImGui) // IMGUI ���� ��ġ�ҰŴ�
	{
		m_pTrail->m_bUseImGui = true;
		m_pTrailDash->m_bUseImGui = true;
		CImguiManager::GetInstance()->Transform_Control(m_pTransformCom, m_CloneIdx, &m_bUseImGui);
	}

#endif
	__super::Render_Colliders();
	//m_pRendererCom->ClearRenderStates();
#endif // _DEBUG
	return S_OK;
}

_vector CWar::Get_War_Pos()
{
	return m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
}

void CWar::Set_Jump(_bool warJump, _bool clearJump)
{
	m_bJump = warJump;
	if (clearJump)
		m_pTransformCom->ClearJumpVar();
}

_float CWar::Get_Speed()
{
	assert(m_pTransformCom);
	CTransform::TRANSFORMDESC* pDesc = m_pTransformCom->Get_TransformDesc_Ptr();
	return pDesc->fSpeedPerSec;
}

void CWar::Set_Speed(const _float& fSpeed)
{
	assert(m_pTransformCom);
	m_pTransformCom->Set_TransformDesc_Speed(fSpeed);
}

_float CWar::GetDegree(_ubyte downedKey)
{
	_float retDegree = 0.f;//12 Default�� ������ �ٶ󺸰�����.

	// ī�޶� �ٶ󺸴� ��ġ�� ���� �������� �ٸ��� �־���Ѵ�. 
	// ī�޶� ������ +Z���� 0, �ð�������� + �̴�. 
	_vector CameraToWar_Look = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - CPipeLine::GetInstance()->Get_CamPosition();
	_vector CameraToWar_Look_XZ = XMVectorSet(XMVectorGetX(CameraToWar_Look), 0.f, XMVectorGetZ(CameraToWar_Look), 0.f);
	CameraToWar_Look_XZ = XMVector2Normalize(CameraToWar_Look_XZ);

	_vector vecCameraFromZ = XMVector3AngleBetweenVectors(CameraToWar_Look_XZ, XMVectorSet(0.f, 0.f, 1.f, 0.f)) * (XMVectorGetX(CameraToWar_Look_XZ) < 0.f ? -1.f : 1.f)/* ���� xz �躤�Ͱ� -x���� ���� -1.f�� ������.*/;
	_float degreeCameraFromZ = XMConvertToDegrees(XMVectorGetX(vecCameraFromZ));

	//cout << degreeCameraFromZ << endl;

	// ��������
	if (downedKey == 0b0001)//6
		retDegree = 180.f;
	else if (downedKey == 0b0011)//4
		retDegree = 135.f;
	else if (downedKey == 0b0010)//3
		retDegree = 90.f;
	else if (downedKey == 0b0110)//2
		retDegree = 45.f;
	else if (downedKey == 0b0100)//12
		retDegree = 0.f;
	else if (downedKey == 0b1100)//11
		retDegree = 315.f;
	else if (downedKey == 0b1000)//9
		retDegree = 270.f;
	else if (downedKey == 0b1001)//7
		retDegree = 225.f;

	return retDegree + degreeCameraFromZ;

}

_bool CWar::KeyCheck(IN _ubyte key, OUT _ubyte& keyDownCheckBit)
{
	if (CInput_Device::GetInstance()->Key_Pressing(key))
	{
		_ubyte dirBit = 0b0; // ��������
		switch (key)
		{
		case DIK_A:
			dirBit = 0b1000;
			break;
		case DIK_W:
			dirBit = 0b0100;
			break;
		case DIK_D:
			dirBit = 0b0010;
			break;
		case DIK_S:
			dirBit = 0b0001;
			break;
		default:
			break;
		}

		keyDownCheckBit |= dirBit;
		return true;
	}

	return false;
}

void CWar::War_Key(_float fTimeDelta)
{
	if (m_bDont_Key == true)
		return;

	// 
	// G ��ų
	// 
	if (CInput_Device::GetInstance()->Key_Down(DIK_G))
	{
		int debug = 0;
		if (m_eGType == G_TYPE_FIRE)
			m_eGType = G_TYPE_EARTH;
		else if (m_eGType == G_TYPE_EARTH)
			m_eGType = G_TYPE_FIRE;
	}

	// �� �޺� ��ų�� ī�޶� ���� �յ��¿�� ���������Ѵ�.
	if (m_bDontTurn_OnlyMove)
	{
		CInput_Device* pinputDevice = GET_INSTANCE(CInput_Device);
		if (pinputDevice->Key_Pressing(DIK_A))
		{
			m_pTransformCom->Go_Left_OnCamera(fTimeDelta, m_pNaviCom);
		}
		if (pinputDevice->Key_Pressing(DIK_D))
		{
			m_pTransformCom->Go_Right_OnCamera(fTimeDelta, m_pNaviCom);
		}
		if (pinputDevice->Key_Pressing(DIK_W))
		{
			m_pTransformCom->Go_Straight_OnCamera(fTimeDelta, m_pNaviCom);
		}
		if (pinputDevice->Key_Pressing(DIK_S))
		{
			m_pTransformCom->Go_Backward_OnCamera(fTimeDelta, m_pNaviCom);
		}
		RELEASE_INSTANCE(CInput_Device);
	}
	// �⺻���� �̵�
	else
	{
		// 
		// �̵� + ȸ�� 
		//
		unsigned char keyDownCheckBit = 0b0;
		bool isKeyDown = false;
		auto const dirtyCheck = [&isKeyDown, &keyDownCheckBit](_bool b) { isKeyDown |= b; };
		dirtyCheck(KeyCheck(DIK_A, keyDownCheckBit));
		dirtyCheck(KeyCheck(DIK_W, keyDownCheckBit));
		dirtyCheck(KeyCheck(DIK_D, keyDownCheckBit));
		dirtyCheck(KeyCheck(DIK_S, keyDownCheckBit));

		if (isKeyDown == false)
			return;

		// War ������ ȸ����Ű��
		m_pTransformCom->TurnTo_AxisY_Degree(GetDegree(keyDownCheckBit), fTimeDelta * 10); 

		if (m_bDontMove_OnlyTurn == false) // ���� ���� �ƴҶ�
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNaviCom);
	}

}

// @Override
_int CWar::Update_Colliders(_matrix wolrdMatrix)
{
	for (auto pCollider : m_ColliderList)
	{
		if (pCollider->Get_ColliderTag() == L"WarWeapon")
		{
			_matrix		OffsetMatrix = XMLoadFloat4x4(&m_WarSwordDesc.OffsetMatrix); // ��->����
			_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_WarSwordDesc.pBoneMatrix); // Root->�� 

			_matrix		PivotMatrix; // ������ �Ǻ�
			if (m_War_On_Ruin_State)
				PivotMatrix = Get_WarRuinPivot();
			else
				PivotMatrix = XMLoadFloat4x4(&m_WarSwordDesc.PivotMatrix); 

			_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_WarSwordDesc.pTargetWorldMatrix); // War ����

			_matrix		TransformationMatrix = 
				(OffsetMatrix * CombinedTransformationMatrix * PivotMatrix) * 
				TargetWorldMatrix; 
			pCollider->Update(TransformationMatrix);
		}
		else
		{
			pCollider->Update(wolrdMatrix);
		}
	}
	return 0;
}


/*
War �������� ��� �浹�ߴ���, m_iHitDir�� set����.
\   F  / 
 \    /
  \  /
L  war R
  /  \
 /    \
/   B  \
*/

void CWar::Set_Collision_Direction(CTransform* pDstTransform)
{
	_vector toTarget = pDstTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	// Front
	if (XMVectorGetX(XMVector3Dot(toTarget, m_pTransformCom->Get_State(CTransform::STATE_LOOK))) >= 0.525/*cos45*/)
	{
		m_iHitDir = HIT_FROM_FRONT;
		return;
	}

	// Back
	if (XMVectorGetX(XMVector3Dot(toTarget, m_pTransformCom->Get_State(CTransform::STATE_LOOK))) <= -0.525/*cos135*/)
	{
		m_iHitDir = HIT_FROM_BACK;
		return;
	}

	// Right
	if (XMVectorGetX(XMVector3Dot(toTarget, m_pTransformCom->Get_State(CTransform::STATE_RIGHT))) >= 0.525/*cos45*/)
	{
		m_iHitDir = HIT_FROM_RIGHT;
		return;
	}
	// Left
	if (XMVectorGetX(XMVector3Dot(toTarget, m_pTransformCom->Get_State(CTransform::STATE_RIGHT))) <= 0.525/*cos45*/)
	{
		m_iHitDir = HIT_FROM_LEFT;
		return;
	}
}

void CWar::OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
	// �÷��̾� ����� ���� ���� �浹�� ���. 
	if (m_bHitted == false && pSrc->Get_ColliderTag() == COL_WAR_BODY1 &&
		pDst->Get_ColliderTag() == COL_MONSTER_WEAPON)
	{
		m_bHitted = true;
		m_fHitPower = .65f;

		CGameObject* pDstObj = pDst->Get_Owner();
		m_tGameInfo.iHp -= pDstObj->m_tGameInfo.iAtt;
#ifdef _DEBUG
		cout << DXString::WideToChar(this->m_pLayerTag) << ": " << m_tGameInfo.iHp << endl;
#endif
		// �ǰݽ� �ǰݸ������ õ���ϱ� ���� ��� ���⿡�� �ǰݵǾ����� ���������.
		// ������ ���۾Ƹ� ���¿����� �ǰݻ��·� õ�̰� �ȵȴ�. �������� �Դ´�. 
		if (m_bSuperArmor == false)
		{
			// ������⿡�� �¾Ҵ��� m_iHitDir�� ��������
			Set_Collision_Direction(static_cast<CTransform*>(pDstObj->Get_ComponentPtr(L"Com_Transform")));
		}
		return;
	}
}


#define PUSH_LENGTH_MON 1.5f
#define PUSH_LENGTH_BAl 3.f
void CWar::OnCollision_Stay(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
	// [�о��]
	if (pSrc->Get_ColliderTag() == COL_WAR_BODY1)
	{
		// War ���� vs ���� �����ΰ��, ���͸� �о��. 
		if (pDst->Get_ColliderTag() == COL_MONSTER_BODY1 || pDst->Get_ColliderTag() == COL_MONSTER_BODY2)
		{
			CMonster* pMonster = static_cast<CMonster*>(pDst->Get_Owner());
			CTransform* pMonTransform = pMonster->Get_Transform();
			_vector toTarget = XMVectorSetY(pMonTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f);
			_float fLength = XMVectorGetX(XMVector3Length(toTarget));
			if (fLength < 1.f)
			{
				// ������ ���Ͱ� ���۾Ƹ� ���¶�� �÷��̾ �з����� 
				if (pMonster->m_bSuperArmor)
					m_pTransformCom->Go_Dir(toTarget, -fTimeDelta); // �÷��̾ �з����� 
				else
					pMonTransform->Go_Dir(toTarget, fTimeDelta); // ���Ͱ� �з�����
			}
		}
		// ������ �ٸ���Ÿ �Ǵ� �����ΰ��, War�� �о��, War�� �������� �ʰ� �ڷ� �������Ѵ�.
		else if (pDst->Get_ColliderTag() == COL_BALLISTA_BODY)
		{
			CTransform* pBallistaTrans = static_cast<CTransform*>(pDst->Get_Owner()->Get_ComponentPtr(L"Com_Transform"));
			_float fLength = XMVectorGetX(XMVector3Length(pBallistaTrans->Get_State(CTransform::STATE_POSITION)
				- m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
			if (fLength < PUSH_LENGTH_BAl)
			{
				m_pTransformCom->Go_Backward(fTimeDelta);
			}
		}
		// �μ����� �͵��̶��. �÷��̾ �з�����
		else if (pDst->Get_ColliderTag() == COL_BREAKABLE_BODY)
		{
			CTransform* pBreakableTrans = static_cast<CTransform*>(pDst->Get_Owner()->Get_ComponentPtr(L"Com_Transform"));
			_vector toTarget = XMVectorSetY(pBreakableTrans->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f);
			_float fLength = XMVectorGetX(XMVector3Length(toTarget));
			if (fLength < PUSH_LENGTH_BAl)
			{
				m_pTransformCom->Go_Dir(toTarget, -fTimeDelta); // �÷��̾ �з����� 
			}
		}
	}
}

void CWar::OnCollision_Leave(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
}

HRESULT CWar::SetUp_Component()
{
	// Transform
	{
		/* For.Com_Transform */
		CTransform::TRANSFORMDESC		TransformDesc;
		ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

		TransformDesc.fSpeedPerSec = WAR_SPEED;
		TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
			return E_FAIL;
	}

	// Renderer
	{
		/* For.Com_Renderer*/
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
			return E_FAIL;
	}

	
	// War Model
	{
		/* For.Com_Model_War */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War"), TEXT("Com_Model_War"), (CComponent**)&m_pModelCom[MODELTYPE_WAR])))
			return E_FAIL;
		m_pModelCom[MODELTYPE_WAR]->SetUp_Animation("War_Mesh.ao|War_Idle");

		/* For.Com_Model_War_Ruin */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War_Ruin"), TEXT("Com_Model_War_Ruin"), (CComponent**)&m_pModelCom_Ruin)))
			return E_FAIL;
		m_pModelCom_Ruin->SetUp_Animation("War_Ruin_Mesh.ao|War_Horse_Mount_Running", false, false);//Not Loop


		/* For.Com_Model_War_Gauntlet */
			// �ִϸ��̼��� Com_Model_War�� ��������
		CModel::MODELDESC	tagModelWarGauntletDesc;
		tagModelWarGauntletDesc.pHierarchyNodes = m_pModelCom[MODELTYPE_WAR]->Get_HierarchyNodes();
		tagModelWarGauntletDesc.pAnimations = m_pModelCom[MODELTYPE_WAR]->Get_Animations();
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War_Gauntlet"), TEXT("Com_Model_War_Gauntlet"), (CComponent**)&m_pModelCom[MODELTYPE_GAUNTLET], &tagModelWarGauntletDesc)))
			return E_FAIL;

		/* For.Prototype_Component_Model_War_Weapon */
			// Model_War�� ���� �ִ� ���� �̸��� "Bone_War_Weapon_Sword" �ΰ��� ã�� �־�����.
		CModel::MODELDESC	tagModelWarWeaponDesc;
		tagModelWarWeaponDesc.pHierarchyNode = m_pModelCom[MODELTYPE_WAR]->Find_HierarchyNode("Bone_War_Weapon_Sword");
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War_Weapon"), TEXT("Com_Model_War_Weapon"), (CComponent**)&m_pModelCom[MODELTYPE_WEAPON], &tagModelWarWeaponDesc)))
			return E_FAIL;

		/* For.Com_StateMachine : Model �ڿ� �ؾ��Ѵ�. State Ready���� Model�� ����ϱ� ����. */
		g_pWar_Model_Context = m_pModelCom[MODELTYPE_WAR];
		g_pWar_Model_Gauntlet_Context = m_pModelCom[MODELTYPE_GAUNTLET];
		g_pWar_Model_Sword_Context = m_pModelCom[MODELTYPE_WEAPON];
		g_pWar_Model_Ruin_Context = m_pModelCom_Ruin;
		g_pWar_Transform_Context = m_pTransformCom;
		CStateMachine::STATEMACHINEDESC fsmDesc;
		fsmDesc.pOwner = this;
		fsmDesc.pInitState = CState_War_Idle::GetInstance();
		// For.Com_StateMachine
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachineCom, &fsmDesc)))
			return E_FAIL;
		static_cast<CStateMachine*>(m_pStateMachineCom)->Set_GlobalState(CGlobal_State_War::GetInstance());
		g_pWar_State_Context = m_pStateMachineCom;
		g_pWar = this;
	}


	// Collider
	{
		/* For.Com_OBB */
		CCollider::COLLIDERDESC		ColliderDesc;
		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vPivot = _float3(0.f, 1.0f, 0.f);
		ColliderDesc.vSize = _float3(1.4f, 2.0f, 1.4f);
		ColliderDesc.eColType = CCollider::COL_TYPE::COL_TYPE_AABB;
		__super::Add_Collider(&ColliderDesc, COL_WAR_BODY1);

		/* For.WarWeapon */
		ColliderDesc.vPivot = _float3(0.f, 0.f, 0.9f); //Į ��
		//ColliderDesc.vSize = _float3(0.2f, 0.5f, 2.3f); //x:Į��, y:Į�ʺ�, z:Į����
		ColliderDesc.vSize = _float3(0.5f, 1.0f, 3.f); //x:Į��, y:Į�ʺ�, z:Į����
		ColliderDesc.eColType = CCollider::COL_TYPE_OBB;
		__super::Add_Collider(&ColliderDesc, COL_WAR_WEAPON, true);
	}


	{
		/* For.Com_Navi */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navi"), (CComponent**)&m_pNaviCom)))
			return E_FAIL;
	}


	return S_OK;
}


HRESULT CWar::SetUp_BoneMatrix()
{
	ZeroMemory(&m_WarSwordDesc, sizeof(SWORDDESC));
	m_WarSwordDesc.pBoneMatrix = m_pModelCom[MODELTYPE_WAR]->Get_CombinedMatrixPtr("Bone_War_Weapon_Sword"); // War root -> ���
	m_WarSwordDesc.OffsetMatrix = m_pModelCom[MODELTYPE_WAR]->Get_OffsetMatrix("Bone_War_Weapon_Sword"); // War ��->����
	m_WarSwordDesc.PivotMatrix = m_pModelCom[MODELTYPE_WAR]->Get_PivotMatrix_Bones(); // War �Ǻ��� 
	m_WarSwordDesc.pTargetWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr(); // War ����

	return S_OK;
}



HRESULT CWar::SetUp_Ruin_ConstantTable(_uint iPassIndex, bool drawOutLine)
{
	if (nullptr == m_pModelCom_Ruin)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Bind Directional Light
	LIGHTDESC		dirLightDesc = *pGameInstance->Get_LightDesc(0);
	DirectionalLight mDirLight;
	mDirLight.Ambient = dirLightDesc.vAmbient;
	mDirLight.Diffuse = dirLightDesc.vDiffuse;
	mDirLight.Specular = dirLightDesc.vSpecular;
	mDirLight.Direction = dirLightDesc.vDirection;
	m_pModelCom_Ruin->Set_RawValue("g_DirLight", &mDirLight, sizeof(DirectionalLight));

	// Bind Material
	m_pModelCom_Ruin->Set_RawValue("g_Material", &m_tMtrlDesc, sizeof(MTRLDESC));

	// Bind Transform
	m_pTransformCom->Bind_OnShader(m_pModelCom_Ruin, "g_WorldMatrix");
	if (iPassIndex == 3)
	{
		m_pModelCom_Ruin->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_View())), sizeof(_float4x4));
		m_pModelCom_Ruin->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_Proj())), sizeof(_float4x4));
	}
	else
	{
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom_Ruin, "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom_Ruin, "g_ProjMatrix");
	}

	//// Bind Position
	//_float4			vCamPosition;
	//XMStoreFloat4(&vCamPosition, pGameInstance->Get_CamPosition());
	//m_pModelCom_Ruin->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));

	bool tmpFalse = false;
	bool tmpTrue = true;
	// Branch to Use Normal Mapping 
	// ��ָ����� ���� ������ ���⼭ ����
	m_pModelCom_Ruin->Set_RawValue("g_UseNormalMap", &tmpTrue, sizeof(bool));
	m_pModelCom_Ruin->Set_RawValue("g_UseEmissiveMap", &tmpTrue, sizeof(bool));

	// �ش� ���� set �Ǹ� Outline�� �׸���.
	m_pModelCom_Ruin->Set_RawValue("g_DrawOutLine", &drawOutLine, sizeof(bool));

	// ������Ʈ
	//float isMonster = 0.f;
	//m_pModelCom_Ruin->Set_RawValue("g_fIsMonster", &isMonster, sizeof(float));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}



HRESULT CWar::SetUp_ConstantTable(_uint iPssIndex, bool drawOutLine, int modelIdx)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);	

	// Bind Directional Light
	LIGHTDESC		dirLightDesc = *pGameInstance->Get_LightDesc(0);
	DirectionalLight mDirLight;
	mDirLight.Ambient = dirLightDesc.vAmbient;
	mDirLight.Diffuse = dirLightDesc.vDiffuse;
	mDirLight.Specular = dirLightDesc.vSpecular;
	mDirLight.Direction = dirLightDesc.vDirection;
	m_pModelCom[modelIdx]->Set_RawValue("g_DirLight", &mDirLight, sizeof(DirectionalLight));

	// Bind Material
	m_pModelCom[modelIdx]->Set_RawValue("g_Material", &m_tMtrlDesc, sizeof(MTRLDESC));

	// Bind Transform
	m_pTransformCom->Bind_OnShader(m_pModelCom[modelIdx], "g_WorldMatrix");
	if (iPssIndex == 3) // ShadowMap
	{
		m_pModelCom[modelIdx]->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_View())), sizeof(_float4x4));
		m_pModelCom[modelIdx]->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_Proj())), sizeof(_float4x4));
	}
	else
	{
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom[modelIdx], "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom[modelIdx], "g_ProjMatrix");
	}

	//// Bind Position
	//_float4			vCamPosition;
	//XMStoreFloat4(&vCamPosition, pGameInstance->Get_CamPosition());
	//m_pModelCom[modelIdx]->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));

	// Branch to Use Normal Mapping 
	// ��ָ����� ���� ������ ���⼭ ����
	m_pModelCom[modelIdx]->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));
	if (modelIdx == MODELTYPE_WEAPON) // ����� EmissiveMap ������� ���� �ϴ�.
	{
		bool tempFalse = false;
		m_pModelCom[modelIdx]->Set_RawValue("g_UseEmissiveMap", &tempFalse, sizeof(bool));
	}
	else
		m_pModelCom[modelIdx]->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));

	// Outline ������ �׸����ʴ´�.
	m_pModelCom[modelIdx]->Set_RawValue("g_DrawOutLine", &drawOutLine, sizeof(bool));

	// Roughness Map �������
	m_pModelCom[modelIdx]->Set_RawValue("g_UseRoughnessMap", &g_bUseRoughnessMap, sizeof(bool));
	m_pModelCom[modelIdx]->Set_RawValue("g_UseMetalMap", &g_bUseMetalicMap, sizeof(bool));

	//// ������Ʈ
	//float isMonster = 0.f;
	//m_pModelCom[modelIdx]->Set_RawValue("g_fIsMonster", &isMonster, sizeof(float));

	if (modelIdx == 0 || modelIdx == 1) 
		// �ǰݽ� ���� �÷��̾�� ������� �����Ҳ���.
		m_pModelCom[modelIdx]->Set_RawValue("g_vHitPower", &XMVectorSet(m_fHitPower, m_fHitPower, m_fHitPower, 0.f), sizeof(_vector));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


HRESULT CWar::War_Render(_uint iPassIndex)
{
	/* ��ġ�� ���庯ȯ ����� �����Ѵ�. */
	for (int modelIdx = 0; modelIdx < MODELTYPE_END; modelIdx++)
	{
		if (FAILED(SetUp_ConstantTable(iPassIndex, false, modelIdx)))
			return E_FAIL;

		// iNumMaterials : ����, ����
		_uint	iNumMaterials = m_pModelCom[modelIdx]->Get_NumMaterials();
		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelCom[modelIdx]->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
			m_pModelCom[modelIdx]->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
			m_pModelCom[modelIdx]->Set_ShaderResourceView("g_EmissiveTexture", i, aiTextureType_EMISSIVE);
			m_pModelCom[modelIdx]->Set_ShaderResourceView("g_MetalRoughnessTexture", i, aiTextureType_SHININESS);

			m_pModelCom[modelIdx]->Render(i, iPassIndex);
		}
	}


	//
	// �� ������
	//
	if (m_War_On_Ruin_State)
	{
		if (FAILED(SetUp_Ruin_ConstantTable(iPassIndex, false)))
			return E_FAIL;
		auto iNum_RuinMaterials = m_pModelCom_Ruin->Get_NumMaterials();
		for (_uint i = 0; i < iNum_RuinMaterials; ++i)
		{
			m_pModelCom_Ruin->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
			m_pModelCom_Ruin->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
			m_pModelCom_Ruin->Set_ShaderResourceView("g_EmissiveTexture", i, aiTextureType_EMISSIVE);
			m_pModelCom_Ruin->Render(i, iPassIndex);
		}
	}

	return S_OK;
}

HRESULT CWar::War_Outline_Render(_uint iPassIndex)
{
	//// 
	//// 2. (���ٽǹ��۰� 0�ΰ�쿡, �����׽�Ʈ�� ���� �Ͼ�Ƿ� ���ٽ� ���۸� ����� ��찡 �߻��� ��쿡) 
	//// War �ܰ��� Draw, Draw ������ : ����->NONALPHA->War ��
	//// 
	//m_pDeviceContext->OMSetDepthStencilState(RenderStates::DrawReflectionDSS.Get(), 0);

	//for (int modelIdx = 0; modelIdx < MODELTYPE_END; modelIdx++)
	//{
	//	if (FAILED(SetUp_ConstantTable(true, modelIdx)))
	//		return E_FAIL;

	//	_uint	iNumMaterials = m_pModelCom[modelIdx]->Get_NumMaterials();
	//	for (_uint i = 0; i < iNumMaterials; ++i)
	//	{
	//		m_pModelCom[modelIdx]->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
	//		m_pModelCom[modelIdx]->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
	//		m_pModelCom[modelIdx]->Set_ShaderResourceView("g_EmissiveTexture", i, aiTextureType_EMISSIVE);


	//		m_pModelCom[modelIdx]->Render(i, iPassIndex); // Forward_ApiRenderState_Pass
	//	}
	//}

	////
	//// �� �ܰ��� ������
	////
	//if (m_War_On_Ruin_State)
	//{
	//	if (FAILED(SetUp_Ruin_ConstantTable(true)))
	//		return E_FAIL;
	//	auto iNum_RuinMaterials = m_pModelCom_Ruin->Get_NumMaterials();
	//	for (_uint i = 0; i < iNum_RuinMaterials; ++i)
	//	{
	//		m_pModelCom_Ruin->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
	//		m_pModelCom_Ruin->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
	//		m_pModelCom_Ruin->Set_ShaderResourceView("g_EmissiveTexture", i, aiTextureType_EMISSIVE);
	//		m_pModelCom_Ruin->Render(i, iPassIndex); // Forward_ApiRenderState_Pass
	//	}
	//}

	//// Restore default states
	//m_pRendererCom->ClearRenderStates();

	return S_OK;
}



CWar * CWar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CWar*		pInstance = new CWar(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create CWar");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CWar::Clone(void* pArg)
{
	CWar*		pInstance = new CWar(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CWar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWar::Free()
{
	__super::Free();


	Safe_Release(m_pNaviCom);
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pAABBCom);
	Safe_Release(m_pTransformCom);	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pStateMachineCom);

	Safe_Release(m_pTrail);
	Safe_Release(m_pTrailDash);

	for (auto pParticle : m_pParticle)
	{
		Safe_Release(pParticle);
	}


	Safe_Release(m_pModelCom_Ruin);

	for (auto& modelCom : m_pModelCom)
		Safe_Release(modelCom);

	// Destroy the State SingleTon : State_War.cpp 
	g_pWar_State_Context = nullptr;
	g_pWar_Model_Context = nullptr;
	g_pWar_Model_Gauntlet_Context = nullptr;
	g_pWar_Model_Ruin_Context = nullptr;
	g_pWar_Transform_Context = nullptr;
}
