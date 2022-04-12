
#include "Shader_Defines.hlsl"

cbuffer Matrices 
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
};

cbuffer NoiseBuffer
{
	float frameTime;
	float3 scrollSpeeds;
	float3 scales;
	float padding;
};


texture2D		g_DiffuseTexture;
texture2D		g_DepthTexture;
texture2D		g_NoiseTexture; // Noise
texture2D		g_AlphaTexture; // Alpha

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;

	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matrix		TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector		vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vProjPos = Out.vPosition;
	Out.vTexUV = In.vTexUV;

	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};



PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// ������ ������.
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_ALPHA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// ������ ������.
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	float4		vRealProjPos = In.vProjPos / In.vProjPos.w;
	float2		vTexUV;
	vTexUV.x = vRealProjPos.x * 0.5f + 0.5f;
	vTexUV.y = vRealProjPos.y * -0.5f + 0.5f;
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexUV);
	float		fViewZ = vDepthDesc.y * 700.f;
	// In.vProjPos.w : �� ����Ʈ �ȼ��� �佺���̽� ���� ����. 
	// fViewZ : �� ����Ʈ �ȼ��� �׸������ߴ� ��ġ�� �̹� ��ϵǾ��ִ� �������Ǳ���. 

	// ���� �׸� �ȼ��� �������� ���� �������ٸ�, �׽�����������, ���ĺ������� �����ϰ� ������. fDistance�� 0���� �۾����� ���������� �Ⱥ��δ�.
	float		fDistance = max(fViewZ - In.vProjPos.w, 0.f);
	Out.vColor.a = Out.vColor.a * fDistance;

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
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass AlphaBlendingPass
	{
		SetBlendState(AlphaBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}

}
