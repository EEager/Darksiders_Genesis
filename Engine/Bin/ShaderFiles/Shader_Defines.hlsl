
#ifndef _SHADER_DEFINE
#define _SHADER_DEFINE

struct Material
{
	vector vMtrlDiffuse;
	vector vMtrlAmbient;
	vector vMtrlSpecular; // w = SpecPower
	vector vMtrlReflect;
	vector vMtrlEmissive;
	float  fMtrlPower;
	float3 pad;
};


// --------------------
// sampler_state
// --------------------
SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState samLinearClamp
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = BORDER;
	AddressV = BORDER;
};

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState DefaultSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerComparisonState samShadow
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	ComparisonFunc = LESS;
};


// --------------------
// BlendState
// --------------------
BlendState AlphaBlendState
{
	BlendEnable[0] = true;
	BlendOp = Add;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
};

BlendState OneBlendState
{
	BlendEnable[0] = true;
	BlendOp = Add;
	SrcBlend = one;
	DestBlend = one;
};

BlendState NonBlendState
{
	BlendEnable[0] = false;
};

BlendState LightBlendState
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	BlendOp = Add;
	SrcBlend = one;
	DestBlend = one;
};


// --------------------
// DepthStencilState
// --------------------
DepthStencilState DefaultDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less;
};

DepthStencilState NonZTestDepthStencilState
{
	DepthEnable = false;
	DepthWriteMask = all;
	DepthFunc = less;
};

DepthStencilState NonZTestNonZWriteDepthStencilState
{
	DepthEnable = false;
	DepthWriteMask = zero;
	DepthFunc = less;
};

// --------------------
// RasterizerState
// --------------------
RasterizerState DefaultRasterizerState
{
	FillMode = solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState CullCWRasterizerState
{
	FillMode = solid;
	CullMode = front;
	FrontCounterClockwise = false;
};

RasterizerState WireframeRasterizerState
{
	FillMode = wireframe;
};


RasterizerState NoCull
{
	CullMode = None;
};

RasterizerState ShadowDepthNoCull
{
	// [From MSDN]
	// If the depth buffer currently bound to the output-merger stage has a UNORM format or
	// no depth buffer is bound the bias value is calculated like this: 
	//
	// Bias = (float)DepthBias * r + SlopeScaledDepthBias * MaxDepthSlope;
	//
	// where r is the minimum representable value > 0 in the depth-buffer format converted to float32.
	// [/End MSDN]
	// 
	// For a 24-bit depth buffer, r = 1 / 2^24.
	//
	// Example: DepthBias = 100000 ==> Actual DepthBias = 100000/2^24 = .006

	// You need to experiment with these values for your scene.
	DepthBias = 100000;
	DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;

	CullMode = None;

};


// -----------------------
	// -----------------------
vector ToWorldPosition(vector depthDesc, float2	vTexUV, matrix viewInvMat, matrix projInvMat)
{
	// 저장할때 In.vProjPos.w / 700.0f을 하였으니. fViewZ = In.vPosP.w이다 
	float		fViewZ = depthDesc.y * 700.f; 
	vector		vWorldPos;
	/* 투영공간상의 위치. */
	vWorldPos.x = vTexUV.x * 2.f - 1.f;
	vWorldPos.y = vTexUV.y * -2.f + 1.f;
	vWorldPos.z = depthDesc.x;
	vWorldPos.w = 1.f;
	/* 뷰스페이스상의 위치. */
	vWorldPos = vWorldPos * fViewZ; 
	vWorldPos = mul(vWorldPos, projInvMat);
	/* 월드스페이스상의 위치. */
	vWorldPos = mul(vWorldPos, viewInvMat);

	return vWorldPos;
}

#endif
