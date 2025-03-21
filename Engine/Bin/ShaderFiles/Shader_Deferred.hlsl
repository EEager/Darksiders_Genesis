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
	matrix			g_ShadowTransform_Env; // 그림자행렬 VP
	matrix			g_ShadowTransform_Objects; // 그림자행렬 VP

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

cbuffer Distortion
{
	float			g_HeatHazeSpeed;
	float			g_HeatHazeStrength;
	float			g_fTimeDelta;
	bool			g_swapDistortion;
	bool			g_DesertDistortion;
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

// For.LightAcc
texture2D		g_ShadeTexture;
texture2D		g_SpecularTexture;

// For.Shadow Mapping
Texture2D		g_ShadowMap_Env;
Texture2D		g_ShadowMap_Objects;

// For.Distortion
Texture2D		g_BackBufferTexture;
Texture2D		g_DistortionTexture;

// For.PreBB
Texture2D		g_PreBBTexture;

// 사막의 아랑이를 표현하자.
Texture2D		g_NoiseTexture_HeatHaze;




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

VS_OUT VS_MAIN_PreBB_To_BB(VS_IN In)
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

// For ShadowMap Debug
PS_OUT PS_MAIN2(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 c = g_TargetTexture.Sample(DefaultSampler, In.vTexUV).r*0.5f;
	// draw as grayscale
	Out.vColor = float4(c.rrr, 1);
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

	// -------------------------
	// Shadow Mapping
	// -------------------------
	// Only the first light casts a shadow.
	// CalcShadowFactor 인자
		// 3번째 인자는 물방울책에서 pin.ShadowPosH이다.
		// 1) pin.ShadowPosH는 vs에서 vout.ShadowPosH = mul(float4(vin.PosL, 1.0f), gShadowTransform);
		// 2) gShadowTransform는 world*shadowTransform
		// 즉 vin.PosL * world*shadowTransform = vWorldPos * g_ShadowTransform

	float4 A, D, S;
	float shadowEnv = 1.f;
	float shadowObjects = 1.f;

	ComputeDirectionalLight(g_Material, g_DirLight, vNormal, toEyeW.xyz, A, D, S);


	// 각 오브젝트 그림자를 적용한다
	{
		shadowObjects = CalcShadowFactor(samShadow, g_ShadowMap_Objects, mul(vWorldPos, g_ShadowTransform_Objects));
	}

	// 먼저 지형 그림자를 적용한다 
	{
		shadowEnv = CalcShadowFactor(samShadow, g_ShadowMap_Env, mul(vWorldPos, g_ShadowTransform_Env));
	}


	ambient += A;
	diffuse += min(shadowEnv, shadowObjects) * D; 
	spec += min(shadowEnv, shadowObjects) * S;

	Out.vShade = (ambient + diffuse);
	Out.vShade.a = 1.f;
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

	//// HitPower + 림라이트
	//if (vHitPower.g > 0) // g가 있다면 몬스터다. 몬스터는 빨간색이다.
	//{
	//	Out.vColor += float4(vHitPower.r, 0.f, 0.f, 0.f); // 히트파워
	//	Out.vColor += float4(vHitPower.b, 0.f, 0.f, 0.f); // 림라이트 
	//}
	//else // g가 없다면 플레이어이다.
	//{
	//	Out.vColor += float4(vHitPower.r, vHitPower.r, vHitPower.r, 0.f); // 히트파워
	//	Out.vColor += float4(vHitPower.b, vHitPower.b, vHitPower.b, 0.f); // 림라이트
	//}


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


// 백버퍼를 Noise(Distortion)를 이용하여 sampling 한다.
// For Distortion 
PS_OUT PS_MAIN_DISTORTION(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vBackBuffer = g_BackBufferTexture.Sample(DefaultSampler, In.vTexUV);
	vector vDistortion = g_DistortionTexture.Sample(DefaultSampler, In.vTexUV);
	//
	// 기본 빛연산
	//
	Out.vColor = vBackBuffer;
	if (0.f == Out.vColor.a)
		discard;

	return Out;
}

// Deferred를 위해 OffScreen에 모아놧던것을 진짜 백버퍼에 출력하자 
PS_OUT PS_MAIN_PreBB_To_BB(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// g_DistortionTexture에 찍은 Distortion들을 표현하자
	// 이거는 별로 인거 같다..
	if (g_swapDistortion)
	{
		float4	Pert = g_DistortionTexture.Sample(DefaultSampler, In.vTexUV);
		float	x = In.vTexUV.x + (Pert.x) * g_HeatHazeStrength;
		float	y = In.vTexUV.y + (Pert.y) * g_HeatHazeStrength;
		Out.vColor = g_PreBBTexture.Sample(DefaultSampler, float2(x, y));
	}
	// cos, sin 사용.. 이거 잘 되는데요.
	else
	{
		// 이펙트에서 찍은 Distortion(Noise)를 가져온다.
		float noise = g_DistortionTexture.Sample(DefaultSampler, In.vTexUV);
		float alpha = g_DistortionTexture.Sample(DefaultSampler, In.vTexUV).w;
		if (noise != 0) // 0 이면 노이즈가 없는것이다. 
		{
			In.vTexUV.x += noise * cos(g_fTimeDelta * g_HeatHazeSpeed) * g_HeatHazeStrength * alpha;
			In.vTexUV.y += noise * sin(g_fTimeDelta * g_HeatHazeSpeed) * g_HeatHazeStrength * alpha;
		}
		Out.vColor = g_PreBBTexture.Sample(DefaultSampler, In.vTexUV);
	}


	// 사막의 아지랑이를 표현하자
	if (g_DesertDistortion)
	{
		float noise = g_NoiseTexture_HeatHaze.Sample(DefaultSampler, In.vTexUV);
		In.vTexUV.x += cos(noise * g_fTimeDelta * g_HeatHazeSpeed) * g_HeatHazeStrength;
		In.vTexUV.y += sin(noise * g_fTimeDelta * g_HeatHazeSpeed) * g_HeatHazeStrength;
		Out.vColor = g_PreBBTexture.Sample(samLinearClamp, In.vTexUV);
	}


	return Out;
}


PS_OUT PS_MAIN_PreBB_To_BB_TEst(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_PreBBTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}


technique11	DefaultTechnique
{
	// #0
	pass DefaultPass
	{			
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// #1
	pass Light_Direction
	{
		SetBlendState(LightBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	// #2
	pass Light_Point
	{
		SetBlendState(LightBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	// #3 : 논알파 + 빛연산
	pass FinalRender
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FINAL();
	}

	// #4 : ShadowMap 디버깅
	pass ShadowMapDebug_Pass
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN2();
	}

	// #5 : PreBB=>BB
	pass RenderPreBBToBB_Pass
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN_PreBB_To_BB();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PreBB_To_BB();
	}
	pass RenderPreBBToBB_Pass_Test
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN_PreBB_To_BB();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PreBB_To_BB_TEst();
	}

}
