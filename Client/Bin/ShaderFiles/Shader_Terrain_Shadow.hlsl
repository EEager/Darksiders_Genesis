
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

	float4x4 gShadowTransform;  // For.ShadowMap
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

Texture2D gShadowMap; // ShadowMap

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
	float4		ShadowPosH : TEXCOORD2;
};

VS_OUT VS(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix); // 월뷰행
	matWVP = mul(matWV, g_ProjMatrix); // 월뷰투행

	Out.vPosP = mul(vector(In.vPosL, 1.f), matWVP);
	Out.vNormalW = normalize(mul(vector(In.vNormalL, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vPosW = mul(vector(In.vPosL, 1.f), g_WorldMatrix);

	// Generate projective tex-coords to project shadow map onto scene.
	Out.ShadowPosH = mul(float4(In.vPosL, 1.f), gShadowTransform);

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
	float4		ShadowPosH : TEXCOORD2;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};


PS_OUT PS(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// --------------------------
	// Texture
	// --------------------------
	vector		vSourDiffuse = g_SourTexture.Sample(DefaultSampler, In.vTexUV * 10.f);
	vector		vDestDiffuse = g_DestTexture.Sample(DefaultSampler, In.vTexUV * 20.f);
	vector		vFilterDesc = g_FilterTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vBrushColor = vector(0.f, 0.f, 0.f, 0.f);

	/* 브러시 색상이 올라가야할 픽셀들이었다면. */
	if (g_vBrushPosition.x - g_fRange < In.vPosW.x && In.vPosW.x <= g_vBrushPosition.x + g_fRange &&
		g_vBrushPosition.z - g_fRange < In.vPosW.z && In.vPosW.z <= g_vBrushPosition.z + g_fRange)
	{
		float2		vTexUV;
		vTexUV.x = (In.vPosW.x - (g_vBrushPosition.x - g_fRange)) / (2.f * g_fRange);
		vTexUV.y = ((g_vBrushPosition.z + g_fRange) - In.vPosW.z) / (2.f * g_fRange);

		vBrushColor = g_BrushTexture.Sample(DefaultSampler, vTexUV);
	}


	// Interpolating normal can unnormalize it, so normalize it.
	In.vNormalW = normalize(In.vNormalW);

	float3 toEyeW = g_vCamPosition.xyz - In.vPosW.xyz;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEyeW);

	// Normalize.
	toEyeW /= distToEye;

	// --------------------------
	//	Light
	// --------------------------
	float4 texColor = vSourDiffuse * vFilterDesc + vDestDiffuse * (1.f - vFilterDesc) + vBrushColor;
	Out.vColor = texColor;
	{
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// Only the first light casts a shadow.
		float3 shadow = float3(1.0f, 1.0f, 1.0f);
		shadow[0] = CalcShadowFactor(samShadow, gShadowMap, In.ShadowPosH);

		// Sum the light contribution from each light source.
		float4 A, D, S;

		ComputeDirectionalLight(g_Material, g_DirLight, In.vNormalW, toEyeW, A, D, S);
		//ambient += A;
		//diffuse += D;
		//spec += S;

		ambient += A;
		diffuse += shadow[0] * D;
		spec += shadow[0] * S;

		//ComputePointLight(g_Material, g_PointLight, In.vPosW.xyz, In.vNormalW.xyz, toEyeW, A, D, S);
		//ambient += A;
		//diffuse += D;
		//spec += S;

		//ComputeSpotLight(g_Material, g_SpotLight, In.vPosW.xyz, In.vNormalW.xyz, toEyeW, A, D, S);
		//ambient += A;
		//diffuse += D;
		//spec += S;

		// Modulate with late add.
		Out.vColor = texColor * (ambient + diffuse) + spec;
	}

	//
	// Fogging
	//

	//if (gFogEnabled)
	{
		// float fogLerp = saturate( (distToEye - gFogStart) / gFogRange ); 
		float fogLerp = saturate((distToEye - 15.0f) / 100.f);

		// Blend the fog color and the lit color.
		Out.vColor = lerp(Out.vColor, vector(0.75f, 0.75f, 0.75f, 1.0f), fogLerp);
	}

	// Common to take alpha from diffuse material and texture.
	Out.vColor.a = g_Material.vMtrlDiffuse.a * texColor.a;

	return Out;
}

technique11 LightTech
{
	pass P0
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(WireframeRasterizerState);

		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}



