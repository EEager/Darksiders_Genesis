#include "stdafx.h"
#include "..\public\SceneChangeEffect.h"

#include "GameInstance.h"

#include "Level.h"

#ifdef USE_IMGUI
#include "imgui_Manager.h"
#endif


CSceneChangeEffect1::CSceneChangeEffect1(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSceneChangeEffect1::CSceneChangeEffect1(const CSceneChangeEffect1& rhs)
	: CGameObject(rhs),
	m_ProjMatrix(rhs.m_ProjMatrix), 
	m_ViewMatrix(rhs.m_ViewMatrix),
	m_pTextureCom(rhs.m_pTextureCom),
	m_pRendererCom(rhs.m_pRendererCom),
	m_pVIBufferCom(rhs.m_pVIBufferCom)
{
}

HRESULT CSceneChangeEffect1::NativeConstruct_Prototype()
{	
	if (SetUp_Component())
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f)));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CSceneChangeEffect1::NativeConstruct(void * pArg)
{

	return S_OK;
}

_int CSceneChangeEffect1::Tick(_float fTimeDelta)
{
	if (m_isDead)
		return -1;

	m_fTexturePosY -= 15;
	return _int();
}

_int CSceneChangeEffect1::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
		return 0;
	
	if (m_bWillDead)
		m_isDead = true;

	return _int();
}

HRESULT CSceneChangeEffect1::Render(_uint iPassIndex)
{
	for (int i = 0; i < 11; i++)
	{
		_float finalposY = m_fTexturePosY + 100.f * i;
		if (finalposY <= MAX_TEXTURE_POS_Y) // 화면을 꽉채울 정도로 올라갔다면 그만 올라가자
			finalposY = MAX_TEXTURE_POS_Y;

		if (FAILED(SetUp_ConstantTable(m_fTexturePosX + i * fSizeX, finalposY)))
			return E_FAIL;
		m_pVIBufferCom->Render(1); // ZIgnoreNAlphablending


		// 만약에 마지막 아이 또한 다 그렸을 경우. 레벨 매니져의 현재 레벨(CLevel_Logo)한테 다음 레벨로 넘어가라고 하자
		if (i == 10 && finalposY <= MAX_TEXTURE_POS_Y)
		{
			CLevel_Manager::GetInstance()->Get_CurrentLevel()->m_bGoToThe_NextLevel = true;
			m_bWillDead = true;
		}
	}

	return S_OK;
}

HRESULT CSceneChangeEffect1::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{

#ifdef USE_IMGUI
	ImGui::Begin("CSceneChangeEffect");
	{
		if (ImGui::DragFloat("Position", &m_fTexturePosY));
	}
	ImGui::End();
#endif
	return S_OK;
}

HRESULT CSceneChangeEffect1::SetUp_Component()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)m_pRendererCom.GetAddressOf())))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)m_pVIBufferCom.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Texture*/ 
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MenuTransSlice1"), TEXT("Com_Texture"), (CComponent**)m_pTextureCom.GetAddressOf())))
		return E_FAIL; 

	return S_OK;
}

HRESULT CSceneChangeEffect1::SetUp_ConstantTable(_float positionX, _float positionY)
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	// postion은 중심점위치입니다.
	_float fPosX = positionX;
	_float fPosY = positionY;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = fSizeX;
	WorldMatrix._22 = fSizeY;
	WorldMatrix._41 = fPosX - (g_iWinCX >> 1);
	WorldMatrix._42 = -fPosY + (g_iWinCY >> 1);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pVIBufferCom.Get(), "g_DiffuseTexture")))
		return E_FAIL;	

	return S_OK;
}

