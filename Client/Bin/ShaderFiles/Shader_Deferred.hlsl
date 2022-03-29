#include "Shader_Defines.hlsl"
#include "LightHelper.hlsl" 

cbuffer Light
{
	DirectionalLight g_DirLight;
	PointLight g_PointLight;
	SpotLight g_SpotLight;
	Material	g_Material;
};

cbuffer Matrices 
{
	matrix			g_TransformMatrix;
	matrix			g_ProjMatrix;
	matrix			g_ShadowTransform; // 그림자행렬

};

cbuffer InverseMatrices
{
	matrix			g_ViewMatrixInverse;
	matrix			g_ProjMatrixInverse;
};

cbuffer Camera
{
	vector			g_vCamPosition;
};

cbuffer Material
{
	vector			g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
	vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
};

texture2D		g_TargetTexture;
texture2D		g_NormalTexture;
texture2D		g_DepthTexture;
texture2D		g_DepthTexture_War;
texture2D		g_DiffuseTexture;
texture2D		g_EmissiveTexture;
texture2D		g_HitPowerTexture;

texture2D		g_ShadeTexture;
texture2D		g_SpecularTexture;

Texture2D		g_ShadowMap;


// ----------
// VS
// ----------
struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;


	matrix		matTP;

	matTP = mul(g_TransformMatrix, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matTP);
	
	Out.vTexUV = In.vTexUV;

	return Out;
}

// ----------
// PS
// ----------
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

	Out.vColor = g_TargetTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}


struct PS_OUT_DIRECTIONAL
{
	float4		vShade : SV_TARGET0;
};


struct PS_OUT_LIGHT
{
	float4		vShade : SV_TARGET0;
	float4		vSpecular : SV_TARGET1;
};


PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	// 각 오브젝트들이 저장한 Normal 값을 가져오자. 
	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV); 
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f); // [0,1] = > [-1,1]

	// 각 오브젝트들이 저장한 Depth값을 가져오자.
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vWorldPos = ToWorldPosition(vDepthDesc, In.vTexUV, g_ViewMatrixInverse, g_ProjMatrixInverse);

	// -------------------------
	// 빛연산 시작.
	// -------------------------
	vNormal = normalize(vNormal); 
	float3 toEyeW = normalize(g_vCamPosition.xyz - vWorldPos.xyz); //toEyeW : 카메라 보는 벡터

	// Start with a sum of zero. 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Only the first light casts a shadow.
	//float3 shadow = float3(1.0f, 1.0f, 1.0f);
	// pin.ShadowPosH = vWorldPos를 VP그림자행렬(g_ShadowTransform)로 곱한위치이다.
	// 그래서 뭘 던져주면되냐.. 
	//shadow = CalcShadowFactor(samShadow, g_ShadowMap, );

	// Sum the light contribution from each light source.
	float4 A, D, S;
	ComputeDirectionalLight(g_Material, g_DirLight, vNormal, toEyeW.xyz, A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;

	Out.vShade = (ambient + diffuse);
	//Out.vShade.a = 1.f;
	Out.vSpecular = spec;

	return Out;
}


PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	return (PS_OUT_LIGHT)0;
	//PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	//vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	//vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);

	//float		fViewZ = vDepthDesc.y * 300.f;

	//vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	//vector		vWorldPos;

	///* 투영공간상의 위치. */
	//vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	//vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	//vWorldPos.z = vDepthDesc.x;
	//vWorldPos.w = 1.f;

	///* 뷰스페이스상의 위치. */
	//vWorldPos = vWorldPos * fViewZ;
	//vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);

	///* 월드스페이스상의 위치. */
	//vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);

	//vector		vLightDir = vWorldPos - g_vLightPos;
	//float		fDistance = length(vLightDir);

	//float		fAtt = max((g_fRange - fDistance), 0.f) / g_fRange;

	//Out.vShade = g_vLightDiffuse * saturate(saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
	//Out.vShade.a = 1.f;

	//vector			vReflect = normalize(reflect(normalize(vLightDir), vNormal));
	//vector			vLook = normalize(vWorldPos - g_vCamPosition);

	//Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(vReflect * -1.f, vLook)), 30.f) * fAtt;
	//Out.vSpecular.a = 0.f;

	//return Out;
}


// ------------------
// Render Blend
// ------------------
PS_OUT PS_MAIN_FINAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;


	//
	// War 깊이가 더 멀리 있는 경우
	//
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDepthDesc_War = g_DepthTexture_War.Sample(DefaultSampler, In.vTexUV);
	if (vDepthDesc.r < vDepthDesc_War.r)
	{
		Out.vColor = float4(1.f / 255.f, 249.f / 255.f, 254.f / 255.f, 1.f);
		return Out;
	}


	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vHitPower = g_HitPowerTexture.Sample(DefaultSampler, In.vTexUV);

	//
	// 기본 빛연산
	//
	Out.vColor = vDiffuse * vShade + vSpecular;
	Out.vColor += vEmissive;
	Out.vColor += vHitPower;

	if (0.f == Out.vColor.a)
		discard;

	// 
	// Fogging 기법은 상하로 적용하자
	// 
	vector vWorldPos = ToWorldPosition(vDepthDesc, In.vTexUV, g_ViewMatrixInverse, g_ProjMatrixInverse);
	float		fHeight = vWorldPos.y;
	float		fogLerp = 0.f;
	if (fHeight <= 0) // 수직 Fogging 기법 사용
	{
		fogLerp = saturate((-fHeight - 1.0f) / 100.f);
	}
	vector fogColor = vector(0.835, 0.509f, 0.235f, 1.0f); // 석양느낌
	Out.vColor = lerp(Out.vColor, fogColor, fogLerp);


	return Out;
}





technique11	DefaultTechnique
{
	pass DefaultPass
	{			
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Light_Direction
	{
		SetBlendState(LightBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetBlendState(LightBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	pass FinalRender
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FINAL();
	}

}
