#include "stdafx.h"
#include "..\public\Enviroment.h"

#include "GameInstance.h"


CEnviroment::CEnviroment(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CEnviroment::CEnviroment(const CEnviroment & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnviroment::NativeConstruct_Prototype()
{	

	// Material Init
	m_tMtrlDesc.vMtrlDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_tMtrlDesc.vMtrlAmbient = { 0.5f, 0.5f, 0.5f, 1.0f };
	m_tMtrlDesc.vMtrlSpecular = { 0.2f, 0.2f, 0.2f, 1.0f };
	m_tMtrlDesc.vMtrlEmissive = { 1.f, 1.f, 1.f, 1.f };
	m_tMtrlDesc.fMtrlPower = 20.f;

	return S_OK;
}


HRESULT CEnviroment::NativeConstruct(void * pArg)
{
	if (pArg) // 모델 Com 객체의 프로토타입을 클론시 새롭게 받아오자
		wsprintfW(m_szModelName, L"%s", pArg);

	if (SetUp_Component())
		return E_FAIL;


	// Enviroment 모델의 경우 기준점이 되는 메쉬가 존재한는데 거기에 맞춰 초기 위치를 설정하자
	_float3 EnviromentBase = static_cast<CModel*>(m_pModelCom)->m_vEnviromentBase;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-EnviromentBase.x + ENVIROMENT_OFFSET_X, -EnviromentBase.y, -EnviromentBase.z + ENVIROMENT_OFFSET_Z, 1.f));

	return S_OK;
}

_int CEnviroment::Tick(_float fTimeDelta)
{
	m_pAABBCom->Update(m_pTransformCom->Get_WorldMatrix());

	return _int();
}

_int CEnviroment::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA_TERRAIN, this)))
		goto _EXIT;
_EXIT:
	RELEASE_INSTANCE(CGameInstance);


	return _int();
}

HRESULT CEnviroment::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;
	
	/* 장치에 월드변환 행렬을 저장한다. */
	_uint	iNumMeshContainer = m_pModelCom->Get_NumMeshContainer();
	
	for (_uint i = 0; i < iNumMeshContainer; ++i)
	{
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
		m_pModelCom->Set_ShaderResourceView("g_EmissiveTexture", i, aiTextureType_EMISSIVE);

		m_pModelCom->Render(i, 0);
	}

#ifdef _DEBUG
	m_pAABBCom->Render();
#endif // _DEBUG



	// restore default states, as the SkyFX changes them in the effect file.
	m_pDeviceContext->RSSetState(0);
	m_pDeviceContext->OMSetDepthStencilState(0, 0);


	return S_OK;
}

HRESULT CEnviroment::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_szModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_AABB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ColliderDesc.vPivot = static_cast<CModel*>(m_pModelCom)->Get_Center();
	ColliderDesc.vSize = static_cast<CModel*>(m_pModelCom)->Get_Extents();
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnviroment::SetUp_ConstantTable()
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
	m_pModelCom->Set_RawValue("g_DirLight", &mDirLight, sizeof(DirectionalLight));

	// Bind Material
	m_pModelCom->Set_RawValue("g_Material", &m_tMtrlDesc, sizeof(MTRLDESC));

	// Bind Transform
	m_pTransformCom->Bind_OnShader(m_pModelCom, "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom, "g_ProjMatrix");

	// Bind Position
	_float4			vCamPosition;
	XMStoreFloat4(&vCamPosition, pGameInstance->Get_CamPosition());
	m_pModelCom->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));

	// 노멀맵할지 말지 선택을 여기서 하자
	m_pModelCom->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));
	m_pModelCom->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CEnviroment * CEnviroment::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CEnviroment*		pInstance = new CEnviroment(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CEnviroment");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CEnviroment::Clone(void* pArg)
{
	CEnviroment*		pInstance = new CEnviroment(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CEnviroment");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnviroment::Free()
{

	__super::Free();

	Safe_Release(m_pAABBCom);
	Safe_Release(m_pTransformCom);	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
