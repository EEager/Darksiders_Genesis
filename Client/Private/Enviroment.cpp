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

	//// Material Init
	//m_tMtrlDesc.vMtrlDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	//m_tMtrlDesc.vMtrlAmbient = { 0.5f, 0.5f, 0.5f, 1.0f };
	//m_tMtrlDesc.vMtrlSpecular = { 0.2f, 0.2f, 0.2f, 1.0f };
	//m_tMtrlDesc.vMtrlEmissive = { 1.f, 1.f, 1.f, 1.f };
	//m_tMtrlDesc.fMtrlPower = 20.f;

	return S_OK;
}


HRESULT CEnviroment::NativeConstruct(void * pArg)
{
	if (pArg) // �� Com ��ü�� ������Ÿ���� Ŭ�н� ���Ӱ� �޾ƿ���
		wsprintfW(m_szModelName, L"%s", pArg);

	if (SetUp_Component())
		return E_FAIL;


	// Enviroment ���� ��� �������� �Ǵ� �޽��� �����Ѵµ� �ű⿡ ���� �ʱ� ��ġ�� ��������
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

	float fRadian = XMVectorGetX(XMVector3Length(XMLoadFloat3(&static_cast<CModel*>(m_pModelCom)->Get_Extents()))) / 2.f;

	if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), fRadian))
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			goto _EXIT;

		if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
			goto _EXIT;
	}
_EXIT:
	RELEASE_INSTANCE(CGameInstance);


	return _int();
}

HRESULT CEnviroment::Render(_uint iPassIndex)
{
	if (FAILED(SetUp_ConstantTable(iPassIndex)))
		return E_FAIL;

	/* ��ġ�� ���庯ȯ ����� �����Ѵ�. */
	_uint	iNumMeshContainer = m_pModelCom->Get_NumMeshContainer();
	
	for (_uint i = 0; i < iNumMeshContainer; ++i)
	{
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
		m_pModelCom->Set_ShaderResourceView("g_EmissiveTexture", i, aiTextureType_EMISSIVE);


		m_pModelCom->Render(i, iPassIndex); 
	}

	m_pDeviceContext->RSSetState(0);
	m_pDeviceContext->OMSetDepthStencilState(0, 0);


	return S_OK;
}

HRESULT CEnviroment::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
#ifdef _DEBUG
	m_pAABBCom->Render();
#endif // _DEBUG

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
	ColliderDesc.eColType = CCollider::COL_TYPE_AABB;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_AABB"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnviroment::SetUp_ConstantTable(_uint iPassIndex)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Bind Transform
	if (iPassIndex == 3) // ShadowMap
	{
		m_pTransformCom->Bind_OnShader(m_pModelCom, "g_WorldMatrix");
		m_pModelCom->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&CLight_Manager::GetInstance()->m_LightView)), sizeof(_float4x4));
		m_pModelCom->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&CLight_Manager::GetInstance()->m_LightProj)), sizeof(_float4x4));
	}
	else
	{
		m_pTransformCom->Bind_OnShader(m_pModelCom, "g_WorldMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom, "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom, "g_ProjMatrix");
	}

	// ��ָ����� ���� ������ ���⼭ ����
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
		MSG_BOX("Failed to Clone CEnviroment");
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
