#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CPicking final : public CBase
{
	DECLARE_SINGLETON(CPicking)
public:
	CPicking();
	virtual ~CPicking() = default;
public:
	void Set_WindowHandle(HWND hWnd) {
		m_hWnd = hWnd;
	}

public:
	HRESULT Transform_ToWorldSpace();
	HRESULT Transform_WorldSpaceToLocalSpace(class CTransform* pTransform);
	_bool Picking(class CVIBuffer* pBuffer, _float3* pPickPos);

	_float3 Get_MouseRayPos() { return m_vMouseRayPos; }
	_float3 Get_MouseRay() { return m_vMouseRay; }

private:
	HWND		m_hWnd;
	_float3		m_vMouseRay, m_vMouseRayPos;
	_float3		m_vLocalMouseRay, m_vLocalMouseRayPos;

public:
	virtual void Free()	override;

};

END