
#include "Shader_Defines.hlsl"

cbuffer Matrices 
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
};

struct tagBoneMatrixArray
{
	matrix		Bones[192];
};

cbuffer BoneMatrices
{
	tagBoneMatrixArray		g_BoneMatrices;
};

texture2D		g_DiffuseTexture;

sampler DefaultSampler = sampler_state
{
	/*minfilter = linear;
	mipfilter = linear;
	magfilter = linaer*/

	Filter = min_mag_mip_linear;
	
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
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

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		BoneMatrix = g_BoneMatrices.Bones[In.vBlendIndex.x] * In.vBlendWeight.x + 
		g_BoneMatrices.Bones[In.vBlendIndex.y] * In.vBlendWeight.y + 
		g_BoneMatrices.Bones[In.vBlendIndex.z] * In.vBlendWeight.z + 
		g_BoneMatrices.Bones[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	
	Out.vTexUV = In.vTexUV;

	return Out;
}


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

	// alpha testing
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
