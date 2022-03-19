#include "stdafx.h"
#include "..\public\Monster\Monster.h"

#include "GameInstance.h"


CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonster::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CMonster::NativeConstruct(void * pArg)
{
	// ��� ���ʹ� m_pTransformCom, m_pRendererCom, m_pNaviCom�� ������
	if (SetUp_Component())
		return E_FAIL;	

	return S_OK;
}

_int CMonster::Tick(_float fTimeDelta)
{
	// ��� ���ʹ� Collider list �� update�ؾ��Ѵ�
	Update_Colliders(m_pTransformCom->Get_WorldMatrix());

	return _int();
}

_int CMonster::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// ��� ���ʹ� Navi ���� Height�� Ÿ���Ѵ�
	_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float curFloorHeight = m_pNaviCom->Compute_Height(vPosition);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, curFloorHeight));

	// ��� ���ʹ� Nonalpha �׷쿡�� render�Ѵ�
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		return 0;

	// ��� ���ʹ� �ڱⰡ ������ �ִ� Collider list�� collider manager�� ����Ͽ� �浹ó���� �����Ѵ�
	pGameInstance->Add_Collision(this);

	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

HRESULT CMonster::Render()
{
	// ��� ���ʹ� �Ʒ��� ���� Render�� �����Ѵ�
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
		m_pModelCom->Set_ShaderResourceView("g_EmissiveTexture", i, aiTextureType_EMISSIVE);

		m_pModelCom->Render(i, 0);
	}

#ifdef _DEBUG
	// ��� ���ʹ� Collider�� render�Ѵ�
	__super::Render_Colliders();
#endif

	return S_OK;
}

HRESULT CMonster::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Navi */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navi"), (CComponent**)&m_pNaviCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::SetUp_ConstantTable()
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

	// Branch to Use Normal Mapping
	m_pModelCom->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));

	// Branch to Use Emissive Mapping
	m_pModelCom->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CMonster * CMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMonster*		pInstance = new CMonster(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMonster::Clone(void* pArg)
{
	CMonster*		pInstance = new CMonster(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster::Free()
{

	__super::Free();

	Safe_Release(m_pNaviCom);
	Safe_Release(m_pTransformCom);	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
