#include "stdafx.h"
#include "..\public\MapObject/Ballista.h"
#include "GameInstance.h"

#ifdef USE_IMGUI
#include "imgui_Manager.h"
#endif

CBallista::CBallista(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBallista::CBallista(const CBallista & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBallista::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CBallista::NativeConstruct(void * pArg)
{
	if (SetUp_Component())
		return E_FAIL;

	m_pModelCom->SetUp_Animation("Ballista_A.ao|Ballista_A_Full");

	return S_OK;
}

_int CBallista::Tick(_float fTimeDelta)
{
	m_pModelCom->Update_Animation(fTimeDelta);

	return _int();
}

_int CBallista::LateTick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// AddRenderGroup
	bool AddRenderGroup = false;
	if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		AddRenderGroup = true;

#ifdef USE_IMGUI
	if (m_bUseImGui) // 음... 이거 고려해봐야할듯? 
		AddRenderGroup = true;
#endif

	if (AddRenderGroup)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			assert(0);
		if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
			assert(0);
	}

	//// Collider 
	//pGameInstance->Add_Collision(this);

	RELEASE_INSTANCE(CGameInstance);
	return 0;
}

HRESULT CBallista::Render(_uint iPassIndex)
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

	return S_OK;
}

HRESULT CBallista::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
#ifdef USE_IMGUI
	if (m_bUseImGui)
	{
		char TagTmp[32];
		sprintf_s(TagTmp, "Edit##%d", m_CloneIdx);
		ImGui::Begin(TagTmp, &m_bUseImGui);
		{
			float vec3f[3] = { 0,0,0 };
			_vector temp = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vec3f[0] = XMVectorGetX(temp);
			vec3f[1] = XMVectorGetY(temp);
			vec3f[2] = XMVectorGetZ(temp);

			// Position 
			if (ImGui::DragFloat3("Position", vec3f, 1.f))
			{
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vec3f);
			}

			// Angle
			{
				static _float rotateSnap = 0.1f; // 10번하면 90도임
				ImGui::InputFloat("Snap Angle", &rotateSnap, 0.1f);

				if (ImGui::Button("Rt -X"))
					m_pTransformCom->Turn({ 1.f,0.f,0.f }, -rotateSnap);
				ImGui::SameLine();
				if (ImGui::Button("Rt +X"))
					m_pTransformCom->Turn({ 1.f,0.f,0.f }, rotateSnap);

				if (ImGui::Button("Rt -Y"))
					m_pTransformCom->Turn({ 0.f,1.f,0.f }, -rotateSnap);
				ImGui::SameLine();
				if (ImGui::Button("Rt +Y"))
					m_pTransformCom->Turn({ 0.f,1.f,0.f }, rotateSnap);

				if (ImGui::Button("Rt -Z"))
					m_pTransformCom->Turn({ 0.f,0.f,1.f }, -rotateSnap);
				ImGui::SameLine();
				if (ImGui::Button("Rt +Z"))
					m_pTransformCom->Turn({ 0.f,0.f,1.f }, rotateSnap);

				if (ImGui::Button("Reset"))
				{
					m_pTransformCom->Set_State(CTransform::STATE::STATE_RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f) * m_pTransformCom->Get_Scale(CTransform::STATE::STATE_RIGHT));
					m_pTransformCom->Set_State(CTransform::STATE::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f) * m_pTransformCom->Get_Scale(CTransform::STATE::STATE_UP));
					m_pTransformCom->Set_State(CTransform::STATE::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f) * m_pTransformCom->Get_Scale(CTransform::STATE::STATE_LOOK));
				}
			}
		}
		ImGui::End();
	}
#endif

	return S_OK;
}

HRESULT CBallista::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Ballista"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	//// Collider 
	///* For.Com_AABB */
	//CCollider::COLLIDERDESC		ColliderDesc;
	//ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	//ColliderDesc.vPivot = _float3(1.f, 1.f, 1.f);
	//ColliderDesc.vSize = _float3(2.f, 2.f, 2.f);
	//ColliderDesc.eColType = CCollider::COL_TYPE::COL_TYPE_AABB;
	//__super::Add_Collider(&ColliderDesc, L"COL_MONSTER_BODY1");

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


CBallista * CBallista::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBallista*		pInstance = new CBallista(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CBallista");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBallista::Clone(void* pArg)
{
	CBallista*		pInstance = new CBallista(*this);

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
}
