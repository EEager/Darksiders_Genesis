
#include "Shader_Defines.hlsl"
#include "LightHelper.hlsl" 

cbuffer cbPerFrame
{
	DirectionalLight g_DirLight;
	PointLight g_PointLight;
	SpotLight g_SpotLight;

	float  gFogStart;
	float  gFogRange;
};


cbuffer cbPerObject
{
	matrix		g_WorldMatrix;
	matrix		g_ViewMatrix;
	matrix		g_ProjMatrix;	
	Material	g_Material;
	bool		g_UseNormalMap;
};

cbuffer CameraDesc
{
	vector		g_vCamPosition = vector(0.f, 0.f, 0.f, 0.f);
};

struct tagBoneMatrixArray
{
	matrix		Bones[192];
};

cbuffer BoneMatrices
{
	tagBoneMatrixArray		g_BoneMatrices;
};

texture2D		g_DiffuseTexture; // Diffuse Map
texture2D		g_NormalTexture; // Normal Map


// --------------------
// sampler_state
// --------------------
sampler DefaultSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;

	Filter = min_mag_mip_linear;
};

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

// --------------------
// VS
// --------------------
struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matrix		BoneMatrix = g_BoneMatrices.Bones[0];

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);


	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);

	Out.vPosition = mul(vPosition, matWVP);

	Out.vTexUV = In.vTexUV;

	return Out;
}


// --------------------
// PS
// --------------------
struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};



PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	clip(Out.vColor.a - 0.1f);


	return Out;
}

RasterizerState NoCull
{
	CullMode = None;
};




technique11	DefaultTechnique
{
	pass DefaultPass
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
		SetRasterizerState(NoCull);
	}

}
