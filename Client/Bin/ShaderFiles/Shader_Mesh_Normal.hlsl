
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


	float		g_NoiseAlphaPwr;
};

cbuffer cbGameObject
{
	vector		g_vHitPower;
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
texture2D		g_NoiseTexture;


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


struct VS_SHADOW_OUT
{
	float4 vPosP : SV_POSITION;
	float2 vTexUV  : TEXCOORD;
};

VS_SHADOW_OUT VS_SHADOW(VS_IN In)
{
	VS_SHADOW_OUT		Out = (VS_SHADOW_OUT)0;

	// 그림자 시점의 V, P는 Light_Manager가 가지고 있는 m_LightView, m_LightProj값이다. 
	matrix		matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	Out.vPosP = mul(vector(In.vPosL, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
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
	float4		vEmissive : SV_TARGET3; 
	float4		vHitPower : SV_TARGET4;
};

PS_DEFERRED_OUT PS_DEFERRED_MAIN(PS_IN In)
{
	PS_DEFERRED_OUT		Out = (PS_DEFERRED_OUT)0;

	// ------------------------------------
	// #1. vDiffuse : SV_TARGET0;
	float4 texColor = float4(1, 1, 1, 1); // Default to multiplicative identity.
	texColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);	// Sample texture.
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

	// -----------------------------
	// #4. vEmissive : SV_TARGET3
	// Emissive 맵 출력
	if (g_UseEmissiveMap)
		Out.vEmissive = g_EmissiveTexture.Sample(samLinear, In.vTexUV);

	// -----------------------------
	// #5. g_vHitPower : SV_TARGET4
	Out.vHitPower = g_vHitPower;

	return Out;
}

// ----------------------
// PS_DEFERRED_ONLY_WAR
// ----------------------
struct PS_DEFERRED_ONLY_WAR_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormalW : SV_TARGET1;
	float4		vDepthW : SV_TARGET2;
	float4		vEmissive : SV_TARGET3;
	float4		vHitPower : SV_TARGET4;
	float4		vDepthW_War : SV_TARGET5;
};

PS_DEFERRED_ONLY_WAR_OUT PS_DEFERRED_ONLY_WAR_MAIN(PS_IN In)
{
	PS_DEFERRED_ONLY_WAR_OUT		Out = (PS_DEFERRED_ONLY_WAR_OUT)0;

	// ------------------------------------
	// #1. vDiffuse : SV_TARGET0;
	float4 texColor = float4(1, 1, 1, 1); // Default to multiplicative identity.
	texColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);	// Sample texture.
	clip(texColor.a - 0.1f);
	Out.vDiffuse = texColor;

	//	Emissive mapping
	if (g_UseEmissiveMap)
		Out.vDiffuse += g_EmissiveTexture.Sample(DefaultSampler, In.vTexUV);

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
	// vDepthW_War :  : SV_TARGET5
	Out.vDepthW_War = vector(In.vPosPTexcoord.z / In.vPosPTexcoord.w, In.vPosPTexcoord.w / 700.f, 0.f, 0.f);

	// -----------------------------
	// #4. vEmissive : SV_TARGET3
	// Emissive 맵 출력
	if (g_UseEmissiveMap)
		Out.vEmissive = g_EmissiveTexture.Sample(samLinear, In.vTexUV);

	//// -----------------------------
	//// #5. vEmissive : SV_TARGET4
	//// g_vHitPower 맵 출력
	Out.vHitPower = vector(0.f, 0.f, 0.f, 0.f); // 일반 Mesh에는 적용하지 말자


	return Out;
}

// --------------------
// PS_FOWARD_OUT
// --------------------

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


// --------------------
// PS_ALPHABLEND
// --------------------

struct PS_ALPHABLEND_OUT
{
	float4		vBackBuffer : SV_TARGET0;
	float4		vDistortion   : SV_TARGET1;
};

PS_ALPHABLEND_OUT PS_ALPHABLEND(PS_IN In)
{
	PS_ALPHABLEND_OUT		Out = (PS_ALPHABLEND_OUT)0;
	// BackBuffer
	Out.vBackBuffer = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//Out.vBackBuffer.w = 1.f;
	
	// Distortion
	float4	DistortionOut = { 0.f, 0.f, 0.f, 0.f };
	DistortionOut = g_NoiseTexture.Sample(SampleType, In.vTexUV); // Noise를 사용한다.
	//DistortionOut.w = g_AlphaTexture.Sample(SampleType, In.vTexUV); // 알파를 사용한다.
	Out.vDistortion = DistortionOut;

	return Out;
}

PS_ALPHABLEND_OUT PS_ALPHABLEND_ONLYDISTORTION(PS_IN In)
{
	PS_ALPHABLEND_OUT		Out = (PS_ALPHABLEND_OUT)0;

	// Distortion
	float4	DistortionOut = { 0.f, 0.f, 0.f, 0.f };
	DistortionOut = g_NoiseTexture.Sample(SampleType, In.vTexUV); // Noise를 사용한다.
	Out.vDistortion = DistortionOut;
	Out.vDistortion.w *= g_NoiseAlphaPwr;

	return Out;
}

technique11	DefaultTechnique
{
	// #0
	pass Deferred_Pass
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(NoCull);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFERRED_MAIN();
	}

	// #1
	pass Deferred_WarOnly_Pass // War만의 Depth를 따로 찍어줘야한다
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(NoCull);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFERRED_ONLY_WAR_MAIN();
	}

	// #2
	// Api에서 render state 설정하자. outline 같은거 그릴때 이 Pass를 수행
	pass Forward_ApiRenderState_Pass
	{
		SetRasterizerState(NoCull);
		VertexShader = compile vs_5_0 VS_MAIN(); 
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FORWARD_MAIN();
	}

	// #3
	pass BuildShadowMap_Pass
	{
		VertexShader = compile vs_5_0 VS_SHADOW();
		GeometryShader = NULL;
		PixelShader = NULL;

		SetRasterizerState(ShadowDepthNoCull);
	}

	// #4 알파타겟용도. 매쉬 이펙트에 알파맥일때 사용. 디스토션 타겟에다가도 넣어보자.
	pass P4
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHABLEND();
		
		SetBlendState(AlphaBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetRasterizerState(NoCull);
	}
	pass P5
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHABLEND_ONLYDISTORTION();

		SetBlendState(AlphaBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetRasterizerState(NoCull);
	}
}
