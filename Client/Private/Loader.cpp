#include "stdafx.h"
#include "..\public\Loader.h"

#include "GameInstance.h"

// -------------------
// GameObject Headers
// -------------------
#include "Terrain.h"
#include "Fork.h"
#include "Player.h"
#include "Sword.h"
#include "War.h"
#include "UI_War_Hp_n_Wrath_Bar.h"
#include "UI_War_Skills.h"
#include "RectEffect.h"
#include "Sky.h"
#include "PointEffect.h"
#include "Enviroment.h"


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

}

unsigned int APIENTRY EntryMain(void* pArg)
{
	CLoader*	pLoader = (CLoader*)pArg;

	EnterCriticalSection(pLoader->Get_CriticalSection());

	HRESULT			hr = 0;

	switch (pLoader->Get_NextLevel())
	{
	case LEVEL_LOGO:
		hr = pLoader->Loading_ForLogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = pLoader->Loading_ForGamePlayLevel();
		break;
	}

	LeaveCriticalSection(pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader::NativeConstruct(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_CS);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, EntryMain, this, 0, nullptr);


	return S_OK;
}

HRESULT CLoader::Loading_ForLogoLevel()
{
	/* 로고 레벨에 필요한 객체들의 원형을 생성한다. */


	wsprintf(m_szLoading, TEXT("LEVEL_LOGO Load Completed!"));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	// ===========================================================================
	/* GameObjects */
	// ===========================================================================
	wsprintf(m_szLoading, TEXT("Loading GameObjects"));

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Fork */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Fork"),
		CFork::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sword */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sword"),
		CSword::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_RectEffect */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RectEffect"),
		CRectEffect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_PointEffect */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PointEffect"),
		CPointEffect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_CEnviroment */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CEnviroment"),
		CEnviroment::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;



	/* For.Prototype_GameObject_War */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_War"),
		CWar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	// ---
	// UIs
	// ---
	/* For.Prototype_GameObject_UI_War_Hp_n_Wrath_Bar */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_War_Hp_n_Wrath_Bar"),
		CUI_War_Hp_n_Wrath_Bar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_War_Skills */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_War_Skills"),
		CUI_War_Skills::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	// ===========================================================================
	/* Componenets */
	// ===========================================================================
	wsprintf(m_szLoading, TEXT("Loading Components"));


	// [Auto Navi Gen 기능을 위한 절차]
	// 1. Enviroment Mesh를 Load할 때 Terrain 크기 만큼의 vector에 해당 정점들의 "최대" 높이를 저장해 놓는다
	// 2. 


	/* For.Prototype_Component_VIBuffer_Terrain */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"), 
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Terrain_Light.hlsl"), TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
	//	return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Terrain_Shadow.hlsl"), X_MAPSIZE, Z_MAPSIZE))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_RectInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_RectInstance"),
		CVIBuffer_RectInstance::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_RectInstance.hlsl"), 20))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Cube.hlsl")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_PointInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_PointInstance"),
		CVIBuffer_PointInstance::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_PointInstance.hlsl"), 20))))
		return E_FAIL;

	///* For.Prototype_Component_VIBuffer_Sphere */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Sphere"),
	//	CVIBuffer_Sphere::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Sphere.hlsl"), 10.f))))
	//	return E_FAIL;







	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"), 
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;	

	/* For.Prototype_Component_Texture_Black */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Black"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Default/Black_%d.tga"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Test.tga")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Brush */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/SkyBox/burger3.dds")))))
		return E_FAIL;


	// ---
	// GamePlay War Hp and Wrath UI Bar
	// ---
	/* For.Prototype_Component_Texture_UI_War_Base */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_War_Base"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_War_Base.dds")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_War_HpBar */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_War_HpBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_War_HpBar.dds")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_War_WrathBar */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_War_WrathBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_War_WrathBar.dds")))))
		return E_FAIL;

	// ---
	// GamePlay War Skill UI
	// ---
	/* For.Prototype_Component_Texture_UI_Skill_Base */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Skill_Base"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Skill_Base.dds")))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_UI_WrathPower_War */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_WrathPower_War"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_WrathPower_War.dds")))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_UI_Enhancement_War_Fire */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Enhancement_War_Fire"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Enhancement_War_Fire.dds")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_Enhancement_War_Leaf */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Enhancement_War_Leaf"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_Enhancement_War_Leaf.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UIButtonBase_1 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UIButtonBase_1"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UIButtonBase_1.dds")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UIButtonBase_G */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UIButtonBase_G"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UIButtonBase_G.dds")))))
		return E_FAIL;



	// ===========================================================================
	/* Component Models  */
	// ===========================================================================
#ifndef ONLY_WAR
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Fiona"));
	_matrix		PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	/* For.Prototype_Component_Model_Player , Fiona */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh.hlsl"), "../Bin/Resources/Meshes/Fiona/", "Fiona.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Fork*/
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Fork"));
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fork"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "../Bin/Resources/Meshes/ForkLift/", "ForkLift.fbx", PivotMatrix))))
		return E_FAIL;
#endif

	/* For.Prototype_Component_Model_War */
	wsprintf(m_szLoading, TEXT("Loading Component_Model_War"));
	_matrix		War_PivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Heroes/Hero_War/War/", "War.fbx", War_PivotMat))))
		return E_FAIL;

	/* Prototype_Component_Model_War_Gauntlet */  
	wsprintf(m_szLoading, TEXT("Loading Component_Model_War_Gauntlet"));
	//애니메이션은 없지만, 다른 모델것을 사용하고 싶다. +뼈는 있다. War_Gauntlet의 애니메이션 행렬은 Model_War를 따라간다
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War_Gauntlet"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM_USE_OTHER, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Heroes/Hero_War/War_Gauntlet/", "War_Gauntlet.fbx", War_PivotMat))))
		return E_FAIL;

	/* Prototype_Component_Model_War_Ruin */
	wsprintf(m_szLoading, TEXT("Loading Component_Model_War_Ruin"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War_Ruin"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Heroes/Hero_War/War_Ruin/", "War_Ruin.fbx", War_PivotMat))))
		return E_FAIL;

	/* Prototype_Component_Model_War_Weapon */ 
	wsprintf(m_szLoading, TEXT("Loading Component_Model_War_Weapon"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War_Weapon"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Heroes/Hero_War/War_Weapon/", "War_Weapon.fbx", War_PivotMat))))
		return E_FAIL;



	// 
	// Enviroment
	//
	/* For.Prototype_Component_Model_Enviroment1*/
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Enviroment1"));
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enviroment1"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Enviroment/Enviroment1/", "Enviroment1.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Enviroment2*/
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Enviroment2"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enviroment2"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Enviroment/Enviroment2/", "Enviroment2.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Enviroment3*/
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Enviroment3"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enviroment3"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Enviroment/Enviroment3/", "Enviroment3.fbx", PivotMatrix))))
		return E_FAIL;




	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Data/NavigationData.dat")))))
		return E_FAIL;

	/* For.Prototype_Component_Colllider_AABB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Colllider_OBB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	///* For.Prototype_Component_Collider_Sphere */ // -> Move to MainApp
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;




	wsprintf(m_szLoading, TEXT("LEVEL_GAMEPLAY Load Completed!"));

	m_isFinished = true;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevel)
{
	CLoader*		pInstance = new CLoader(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(eNextLevel)))
	{
		MSG_BOX("Failed to Created CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);
	
	DeleteCriticalSection(&m_CS);

	CloseHandle(m_hThread);

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