CSceneChangeEffect1 * CSceneChangeEffect1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CSceneChangeEffect1*		pInstance = new CSceneChangeEffect1(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CSceneChangeEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSceneChangeEffect1::Clone(void* pArg)
{
	CSceneChangeEffect1*		pInstance = new CSceneChangeEffect1(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CSceneChangeEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSceneChangeEffect1::Free()
{

	__super::Free();
}






// ----------------------------------------------------------
// CSCeneCahgneEffect2
CSceneChangeEffect2::CSceneChangeEffect2(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSceneChangeEffect2::CSceneChangeEffect2(const CSceneChangeEffect2& rhs)
	: CGameObject(rhs),
	m_ProjMatrix(rhs.m_ProjMatrix),
	m_ViewMatrix(rhs.m_ViewMatrix),
	m_pTextureCom(rhs.m_pTextureCom),
	m_pRendererCom(rhs.m_pRendererCom),
	m_pVIBufferCom(rhs.m_pVIBufferCom)
{
}

HRESULT CSceneChangeEffect2::NativeConstruct_Prototype()
{
	if (SetUp_Component())
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f)));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CSceneChangeEffect2::NativeConstruct(void* pArg)
{

	return S_OK;
}

_int CSceneChangeEffect2::Tick(_float fTimeDelta)
{
	if (m_isDead)
		return -1;

	if (m_eType == INCREASE)
		m_fTexturePosY -= 15;
	else
		m_fTexturePosY += 15;

	return _int();
}

_int CSceneChangeEffect2::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
		return 0;

	if (m_bWillDead) // 마지막 한번을 출력하고 죽자
		m_isDead = true;

	return _int();
}

HRESULT CSceneChangeEffect2::Render(_uint iPassIndex)
{
	if (m_eType == INCREASE)
	{
		for (int i = 0; i < 11; i++) // m_fTexturePosY는 감소한다
		{
			_float finalposY = m_fTexturePosY + 100.f * i;
			if (finalposY <= MAX_TEXTURE_POS_Y) // 화면을 꽉채울 정도로 올라갔다면 그만 올라가자
				finalposY = MAX_TEXTURE_POS_Y;

			if (FAILED(SetUp_ConstantTable(m_fTexturePosX + i * fSizeX, finalposY)))
				return E_FAIL;
			m_pVIBufferCom->Render(1); // ZIgnoreNAlphablending

			// 만약에 마지막 아이 또한 다 그렸을 경우. 다시 끝에서 부터 내려가자
			if (i == 10 && finalposY <= MAX_TEXTURE_POS_Y)
			{
				m_eType = DESCENT;
				m_fTexturePosY = MAX_TEXTURE_POS_Y;
			}
		}
	}
	else if (m_eType == DESCENT) // m_fTexturePosY는 증가한다
	{
		for (int i = 0; i < 11; i++)
		{
			_float finalposY = m_fTexturePosY - 100.f*i;
			if (finalposY <= MAX_TEXTURE_POS_Y) // 화면을 꽉채울 정도로 올라갔다면 그만 올라가자
				finalposY = MAX_TEXTURE_POS_Y;

			if (FAILED(SetUp_ConstantTable(m_fTexturePosX + i * fSizeX, finalposY)))
				return E_FAIL;
			m_pVIBufferCom->Render(1); // ZIgnoreNAlphablending

			// 마지막 아이까지 다 내렸을 경우. 죽자.
			if (i == 10 && (finalposY >= fSizeY / 2.f + 950.f))
				m_bWillDead = true;
		}
	}

	return S_OK;
}

HRESULT CSceneChangeEffect2::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{

#ifdef USE_IMGUI
	ImGui::Begin("CSceneChangeEffect");
	{
		if (ImGui::DragFloat("Position", &m_fTexturePosY));
	}
	ImGui::End();
#endif
	return S_OK;
}

HRESULT CSceneChangeEffect2::SetUp_Component()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)m_pRendererCom.GetAddressOf())))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)m_pVIBufferCom.GetAddressOf())))
		return E_FAIL;

	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MenuTransSlice1"), TEXT("Com_Texture"), (CComponent**)m_pTextureCom.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}

HRESULT CSceneChangeEffect2::SetUp_ConstantTable(_float positionX, _float positionY)
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	// postion은 중심점위치입니다.
	_float fPosX = positionX;
	_float fPosY = positionY;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = fSizeX;
	WorldMatrix._22 = fSizeY;
	WorldMatrix._41 = fPosX - (g_iWinCX >> 1);
	WorldMatrix._42 = -fPosY + (g_iWinCY >> 1);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pVIBufferCom.Get(), "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

CSceneChangeEffect2* CSceneChangeEffect2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CSceneChangeEffect2* pInstance = new CSceneChangeEffect2(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CSceneChangeEffect2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSceneChangeEffect2::Clone(void* pArg)
{
	CSceneChangeEffect2* pInstance = new CSceneChangeEffect2(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CSceneChangeEffect2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSceneChangeEffect2::Free()
{

	__super::Free();
}