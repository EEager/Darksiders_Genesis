
#include "Shader_Defines.hlsl"

cbuffer Matrices
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
	
	float			g_DissolvePwr;
};

texture2D		g_DiffuseTexture;
texture2D		g_DissolveTexture;

//--------------
// VS
// -------------
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
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);


	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;

	return Out;
}


//--------------
// PS
// -------------
struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
};



PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

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

	return Out;
}


technique11	DefaultTechnique
{
	pass DefaultPass
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(NoCull);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}