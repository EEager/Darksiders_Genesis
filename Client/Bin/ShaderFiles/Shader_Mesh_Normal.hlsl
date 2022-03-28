
#include "Shader_Defines.hlsl"
#include "LightHelper.hlsl" 

cbuffer Light
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
	bool		g_UseEmissiveMap;

	bool		g_UseRoughnessMap;
	bool		g_UseMetalMap;

	float		g_DissolvePwr;
};

cbuffer CameraDesc
{
	vector		g_vCamPosition = vector(0.f, 0.f, 0.f, 0.f);
};

// --------------------
// Global Variables
// --------------------
texture2D		g_DiffuseTexture; // Diffuse Map
texture2D		g_NormalTexture; // Normal Map
texture2D		g_EmissiveTexture; // Emissive Map
texture2D		g_MetalRoughnessTexture; // Roughness Metal Map
texture2D		g_DissolveTexture;


// --------------------
// VS
// --------------------
struct VS_IN
{
	float3		vPosL : POSITION;
	float3		vNormalL : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangentL : TANGENT;
};

struct VS_OUT
{
	float4		vPosP : SV_POSITION;
	float4		vNormalW : NORMAL;
	float3		TangentW : TANGENT;
	float2		vTexUV : TEXCOORD0;
	float4		vPosW : TEXCOORD1;
	float4		vPosPTexcoord : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosP = mul(vector(In.vPosL, 1.f), matWVP);
	Out.vNormalW = normalize(mul(vector(In.vNormalL, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.TangentW = mul(In.vTangentL, (float3x3)g_WorldMatrix);
	Out.vPosW = mul(vector(In.vPosL, 1.f), g_WorldMatrix);
	Out.vPosPTexcoord = Out.vPosP;

	return Out;
}


// --------------------
// PS
// --------------------
struct PS_IN
{
	float4		vPosP : SV_POSITION;
	float4		vNormalW : NORMAL;
	float3		TangentW : TANGENT;
	float2		vTexUV : TEXCOORD0;
	float4		vPosW : TEXCOORD1;
	float4		vPosPTexcoord : TEXCOORD2;
};

struct PS_DEFERRED_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormalW : SV_TARGET1;
	float4		vDepthW : SV_TARGET2; 
};

PS_DEFERRED_OUT PS_DEFERRED_MAIN(PS_IN In)
{
	PS_DEFERRED_OUT		Out = (PS_DEFERRED_OUT)0;

	// ------------------------------------
	// #1. vDiffuse : SV_TARGET0;
	float4 texColor = float4(1, 1, 1, 1); // Default to multiplicative identity.
	texColor = g_DiffuseTexture.Sample(samLinearClamp, In.vTexUV);	// Sample texture.
	clip(texColor.a - 0.1f);
	Out.vDiffuse = texColor;

	//	Emissive mapping
	if (g_UseEmissiveMap)
		Out.vDiffuse += g_EmissiveTexture.Sample(samLinear, In.vTexUV);

	// Dissolve
	if (g_DissolvePwr > 0)
	{
		float Dissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV).g; // r:잘게, g:부드럽게, b:한쪽먼저
		float dissolveTest = Dissolve - g_DissolvePwr;
		clip(dissolveTest);

		if (dissolveTest > 0.03 && dissolveTest <= 0.05)
		{
			Out.vDiffuse = float4(1, 0, 0, 1); // 빨
		}
		else if (dissolveTest <= 0.03f && dissolveTest > 0.01f)
		{
			Out.vDiffuse = float4(1, 1, 0, 1); // 노
		}
		else if (dissolveTest <= 0.01f)
		{
			Out.vDiffuse = float4(1, 1, 1, 1); // 흰
		}
	}

	// ------------------------------------
	// #2. vNormalW : SV_TARGET1;
	if (g_UseNormalMap) // Normal Map 사용시
	{
		float3 normalMapSample = g_NormalTexture.Sample(samAnisotropic, In.vTexUV).rgb;
		Out.vNormalW.xyz = NormalSampleToWorldSpace(normalMapSample, In.vNormalW.xyz, In.TangentW);
		Out.vNormalW = vector(Out.vNormalW.xyz * 0.5f + 0.5f, 0.f);
	}
	else
	{
		Out.vNormalW = vector(In.vNormalW.xyz * 0.5f + 0.5f, 0.f);
	}

	// ------------------------------------
	// #3. vDepthW : SV_TARGET2
	// x,y상의 깊이를 저장하자. 700 : far
	Out.vDepthW = vector(In.vPosPTexcoord.z / In.vPosPTexcoord.w, In.vPosPTexcoord.w / 700.f, 0.f, 0.f);

	return Out;
}



struct PS_FORWARD_OUT
{
	float4		vDiffuse : SV_TARGET;
};

PS_FORWARD_OUT PS_FORWARD_MAIN(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	// Interpolating normal can unnormalize it, so normalize it.
	In.vNormalW = normalize(In.vNormalW);

	float3 toEyeW = g_vCamPosition.xyz - In.vPosW.xyz;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEyeW);

