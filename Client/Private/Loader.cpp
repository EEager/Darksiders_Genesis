#include "stdafx.h"
#include "..\public\Loader.h"

#include "GameInstance.h"

// -------------------
// GameObject Headers
// -------------------
#include "Terrain.h"
#include "Fork.h"
#include "Player.h"
#include "War.h"


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


	wsprintf(m_szLoading, TEXT("LEVEL_LOGO 로딩이 완료되었습니다. "));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* 게임플레이 레벨에 필요한 객체들의 원형을 생성한다. */

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
	
	// -----------------
	// Model_War
	// -----------------
	/* For.Prototype_GameObject_War */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_War"),
		CWar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	///* For.Prototype_GameObject_War */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_War_Weapon"),
	//	CWar::Create(m_pDevice, m_pDeviceContext))))
	//	return E_FAIL;



	// =============================================================================================================

	/* 게임플레이 레벨에 필요한 컴포넌트들의 원형을 생성한다.  */
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"), 
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Terrain_Light.hlsl"), TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

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


	_matrix		PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	/* For.Prototype_Component_Model_Player , Fiona */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh.hlsl"), "../Bin/Resources/Meshes/Fiona/", "Fiona.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Fork*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fork"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "../Bin/Resources/Meshes/ForkLift/", "ForkLift.fbx", PivotMatrix))))
		return E_FAIL;

	// -----------------
	// Model_War
	// -----------------
	/* For.Prototype_Component_Model_War */
	_matrix		War_PivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	// War
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Heroes/Hero_War/War/", "War.fbx", War_PivotMat))))
		return E_FAIL;
	/* Prototype_Component_Model_War_Gauntlet */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War_Gauntlet"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Heroes/Hero_War/War_Gauntlet/", "War_Gauntlet.fbx", War_PivotMat))))
		return E_FAIL;
	/* Prototype_Component_Model_War_Weapon */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War_Weapon"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, TEXT("../Bin/ShaderFiles/Shader_AnimMesh_Normal.hlsl"), "../Bin/Resources/Meshes/Characters/Heroes/Hero_War/War_Weapon/", "War_Weapon.fbx", War_PivotMat))))
		return E_FAIL;



	wsprintf(m_szLoading, TEXT("LEVEL_GAMEPLAY 로딩이 완료되었습니다. "));


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
