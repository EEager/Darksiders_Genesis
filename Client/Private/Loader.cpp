#include "stdafx.h"
#include "..\public\Loader.h"

#include "GameInstance.h"
#include "Texture.h"

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
#include "Sky.h"
#include "Enviroment.h"

// Monster
#include "Monster\Monster.h"
#include "Monster\Legion.h"
#include "Monster\Goblin_Armor.h"
#include "Monster\FallenDog.h"
#include "Monster\HollowLord.h"

// Object
#include "MapObject\Ballista.h"
#include "MapObject\Breakables.h"
#include "MapObject\SoulBarrier.h"
#include "MapObject\BrokenCorner.h"
#include "MapObject\SpikeGate.h"
#include "MapObject\Flag.h"
#include "MapObject\Trees.h"

// Effect
#include "Effect\RectEffect.h"
#include "Effect\PointEffect.h"
#include "Effect\FireEffect.h"




CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

}

unsigned int APIENTRY EntryMain(void* pArg)
{
	CLoader* pLoader = (CLoader*)pArg;

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
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(Add_GameObject()))
		return E_FAIL;
	if (FAILED(Add_VIBuffer()))
		return E_FAIL;
	if (FAILED(Add_Texture()))
		return E_FAIL;
	if (FAILED(Add_Model()))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Data/NavigationData.dat")))))
		return E_FAIL;

	wsprintf(m_szLoading, TEXT("LEVEL_GAMEPLAY Load Completed!"));

	m_isFinished = true;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Add_GameObject()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* GameObjects */
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

	/* For.Prototype_GameObject_FireEffect */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireEffect"),
		CFireEffect::Create(m_pDevice, m_pDeviceContext))))
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

	// UIs
	/* For.Prototype_GameObject_UI_War_Hp_n_Wrath_Bar */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_War_Hp_n_Wrath_Bar"),
		CUI_War_Hp_n_Wrath_Bar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_War_Skills */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_War_Skills"),
		CUI_War_Skills::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	// Monsters
	/* For.Prototype_GameObject_Legion */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Legion"),
		CLegion::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Goblin_Armor */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Goblin_Armor"),
		CGoblin_Armor::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Goblin_Spear */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Goblin_Spear"),
		CGoblin_Spear::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_FallenDog */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FallenDog"),
		CFallenDog::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_HollowLord */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HollowLord"),
		CHollowLord::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	// Map Objects
	/* For.Prototype_GameObject_Ballista */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ballista"),
		CBallista::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Ballista_Bolt */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ballista_Bolt"),
		CBallista_Bolt::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	// CSoulBarrier
	/* For.Prototype_GameObject_SoulBarrier */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SoulBarrier"),
		CSoulBarrier::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	// Breakables
	/* For.Prototype_GameObject_Breakable1 */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Breakable1"),
		CBreakable1::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Breakable2 */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Breakable2"),
		CBreakable2::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Breakable3 */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Breakable3"),
		CBreakable3::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Breakable4 */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Breakable4"),
		CBreakable4::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Breakable5 */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Breakable5"),
		CBreakable5::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	// Trees

	// ▼ STATIC Meshes Trees
	/* For.Prototype_Component_Model_TreeA */
	_matrix		Trees_PivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TreeA"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Objects/Trees/", "TreeA.fbx", Trees_PivotMat))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TreeB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TreeB"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Objects/Trees/", "TreeB.fbx", Trees_PivotMat))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TreeC */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TreeC"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Objects/Trees/", "TreeC.fbx", Trees_PivotMat))))
		return E_FAIL;

	/* For.Prototype_GameObject_TreeA */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TreeA"),
		CTreeA::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_TreeB */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TreeB"),
		CTreeB::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_TreeC */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TreeC"),
		CTreeC::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	// ▼ Dynamic
	// CBrokenCorner
	/* For.Prototype_GameObject_BrokenCorner */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BrokenCorner"),
		CBrokenCorner::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BrokenCorner */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpikeGate"),
		CSpikeGate::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Flag_A */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Flag_A"),
		CFlag_A::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLoader::Add_VIBuffer()
{
	/* Componenets : VIBuffer */
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	wsprintf(m_szLoading, TEXT("Loading Components VIBuffer"));
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

	/* For.Prototype_Component_VIBuffer_MonsterHp_PointGS */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_MonsterHp_PointGS"),
		CVIBuffer_MonsterHp_PointGS::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_MonsterHpBar_PointGs.hlsl")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Trail*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Trail.hlsl")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Sphere */
	// 이거 필요하면 CCell::Ready_DebugBuffer() 참조

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLoader::Add_Texture()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* Componenets : Textures */
	wsprintf(m_szLoading, TEXT("Loading Components Textures"));
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
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
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/SkyBox/SkyBox%d.dds"), 4))))
		return E_FAIL;


	// GamePlay War Hp and Wrath UI Bar
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

	// GamePlay War Skill UI
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

	// Boss UI 
	/* For.Prototype_Component_Texture_UI_CreatureCore_Portraits_hollowLord_major */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_CreatureCore_Portraits_hollowLord_major"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_CreatureCore_Portraits_hollowLord_major.dds")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_HUD_BossHealthFrame1 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_HUD_BossHealthFrame1"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD_BossHealthFrame1.dds")))))
		return E_FAIL;

	// Effects

	// Fire
	{
		/* For.Prototype_Component_Texture_fire */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_fire01"),
			CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/VFX/Fire/fire01.dds")))))
			return E_FAIL;

		/* For.Prototype_Component_Texture_noise */
		auto pTexture = CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/VFX/Fire/noise0%d.dds"), 4);
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_noise"), pTexture)))
			return E_FAIL;

		// Renderer에게 noise를 쥐어주자.
		CRenderer* pRenderer = (CRenderer*)CComponent_Manager::GetInstance()->Find_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"));
		pRenderer->m_pTextureNoise = pTexture;
		Safe_AddRef(pTexture);

		/* For.Prototype_Component_Texture_alpha */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_alpha"),
			CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/VFX/Fire/alpha0%d.dds"), 2))))
			return E_FAIL;
	}


	/* For.Prototype_Component_Texture_Trail */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Trail"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/VFX/Trail/trail%d.dds"), 6))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;

}

