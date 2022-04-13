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
	matrix			g_ShadowTransform_Env; // �׸������ VP
	matrix			g_ShadowTransform_Objects; // �׸������ VP

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

cbuffer TimeDelta
{
	float			g_fTimeDelta;
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

	// �� ������Ʈ���� ������ Normal ���� ��������. 
	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV); 
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f); // [0,1] = > [-1,1]

	// �� ������Ʈ���� ������ Depth���� ��������.
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vWorldPos = ToWorldPosition(vDepthDesc, In.vTexUV, g_ViewMatrixInverse, g_ProjMatrixInverse);

	// -------------------------
	// ������ ����.
	// -------------------------
	vNormal = normalize(vNormal); 
	float3 toEyeW = normalize(g_vCamPosition.xyz - vWorldPos.xyz); //toEyeW : ī�޶� ���� ����

	// Start with a sum of zero. 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// -------------------------
	// Shadow Mapping
	// -------------------------
	// Only the first light casts a shadow.
	// CalcShadowFactor ����
		// 3��° ���ڴ� �����å���� pin.ShadowPosH�̴�.
		// 1) pin.ShadowPosH�� vs���� vout.ShadowPosH = mul(float4(vin.PosL, 1.0f), gShadowTransform);
		// 2) gShadowTransform�� world*shadowTransform
		// �� vin.PosL * world*shadowTransform = vWorldPos * g_ShadowTransform

	float4 A, D, S;
	float shadowEnv = 1.f;
	float shadowObjects = 1.f;

	ComputeDirectionalLight(g_Material, g_DirLight, vNormal, toEyeW.xyz, A, D, S);


	// �� ������Ʈ �׸��ڸ� �����Ѵ�
	{
		shadowObjects = CalcShadowFactor(samShadow, g_ShadowMap_Objects, mul(vWorldPos, g_ShadowTransform_Objects));
	}

	// ���� ���� �׸��ڸ� �����Ѵ� 
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

	///* ������������ ��ġ. */
	//vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	//vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	//vWorldPos.z = vDepthDesc.x;
	//vWorldPos.w = 1.f;

	///* �佺���̽����� ��ġ. */
	//vWorldPos = vWorldPos * fViewZ;
	//vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);

	///* ���彺���̽����� ��ġ. */
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
	// War ���̰� �� �ָ� �ִ� ���
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
	// �⺻ ������
	//
	Out.vColor = vDiffuse * vShade + vSpecular;
	Out.vColor += vEmissive;
	Out.vColor += vHitPower;

	if (0.f == Out.vColor.a)
		discard;

	// 
	// Fogging ����� ���Ϸ� ��������
	// 
	vector vWorldPos = ToWorldPosition(vDepthDesc, In.vTexUV, g_ViewMatrixInverse, g_ProjMatrixInverse);
	float		fHeight = vWorldPos.y;
	float		fogLerp = 0.f;
	if (fHeight <= 0) // ���� Fogging ��� ���
	{
		fogLerp = saturate((-fHeight - 1.0f) / 100.f);
	}
	vector fogColor = vector(0.835, 0.509f, 0.235f, 1.0f); // �������
	Out.vColor = lerp(Out.vColor, fogColor, fogLerp);


	return Out;
}


// ����۸� Noise(Distortion)�� �̿��Ͽ� sampling �Ѵ�.
// For Distortion 
PS_OUT PS_MAIN_DISTORTION(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vBackBuffer = g_BackBufferTexture.Sample(DefaultSampler, In.vTexUV);
	vector vDistortion = g_DistortionTexture.Sample(DefaultSampler, In.vTexUV);
	//
	// �⺻ ������
	//
	Out.vColor = vBackBuffer;
	if (0.f == Out.vColor.a)
		discard;

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

	// #3 : ����� + ������
	pass FinalRender
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FINAL();
	}

	// #4 : ShadowMap �����
	pass ShadowMapDebug_Pass
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN2();
	}

	// #5 : Render Blending Post Alpha
	pass RenderBlendingPostAlpha_Pass
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
	}

}
