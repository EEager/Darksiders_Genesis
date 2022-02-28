#pragma once

#include "Model.h"
#include "Texture.h"
#include "Renderer.h"
#include "RendererStates.h"
#include "Transform.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"


/* 컴포넌트들의 원형을 모아둔다. */

BEGIN(Engine)

class CComponent_Manager final : public CBase
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

private:
	CComponent* Find_Component(_uint iLevelIndex, const _tchar* pPrototypeTag);

public:
	virtual void Free() override;
};

END