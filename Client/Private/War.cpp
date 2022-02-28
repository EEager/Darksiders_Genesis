#include "stdafx.h"
#include "..\public\War.h"

#include "GameInstance.h"


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
	if (SetUp_Component())
		return E_FAIL;	

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(rand() % 10, 0.f, rand() % 10, 1.f));

	int randAnimationIdx = rand() % 3;

	for (int i = 0; i < MODELTYPE_END; i++)
	{
		m_pModelCom[i]->SetUp_Animation(randAnimationIdx);
	}

	return S_OK;
}

_int CWar::Tick(_float fTimeDelta)
{
	/*if (GetKeyState(VK_UP) & 0x8000)
	{
		m_pModelCom->SetUp_Animation(1);
	}

	if (GetKeyState(VK_DOWN) & 0x8000)
	{
		m_pModelCom->SetUp_Animation(0);
	}*/

	for (int i = 0; i < MODELTYPE_END; i++)
		m_pModelCom[i]->Update_Animation(fTimeDelta);

	return _int();
}

_int CWar::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA_WAR, this)))
		return 0;

	return _int();
}

HRESULT CWar::Render()
{
	// --------------------------
	// 1. War 원형 렌더하면서, 스텐실 버퍼에 1로 채운다. 
	// --------------------------
	m_pDeviceContext->OMSetDepthStencilState(RenderStates::MarkMirrorDSS.Get(), 1);


	/* 장치에 월드변환 행렬을 저장한다. */
	for (int modelIdx = 0; modelIdx < MODELTYPE_END; modelIdx++)
	{
		if (FAILED(SetUp_ConstantTable(false, modelIdx)))
			return E_FAIL;

		// iNumMaterials : 망토, 몸통
		_uint	iNumMaterials = m_pModelCom[modelIdx]->Get_NumMaterials(); 
		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelCom[modelIdx]->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE); 
			m_pModelCom[modelIdx]->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
			m_pModelCom[modelIdx]->Set_ShaderResourceView("g_EmissiveTexture", i, aiTextureType_EMISSIVE);

			m_pModelCom[modelIdx]->Render(i, 0);
		}
	}

	// restore default states, as the Shader_AnimMesh.hlsl changes them in the effect file.
	m_pDeviceContext->RSSetState(0);
	m_pDeviceContext->OMSetDepthStencilState(0, 0);
	//m_pDeviceContext->OMSetBlendState(0, 0, 0xffffffff);


	// --------------------------
	// 2. (스텐실버퍼가 0인경우에) War 외곽선 Draw, Draw 순서는 : 지형->NONALPHA->War 순
	// --------------------------
	m_pDeviceContext->OMSetDepthStencilState(RenderStates::DrawReflectionDSS.Get(), 0);

	for (int modelIdx = 0; modelIdx < MODELTYPE_END; modelIdx++)
	{
		if (FAILED(SetUp_ConstantTable(true, modelIdx)))
			return E_FAIL;

		_uint	iNumMaterials = m_pModelCom[modelIdx]->Get_NumMaterials();
		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelCom[modelIdx]->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
			m_pModelCom[modelIdx]->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
			m_pModelCom[modelIdx]->Set_ShaderResourceView("g_EmissiveTexture", i, aiTextureType_EMISSIVE);


			m_pModelCom[modelIdx]->Render(i, 0);
		}
	}

	// Restore default states
	m_pDeviceContext->RSSetState(0);
	m_pDeviceContext->OMSetDepthStencilState(0, 0);


	return S_OK;
}

HRESULT CWar::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model_War */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War"), TEXT("Com_Model_War"), (CComponent**)&m_pModelCom[MODELTYPE_WAR])))
		return E_FAIL;
	/* For.Com_Model_War_Gauntlet */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War_Gauntlet"), TEXT("Com_Model_War_Gauntlet"), (CComponent**)&m_pModelCom[MODELTYPE_GAUNTLET])))
		return E_FAIL;
	/* For.Prototype_Component_Model_War_Weapon */
	// Model_War가 갖고 있는 뼈중 이름이 "Bone_War_Weapon_Sword" 인것을 찾아 넣어주자.
	CHierarchyNode* pWeaponBone = m_pModelCom[MODELTYPE_WAR]->Find_HierarchyNode("Bone_War_Weapon_Sword");
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War_Weapon"), TEXT("Com_Model_War_Weapon"), (CComponent**)&m_pModelCom[MODELTYPE_WEAPON], pWeaponBone)))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CWar::SetUp_ConstantTable(bool drawOutLine, int modelIdx)
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
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom[modelIdx], "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom[modelIdx], "g_ProjMatrix");

	// Bind Position
	_float4			vCamPosition;
	XMStoreFloat4(&vCamPosition, pGameInstance->Get_CamPosition());
	m_pModelCom[modelIdx]->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));

	// Branch to Use Normal Mapping 
	// 노멀맵할지 말지 선택을 여기서 하자
	m_pModelCom[modelIdx]->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));
	m_pModelCom[modelIdx]->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));

	// Outline 원형은 그리지않는다.
	m_pModelCom[modelIdx]->Set_RawValue("g_DrawOutLine", &drawOutLine, sizeof(bool));


#if 0 // Legacy
	LIGHTDESC		LightDesc = *pGameInstance->Get_LightDesc(0);
	m_pVIBufferCom->Set_RawValue("g_vLightDir", &_float4(LightDesc.vDirection, 0.f), sizeof(_float4));
	m_pVIBufferCom->Set_RawValue("g_vLightDiffuse", &LightDesc.vDiffuse, sizeof(_float4));
	m_pVIBufferCom->Set_RawValue("g_vLightAmbient", &LightDesc.vAmbient, sizeof(_float4));
	m_pVIBufferCom->Set_RawValue("g_vLightSpecular", &LightDesc.vSpecular, sizeof(_float4));

	_float4			vCamPosition;	
	XMStoreFloat4(&vCamPosition, pGameInstance->Get_CamPosition());
	m_pVIBufferCom->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));
#endif
	

	RELEASE_INSTANCE(CGameInstance);

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


	Safe_Release(m_pTransformCom);	
	Safe_Release(m_pRendererCom);

	for (auto& modelCom : m_pModelCom)
		Safe_Release(modelCom);
}
