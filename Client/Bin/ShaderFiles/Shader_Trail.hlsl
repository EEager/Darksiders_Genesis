
#include "Shader_Defines.hlsl"

cbuffer Matrices 
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
};


texture2D		g_DiffuseTexture;

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

	matrix		matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_DEFERRED_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormalW : SV_TARGET1;
	float4		vDepthW : SV_TARGET2;
	float4		vEmissive : SV_TARGET3;
	float4		vHitPower : SV_TARGET4;
};

PS_DEFERRED_OUT PS_MAIN(PS_IN In)
{
	PS_DEFERRED_OUT		Out = (PS_DEFERRED_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//Out.vDiffuse = float4(1.f, 0.f, 1.f, 1.f);

	return Out;
}

technique11	DefaultTechnique
{
	// 0
	pass DefaultPass
	{			
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// 1
	pass AlphablendingPass
	{
		SetBlendState(AlphaBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