HRESULT CLoader::Add_Model()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// ▼ Enviroment Models
	_matrix		EnviromentPivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));


#if 0 // JJLEE 잠시만 막아두자
	/* For.Prototype_Component_Model_Enviroment4*/
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Enviroment4"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enviroment4"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Enviroment/Enviroment4/", "Enviroment4.fbx", EnviromentPivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Enviroment3_1*/
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Enviroment3_1"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enviroment3_1"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Enviroment/Enviroment3/", "Enviroment3_1.fbx", EnviromentPivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Enviroment3_2*/
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Enviroment3_2"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enviroment3_2"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Enviroment/Enviroment3/", "Enviroment3_2.fbx", EnviromentPivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Enviroment3_3*/
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Enviroment3_3"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enviroment3_3"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Enviroment/Enviroment3/", "Enviroment3_3.fbx", EnviromentPivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Enviroment3_4*/
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Enviroment3_4"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enviroment3_4"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Enviroment/Enviroment3/", "Enviroment3_4.fbx", EnviromentPivotMatrix))))
		return E_FAIL;


	/* For.Prototype_Component_Model_Enviroment2*/
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Enviroment2"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enviroment2"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Enviroment/Enviroment2/", "Enviroment2.fbx", EnviromentPivotMatrix))))
		return E_FAIL;
#endif

	/* For.Prototype_Component_Model_Enviroment1*/
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Enviroment1"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enviroment1"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Enviroment/Enviroment1/", "Enviroment1.fbx", EnviromentPivotMatrix))))
		return E_FAIL;

#if 0 // 처음 나오는 풀들인데 사용안할것이다
	///* For.Prototype_Component_Model_Foliage...*/
	//wsprintf(m_szLoading, TEXT("Loading Component_Model_Foliage0"));
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Foliage0"),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Enviroment/Foliage/", "Foliage0.fbx", EnviromentPivotMatrix))))
	//	return E_FAIL;
#endif

	wsprintf(m_szLoading, TEXT("Loading Component_Model_Foliage1"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Foliage1"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Enviroment/Foliage/", "Foliage1.fbx", EnviromentPivotMatrix))))
		return E_FAIL;
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Foliage2"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Foliage2"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Enviroment/Foliage/", "Foliage2.fbx", EnviromentPivotMatrix))))
		return E_FAIL;
