
#include "Shader_Defines.hlsl"

cbuffer Matrices 
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
};

cbuffer Camera
{
	vector			g_vCamPosition;
};

texture2D		g_DiffuseTexture;

struct VS_IN
{
	float3		vPosition : POSITION;
	float		fPSize: PSIZE;

	float4		vRight : TEXCOORD0;
	float4		vUp : TEXCOORD1;
	float4		vLook : TEXCOORD2;
	float4		vTranslation : TEXCOORD3;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float		fPSize : PSIZE;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matrix		TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector		vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);
	
	Out.vPosition = mul(vPosition, g_WorldMatrix);
	
	Out.fPSize = In.fPSize;

	return Out;
}

struct GS_IN
{
	float4		vPosition : POSITION;
	float		fPSize : PSIZE;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
	GS_OUT			Out[4];

	float3			vLook = normalize(g_vCamPosition - In[0].vPosition).xyz;
	float3			vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook));
	float3			vUp = normalize(cross(vLook, vRight));

	Out[0].vPosition = vector(In[0].vPosition.xyz + vRight * In[0].fPSize * 0.5f + vUp * In[0].fPSize * 0.5f, 1.f);
	Out[0].vTexUV = float2(0.f, 0.f);


	Out[1].vPosition = vector(In[0].vPosition.xyz - vRight * In[0].fPSize * 0.5f + vUp * In[0].fPSize * 0.5f, 1.f);
	Out[1].vTexUV = float2(1.f, 0.f);

	Out[2].vPosition = vector(In[0].vPosition.xyz - vRight * In[0].fPSize * 0.5f - vUp * In[0].fPSize * 0.5f, 1.f);
	Out[2].vTexUV = float2(1.f, 1.f);

	Out[3].vPosition = vector(In[0].vPosition.xyz + vRight * In[0].fPSize * 0.5f - vUp * In[0].fPSize * 0.5f, 1.f);
	Out[3].vTexUV = float2(0.f, 1.f);

	matrix		matVP;
	matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);	

	OutStream.Append(Out[0]);
	OutStream.Append(Out[1]);
	OutStream.Append(Out[2]);
	OutStream.RestartStrip();

	OutStream.Append(Out[0]);
	OutStream.Append(Out[2]);
	OutStream.Append(Out[3]);
	OutStream.RestartStrip();
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

	Out.vColor.r = 1.f;
	Out.vColor.gb = 0.f;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}




technique11	DefaultTechnique
{
	pass DefaultPass
	{	
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}
