#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL RenderStates
{
public:
	static ComPtr<ID3D11RasterizerState> WireframeRS;
	static ComPtr<ID3D11RasterizerState> NoCullRS;
	static ComPtr<ID3D11RasterizerState> CullClockwiseRS;

	static ComPtr<ID3D11DepthStencilState> MarkMirrorDSS;
	static ComPtr<ID3D11DepthStencilState> DrawReflectionDSS;
	static ComPtr<ID3D11DepthStencilState> NoDoubleBlendDSS;
	static ComPtr<ID3D11DepthStencilState> noDepthState;

	static ComPtr<ID3D11BlendState> AlphaToCoverageBS;
	static ComPtr<ID3D11BlendState> TransparentBS;
	static ComPtr<ID3D11BlendState> NoRenderTargetWritesBS;

	/*
	static ComPtr<ID3D11SamplerState> pointWrap;
    static ComPtr<ID3D11SamplerState> pointClamp;
    static ComPtr<ID3D11SamplerState> linearWrap;
    static ComPtr<ID3D11SamplerState> linearClamp;
    static ComPtr<ID3D11SamplerState> anisotropicWrap;
    static ComPtr<ID3D11SamplerState> anisotropicClamp;
	*/

public: // Called in Render.cpp 
	static HRESULT CreateRenderStates(ID3D11Device* pDevice);
};
END