#if 0 // JJLEE 잠시만 막아두자

	wsprintf(m_szLoading, TEXT("Loading Component_Model_Foliage3"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Foliage3"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Enviroment/Foliage/", "Foliage3.fbx", EnviromentPivotMatrix))))
		return E_FAIL;
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Foliage4"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Foliage4"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Enviroment/Foliage/", "Foliage4.fbx", EnviromentPivotMatrix))))
		return E_FAIL;
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Foliage5"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Foliage5"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Enviroment/Foliage/", "Foliage5.fbx", EnviromentPivotMatrix))))
		return E_FAIL;
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Foliage6"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Foliage6"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Enviroment/Foliage/", "Foliage6.fbx", EnviromentPivotMatrix))))
		return E_FAIL;
#endif 


	// ▼ Test
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Fiona"));
	_matrix		PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	/* For.Prototype_Component_Model_Player , Fiona */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Fiona/", "Fiona.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Fork*/
	wsprintf(m_szLoading, TEXT("Loading Prototype_Component_Model_Fork"));
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fork"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/ForkLift/", "ForkLift.fbx", PivotMatrix))))

	// ▼ War
	/* For.Prototype_Component_Model_War */
	wsprintf(m_szLoading, TEXT("Loading Prototype_Component_Model_War"));
	_matrix		War_PivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Heroes/Hero_War/War/", "War.fbx", War_PivotMat))))
		return E_FAIL;

	/* Prototype_Component_Model_War_Gauntlet */
	wsprintf(m_szLoading, TEXT("Loading Prototype_Component_Model_War_Gauntlet"));
	//애니메이션은 없지만, 다른 모델것을 사용하고 싶다. +뼈는 있다. War_Gauntlet의 애니메이션 행렬은 Model_War를 따라간다
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War_Gauntlet"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM_USE_OTHER, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Heroes/Hero_War/War_Gauntlet/", "War_Gauntlet.fbx", War_PivotMat))))
		return E_FAIL;

	/* Prototype_Component_Model_War_Ruin */
	wsprintf(m_szLoading, TEXT("Loading Prototype_Component_Model_War_Ruin"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War_Ruin"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Heroes/Hero_War/War_Ruin/", "War_Ruin.fbx", War_PivotMat))))
		return E_FAIL;

	/* Prototype_Component_Model_War_Weapon */
	wsprintf(m_szLoading, TEXT("Loading Prototype_Component_Model_War_Weapon"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War_Weapon"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Heroes/Hero_War/War_Weapon/", "War_Weapon.fbx", War_PivotMat))))
		return E_FAIL;

	// ▼ Monsters
	/* For.Prototype_Component_Model_Legion*/
	_matrix		Legion_PivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	wsprintf(m_szLoading, TEXT("Loading Prototype_Component_Model_Legion"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legion"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Creatures/Legion/Legion/", "Legion.fbx", Legion_PivotMat))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Legion_Axe_L*/
	Legion_PivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	wsprintf(m_szLoading, TEXT("Loading Prototype_Component_Model_Legion_Axe_L"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legion_Axe_L"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Creatures/Legion/Legion/Legion_Axe/", "Legion_Axe_L.fbx", Legion_PivotMat))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Legion_Axe_R*/
	Legion_PivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	wsprintf(m_szLoading, TEXT("Loading Prototype_Component_Model_Legion_Axe_R"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legion_Axe_R"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Creatures/Legion/Legion/Legion_Axe/", "Legion_Axe_R.fbx", Legion_PivotMat))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Goblin_Armor*/
	_matrix		Goblin_PivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	wsprintf(m_szLoading, TEXT("Loading Prototype_Component_Model_Goblin_Armor"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Goblin_Armor"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Creatures/Goblin/Goblin_Armor/", "Goblin_Armor.fbx", Goblin_PivotMat))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Goblin_Spear*/
	wsprintf(m_szLoading, TEXT("Loading Prototype_Component_Model_Goblin_Spear"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Goblin_Spear"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Creatures/Goblin/Goblin_Spear/", "Goblin_Spear.fbx", Goblin_PivotMat))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Goblin_Quiver*/
	wsprintf(m_szLoading, TEXT("Loading Prototype_Component_Model_Goblin_Quiver"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Goblin_Quiver"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Creatures/Goblin/Goblin_Quiver/", "Goblin_Quiver.fbx", Goblin_PivotMat))))
		return E_FAIL;


	/* For.Prototype_Component_Model_FallenDog*/
	_matrix		FallenDog_PivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	wsprintf(m_szLoading, TEXT("Loading Prototype_Component_Model_FallenDog"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FallenDog"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Creatures/FallenDog/", "FallenDog.fbx", FallenDog_PivotMat))))
		return E_FAIL;

	// ▼ Boss
	/* For.Prototype_Component_Model_HollowLord*/
	_matrix		HollowLord_PivotMat = XMMatrixScaling(0.02f, 0.02f, 0.02f);
	wsprintf(m_szLoading, TEXT("Loading Prototype_Component_Model_HollowLord"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HollowLord"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Creatures/HollowLord/", "HollowLord.fbx", HollowLord_PivotMat))))
		return E_FAIL;


	// ▼ Map Objects Models
	/* For.Prototype_Component_Model_Ballista*/
	_matrix		Ballista_PivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Ballista"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Ballista"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Objects/Ballista/", "Ballista.fbx", Ballista_PivotMat))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Ballista_Destroyed*/
	wsprintf(m_szLoading, TEXT("Loading Prototype_Component_Model_Ballista_Destroyed"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Ballista_Destroyed"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Objects/Ballista/", "Ballista_Destroyed.fbx", Ballista_PivotMat))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Ballista_Bolt*/
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Ballista_Bolt"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Ballista_Bolt"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Objects/Ballista/Ballista_Bolt/", "Ballista_Bolt.fbx", Ballista_PivotMat))))
		return E_FAIL;

	// SoulBarrier
	/* For.Prototype_Component_Model_SoulBarrier */
	_matrix		SoulBarrier_PivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	wsprintf(m_szLoading, TEXT("Loading Component_Model_SoulBarrier"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SoulBarrier"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Objects/SoulBarrier/", "SoulBarrier.fbx", SoulBarrier_PivotMat))))
		return E_FAIL;

	// ▼ Breakables
	_matrix		Breakables_PivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	/* For.Prototype_Component_Model_Breakable1 => Barrel */
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Breakable1"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Breakable1"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Objects/Breakable/", "Barrel.fbx", Breakables_PivotMat))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Breakable2 => Wood_Barrel */
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Breakable2"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Breakable2"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Objects/Breakable/", "Wood_Barrel.fbx", Breakables_PivotMat))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Breakable3 => Barricade_A */
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Breakable3"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Breakable3"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Objects/Breakable/", "Barricade_A.fbx", Breakables_PivotMat))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Breakable4 => Barricade_B */
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Breakable4"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Breakable4"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Objects/Breakable/", "Barricade_B.fbx", Breakables_PivotMat))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Breakable5 => Barricade_C */
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Breakable5"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Breakable5"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Objects/Breakable/", "Barricade_C.fbx", Breakables_PivotMat))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Breakable6 => Barricade_D */
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Breakable6"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Breakable6"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Objects/Breakable/", "Barricade_C.fbx", Breakables_PivotMat))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Breakable7 => Barricade_E */
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Breakable7"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Breakable7"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Objects/Breakable/", "Barricade_E.fbx", Breakables_PivotMat))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Breakable8 => Barricade_F */
	wsprintf(m_szLoading, TEXT("Loading Component_Model_Breakable8"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Breakable8"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh_Normal.hlsl"), "../Bin/Resources/Meshes/Objects/Breakable/", "Barricade_F.fbx", Breakables_PivotMat))))
		return E_FAIL;

	// ▼ Dynamic
	// BrokenCorner
	/* For.Prototype_Component_Model_BrokenCorner */
	_matrix		Dynamic_PivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	wsprintf(m_szLoading, TEXT("Loading Component_Model_BrokenCorner"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BrokenCorner"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Objects/BrokenCorner/", "BrokenCorner.fbx", Dynamic_PivotMat))))
		return E_FAIL;

	/* For.Prototype_Component_Model_SpikeGate */
	wsprintf(m_szLoading, TEXT("Loading Component_Model_SpikeGate"));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpikeGate"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Objects/SpikeGate/", "SpikeGate.fbx", Dynamic_PivotMat))))
		return E_FAIL;

	///* For.Prototype_Component_Model_Flag_A */
	//wsprintf(m_szLoading, TEXT("Loading Component_Model_Flag_A"));
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Flag_A"),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Objects/Flag/", "Flag_A.fbx", Dynamic_PivotMat))))
	//	return E_FAIL;






	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevel)
{
	CLoader* pInstance = new CLoader(pDevice, pDeviceContext);

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
