#pragma once

// ----------------------
// Components Headers
// ----------------------
#include "Model.h"
#include "Texture.h"
#include "Renderer.h"
#include "Collider.h"
#include "RendererStates.h"
#include "Transform.h"
#include "Navigation.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "StateMachine.h"
#include "VIBuffer_RectInstance.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_PointInstance.h"
#include "VIBuffer_Sphere.h"
#include "VIBuffer_MonsterHp_PointGS.h"
#include "VIBuffer_Trail.h"

/* 컴포넌트들의 원형을 모아둔다. */

BEGIN(Engine)

class ENGINE_DLL CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;
public:
	HRESULT Reserve_Container(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);
	HRESULT Clear(_uint iLevelIndex);
private:
	unordered_map<const _tchar*, class CComponent*>*				m_pPrototypes = nullptr;
	typedef unordered_map<const _tchar*, class CComponent*>			PROTOTYPES;
private:
	_uint			m_iNumLevels = 0;

public:
	CComponent* Find_Component(_uint iLevelIndex, const _tchar* pPrototypeTag);

public:
	virtual void Free() override;
};

END