#include "stdafx.h"
#include "..\public\Trail.h"

#include "GameInstance.h"

#ifdef USE_IMGUI
#include "imgui_Manager.h"
#endif



//-------------------------------------------------
// CTrail
//-------------------------------------------------
CTrail::CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTrail::CTrail(const CTrail& rhs)
	: CGameObject(rhs)
{
}


HRESULT CTrail::NativeConstruct_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CTrail::NativeConstruct(void* pArg)
{
	// Not Used
	return E_NOTIMPL;
}

_int CTrail::Tick(_float fTimeDelta)
{
	return _int();
}

_int CTrail::LateTick(_float fTimeDelta)
{
	return _int();
}

HRESULT CTrail::Render(_uint iPassIndex)
{
	return E_NOTIMPL;
}

HRESULT CTrail::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	return 0;
}

_int CTrail::MyTick(_float fTimeDelta, _fmatrix* pBonemMat)
{
	return _int();
}

CTrail* CTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	return nullptr;
}

CGameObject* CTrail::Clone(void* pArg)
{
	return nullptr;
}

void CTrail::Free()
{
}


//-------------------------------------------------
// CTrail_War_Sword
//-------------------------------------------------
CTrail_War_Sword::CTrail_War_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CTrail(pDevice, pDeviceContext)
{
}

CTrail_War_Sword::CTrail_War_Sword(const CTrail_War_Sword& rhs)
	: CTrail(rhs)
{
}

HRESULT CTrail_War_Sword::NativeConstruct_Prototype()
{
	SetUp_Component();
	return S_OK;
}

HRESULT CTrail_War_Sword::NativeConstruct(void* pArg)
{
	// Not Used
	return S_OK;
}

_int CTrail_War_Sword::Tick(_float fTimeDelta)
{
	// Not Used

	return _int();
}

_int CTrail_War_Sword::MyTick(_float fTimeDelta, _fmatrix* pBonemMat)
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

_int CTrail_War_Sword::LateTick(_float fTimeDelta)
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

HRESULT CTrail_War_Sword::Render(_uint iPassIndex)
{
	// 2 : Alphablending_NoCullPass
	// 3 : Alphablending_NoCull_DistortionPass
	iPassIndex = 3;
	if (FAILED(SetUp_ConstantTable(iPassIndex)))
		return E_FAIL;

	m_pTrail->Render(iPassIndex);

	return S_OK;
}

HRESULT CTrail_War_Sword::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
#ifdef _DEBUG
	// Collider 
	__super::Render_Colliders();
#endif // _DEBUG

#ifdef USE_IMGUI
	if (m_bUseImGui)
	{
		char TagTmp[32];
		sprintf_s(TagTmp, "CTrail_War_Sword##%d", m_CloneIdx);
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


HRESULT CTrail_War_Sword::SetUp_Component()
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

	/* For.Com_Texture_Noise */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_noise"), TEXT("Com_Texture_Noise"), (CComponent**)m_pDistortionNoiseTextureCom.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrail_War_Sword::SetUp_ConstantTable(_uint iPassIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Bind Transform
	m_pTargetTransform->Bind_OnShader(m_pTrail.Get(), "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pTrail.Get(), "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pTrail.Get(), "g_ProjMatrix");

	if (FAILED(m_pTrailTextureCom->SetUp_OnShader(m_pTrail.Get(), "g_DiffuseTexture", m_TrailTextureIdx)))
		return E_FAIL;

	// 트레일 + 왜곡을 주고 싶을때 이 패스를 사용한다. 
	if (iPassIndex == 3)
	{
		// 근데 Renderer Render_Blend_Final에서 tmpTrue일때는 1번이 최고네 ㅎㅎ
		if (FAILED(m_pDistortionNoiseTextureCom->SetUp_OnShader(m_pTrail.Get(), "g_NoiseTexture",1)))
			return E_FAIL; 
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}


CTrail_War_Sword* CTrail_War_Sword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTrail_War_Sword* pInstance = new CTrail_War_Sword(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CTrail_War_Sword");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CTrail_War_Sword::Clone(void* pArg)
{
	CTrail_War_Sword* pInstance = new CTrail_War_Sword(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CTrail_War_Sword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail_War_Sword::Free()
{
	__super::Free();
	Safe_Release(m_pTargetTransform);
}



//-------------------------------------------------
// CTrail_War_Dash
//-------------------------------------------------