	// Normalize.
	toEyeW /= distToEye;

	// Default to multiplicative identity.
	float4 texColor = float4(1, 1, 1, 1);
	// Sample texture.
	texColor = g_DiffuseTexture.Sample(samLinearClamp, In.vTexUV);

	clip(texColor.a - 0.1f);

	// --------------------------
	//	Normal mapping
	// --------------------------
	float3 normalMapSample = g_NormalTexture.Sample(samAnisotropic, In.vTexUV).rgb;
	float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample, In.vNormalW.xyz, In.TangentW);

	// --------------------------
	//	Light
	// --------------------------
	Out.vDiffuse = texColor;
	{
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// Sum the light contribution from each light source.
		float4 A, D, S;
		if (g_UseNormalMap)
			ComputeDirectionalLight(g_Material, g_DirLight, float4(bumpedNormalW, 0.f), toEyeW.xyz, A, D, S);
		else
			ComputeDirectionalLight(g_Material, g_DirLight, In.vNormalW, toEyeW.xyz, A, D, S);

		ambient += A;
		diffuse += D;

		if (g_UseRoughnessMap)
		{
			spec = spec + S + (S * g_MetalRoughnessTexture.Sample(samLinear, In.vTexUV).r * 20.f);
		}
		else
		{
			spec += S;
		}

		//ComputePointLight(g_Material, g_PointLight, In.vPosW.xyz, In.vNormalW.xyz, toEyeW, A, D, S);
		//ambient += A;
		//diffuse += D;
		//spec += S;

		//ComputeSpotLight(g_Material, g_SpotLight, In.vPosW.xyz, In.vNormalW.xyz, toEyeW, A, D, S);
		//ambient += A;
		//diffuse += D;            
		//spec += S;

		// Modulate with late add.
		Out.vDiffuse = texColor * (ambient + diffuse) + spec;

		// --------------------------
		//	Emissive mapping
		// --------------------------
		if (g_UseEmissiveMap)
			Out.vDiffuse += g_EmissiveTexture.Sample(samLinear, In.vTexUV);
	}


	// --------------------------
	// Fogging 마지막에 하자 
	// --------------------------
	float fogLerp = 0.f;
	if (In.vPosW.y <= 0) // 땅위는 안개 표현하지말자
	{
		fogLerp = saturate((-In.vPosW.y - 1.0f) / 100.f);
	}

	//vector fogColor = vector(0.75f, 0.75f, 0.75f, 1.0f); // Gray
	vector fogColor = vector(0.835, 0.509f, 0.235f, 1.0f); // 석양느낌
	Out.vDiffuse = lerp(Out.vDiffuse, fogColor, fogLerp);

	// Common to take alpha from diffuse material and texture.
	Out.vDiffuse.a = g_Material.vMtrlDiffuse.a * texColor.a;

	return Out;
}

technique11	DefaultTechnique
{
	pass Deferred_Pass
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(NoCull);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFERRED_MAIN();
	}

	// Api에서 render state 설정하자. outline 같은거 그릴때 이 Pass를 수행
	pass Forward_ApiRenderState_Pass
	{
		SetRasterizerState(NoCull);
		VertexShader = compile vs_5_0 VS_MAIN(); 
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FORWARD_MAIN();
	}

	// 지형 Fog는 Forward 프로세스로 처리해야한다.
	pass Forward_Pass
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(NoCull);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FORWARD_MAIN();
	}

}
