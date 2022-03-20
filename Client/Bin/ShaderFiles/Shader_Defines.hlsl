
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

	ComparisonFunc = LESS_EQUAL; // s(p) 보다 작거나 같은 점을 찍자
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


// ----------------------
// 

#endif
