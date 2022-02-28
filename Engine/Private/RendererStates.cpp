#include "..\public\RendererStates.h"
#include "GameObject.h"


#if 1 // JJLEE_TEST_RenderStates
// Rasterizer states
ComPtr<ID3D11RasterizerState> RenderStates::WireframeRS = 0;
ComPtr<ID3D11RasterizerState> RenderStates::NoCullRS = 0;
ComPtr<ID3D11RasterizerState> RenderStates::CullClockwiseRS = 0;


// Blend states
ComPtr<ID3D11BlendState> RenderStates::AlphaToCoverageBS = 0;
ComPtr<ID3D11BlendState> RenderStates::TransparentBS = 0;
ComPtr<ID3D11BlendState> RenderStates::NoRenderTargetWritesBS = 0;

// Depth/stencil states
ComPtr<ID3D11DepthStencilState>  RenderStates::MarkMirrorDSS = 0;
ComPtr<ID3D11DepthStencilState>  RenderStates::DrawReflectionDSS = 0;
ComPtr<ID3D11DepthStencilState>  RenderStates::NoDoubleBlendDSS = 0;

#endif