
//=============================================================================
// Lighting.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Transforms and lights geometry.
//=============================================================================

#include "Shader_Defines.hlsl"
#include "LightHelper.hlsl" 

cbuffer cbPerFrame
{
	DirectionalLight g_DirLight;
};

cbuffer cbPerObject
{
	matrix		g_WorldMatrix;
	matrix		g_ViewMatrix;
	matrix		g_ProjMatrix;
	Material	g_Material;
};

cbuffer CameraDesc
{
	vector		g_vCamPosition = vector(0.f, 0.f, 0.f, 0.f);
};

cbuffer BrushDesc
{
	vector		g_vBrushPosition;
	float		g_fRange = 0.f;
};


texture2D		g_SourTexture;
texture2D		g_DestTexture;
texture2D		g_FilterTexture;
texture2D		g_BrushTexture;

sampler DefaultSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;

	Filter = min_mag_mip_linear;

};

// --------------------
// VS
// --------------------
struct VS_IN
{
	float3		vPosL : POSITION;
	float3		vNormalL : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosP : SV_POSITION;
	float4		vNormalW : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vPosW : TEXCOORD1;
};

VS_OUT VS(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix); // ¿ùºäÇà
	matWVP = mul(matWV, g_ProjMatrix); // ¿ùºäÅõÇà

	Out.vPosP = mul(vector(In.vPosL, 1.f), matWVP);
	Out.vNormalW = normalize(mul(vector(In.vNormalL, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vPosW = mul(vector(In.vPosL, 1.f), g_WorldMatrix);

	return Out;
}

// --------------------
// PS
// --------------------

struct PS_IN
{
	float4		vPosP : SV_POSITION;
	float4		vNormalW : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vPosW : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};


PS_OUT PS(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Interpolating normal can unnormalize it, so normalize it.
	In.vNormalW = normalize(In.vNormalW);

	float3 toEyeW = normalize(g_vCamPosition - In.vPosW).xyz;

	// Start with a sum of zero. 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Sum the light contribution from each light source.
	float4 A, D, S;

	ComputeDirectionalLight(g_Material, g_DirLight, In.vNormalW, toEyeW, A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;

	//ComputePointLight(gMaterial, gPointLight, pin.PosW, pin.NormalW, toEyeW, A, D, S);
	//ambient += A;
	//diffuse += D;
	//spec += S;

	//ComputeSpotLight(gMaterial, gSpotLight, pin.PosW, pin.NormalW, toEyeW, A, D, S);
	//ambient += A;
	//diffuse += D;
	//spec += S;

	Out.vColor = ambient + diffuse + spec;

	// Common to take alpha from diffuse material.
	Out.vColor.a = g_Material.vMtrlDiffuse.a;

	return Out;
}

technique11 LightTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}



