#pragma once

#include "Component.h"

BEGIN(Engine)

#if 1 // JJLEE_TEST_RenderStates
class ENGINE_DLL RenderStates
{
public:
	static ComPtr<ID3D11RasterizerState> WireframeRS;
	static ComPtr<ID3D11RasterizerState> NoCullRS;
	static ComPtr<ID3D11RasterizerState> CullClockwiseRS;

	static ComPtr<ID3D11DepthStencilState> MarkMirrorDSS;
	static ComPtr<ID3D11DepthStencilState> DrawReflectionDSS;
	static ComPtr<ID3D11DepthStencilState> NoDoubleBlendDSS;

	static ComPtr<ID3D11BlendState> AlphaToCoverageBS;
	static ComPtr<ID3D11BlendState> TransparentBS;
	static ComPtr<ID3D11BlendState> NoRenderTargetWritesBS;
};
#endif
END