#include "stdafx.h"
#include "..\public\Trail.h"

#include "GameInstance.h"

#ifdef USE_IMGUI
#include "imgui_Manager.h"
#endif

CTrail::CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTrail::CTrail(const CTrail& rhs)
	: CGameObject(rhs),
	m_pRendererCom(rhs.m_pRendererCom)
{
}

HRESULT CTrail::NativeConstruct_Prototype()
{
	SetUp_Component();
	return S_OK;
}

HRESULT CTrail::NativeConstruct(void* pArg)
{
	// Not Used
	return S_OK;
}

_int CTrail::Tick(_float fTimeDelta)
{
	// Not Used

	return _int();
}

_int CTrail::MyTick(_float fTimeDelta, _fmatrix* pBonemMat)
{
	_float3 up;
	_float3 down;

	_vector pTemp = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
	if (!pBonemMat) // Bone가 없으면
	{
		XMStoreFloat3(&up, pTemp + XMLoadFloat3(&m_vUpPosOffset));
		XMStoreFloat3(&down, pTemp + XMLoadFloat3(&m_vDownPosOffset));
	}
	else // 뼈 정보가 있으면
	{
		XMStoreFloat3(&up, XMVector3TransformCoord(XMLoadFloat3(&m_vUpPosOffset), *pBonemMat));
		XMStoreFloat3(&down, XMVector3TransformCoord(XMLoadFloat3(&m_vDownPosOffset), *pBonemMat));
	}

	m_pTrail->AddNewTrail(up, down, fTimeDelta);
	m_pTrail->Update(fTimeDelta, &m_pTargetTransform->Get_WorldMatrix());

	return 0;
}

_int CTrail::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// AddRenderGroup
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHA, this)))	assert(0);
	if (FAILED(m_pRendererCom->Add_PostRenderGroup(this))) assert(0);

	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

HRESULT CTrail::Render(_uint iPassIndex)
{
	if (FAILED(SetUp_ConstantTable(iPassIndex)))
		return E_FAIL;

	m_pTrail->Render(2); // Alphablending_NoCullPass

	return S_OK;
}

HRESULT CTrail::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
#ifdef _DEBUG
	// Collider 
	__super::Render_Colliders();
#endif // _DEBUG

#ifdef USE_IMGUI
	if (m_bUseImGui)
	{
		char TagTmp[32];
		sprintf_s(TagTmp, "CTrail##%d", m_CloneIdx);
		ImGui::Begin(TagTmp, &m_bUseImGui);
		{
			ImGui::InputInt("TextureIdx", &m_TrailTextureIdx);
			ImGui::DragFloat("m_dDuration", (float*)&m_pTrail->m_fDuration);
			ImGui::DragFloat("m_dAliveTime", (float*)&m_pTrail->m_fAliveTime);
			ImGui::DragInt("m_LerpCnt", (int*)&(m_pTrail->m_LerpCnt));
			 
			float vec3f_Up[3] = {m_vUpPosOffset.x, m_vUpPosOffset.y, m_vUpPosOffset.z};
			float vec3f_Dn[3] = {m_vDownPosOffset.x, m_vDownPosOffset.y, m_vDownPosOffset.z}; 

			// Up Offset Position 
			if (ImGui::DragFloat3(" Up Offset Position", vec3f_Up, 1.f))
				m_vUpPosOffset = _float3(vec3f_Up[0], vec3f_Up[1], vec3f_Up[2]);

			// Dn Offset Position 
			if (ImGui::DragFloat3("Dn Offset Position", vec3f_Dn, 1.f))
				m_vDownPosOffset = _float3(vec3f_Dn[0], vec3f_Dn[1], vec3f_Dn[2]);

		}
		ImGui::End();
	}
#endif

	return S_OK;
}


HRESULT CTrail::SetUp_Component()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)m_pRendererCom.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Buffer_Trail */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_Buffer_Trail"), (CComponent**)m_pTrail.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Trail_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Trail"), TEXT("Com_Trail_Texture"), (CComponent**)m_pTrailTextureCom.GetAddressOf())))
		return E_FAIL;
	return S_OK;
}

HRESULT CTrail::SetUp_ConstantTable(_uint iPassIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Bind Transform
	m_pTargetTransform->Bind_OnShader(m_pTrail.Get(), "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pTrail.Get(), "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pTrail.Get(), "g_ProjMatrix");

	if (FAILED(m_pTrailTextureCom->SetUp_OnShader(m_pTrail.Get(), "g_DiffuseTexture", m_TrailTextureIdx)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	m_pTrail->Render(2); // Alphablending_NoCullPass
	return S_OK;
}


CTrail* CTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTrail* pInstance = new CTrail(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CTrail::Clone(void* pArg)
{
	CTrail* pInstance = new CTrail(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail::Free()
{
	__super::Free();
	Safe_Release(m_pTargetTransform);
}
