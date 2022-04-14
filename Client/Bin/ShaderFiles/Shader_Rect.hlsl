
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
};

cbuffer DistortionBuffer
{
	float2 distortion1;
	float2 distortion2;
	float2 distortion3;
	float distortionScale;
	float distortionBias;
};

cbuffer Effects
{
	float			g_Alpha;
};

cbuffer HollowLord
{
	float			g_HollowLordCurHpUVX;
};

texture2D		g_DiffuseTexture;
texture2D		g_NoiseTexture; // Noise
texture2D		g_NoiseTexture_HeatHaze; // Noise
texture2D		g_AlphaTexture; // Alpha
texture2D		g_AlphaTexture_HeatHaze; // Alpha


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



struct VS_OUT_DISTORTION
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;

	// �Ʒ� 3���� distortion �뵵
	float2 texCoords1 : TEXCOORD1;
	float2 texCoords2 : TEXCOORD2;
	float2 texCoords3 : TEXCOORD3;
}; 



VS_OUT_DISTORTION VS_MAIN_DISTORTION(VS_IN In)
{
	VS_OUT_DISTORTION		Out = (VS_OUT_DISTORTION)0;

	matrix		matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	// ù ��° ������ �� ���� ��ũ�Ѹ� �ӵ� ���� ����Ͽ� ù ��° ������ �ؽ�ó�� ���� �ؽ�ó ��ǥ�� ����մϴ�.
	Out.texCoords1 = (In.vTexUV * scales.x);
	Out.texCoords1.y = Out.texCoords1.y + (frameTime * scrollSpeeds.x);

	// �� ��° ������ �� ���� ��ũ�Ѹ� �ӵ� ���� ����Ͽ� �� ��° ������ �ؽ�ó�� �ؽ�ó ��ǥ�� ����մϴ�.
	Out.texCoords2 = (In.vTexUV * scales.y);
	Out.texCoords2.y = Out.texCoords2.y + (frameTime * scrollSpeeds.y);

	// �� ��° ������ �� ���� ��ũ�Ѹ� �ӵ� ���� ����Ͽ� �� ��° ������ �ؽ�ó�� �ؽ�ó ��ǥ�� ����մϴ�.
	Out.texCoords3 = (In.vTexUV * scales.z);
	Out.texCoords3.y = Out.texCoords3.y + (frameTime * scrollSpeeds.z);

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


struct PS_OUT_DISTORTION
{
	float4		vBackBuffer : SV_TARGET0;
	float4		vDistortion : SV_TARGET1;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN2(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vColor.a = min(Out.vColor.a, g_Alpha); // ������ ������ ����.

	return Out;
}

PS_OUT PS_MAIN_HollowLord(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	// ���� ü���� ǥ������. ���İ� 166~167�����̸� ü�� ����̴�. 
	if (In.vTexUV.x <= g_HollowLordCurHpUVX)
	{
		if (0.6509f <= Out.vColor.a && Out.vColor.a < 0.6549f)
			Out.vColor.rgb = float3(1.f, 0.f, 0.f);
	}
	Out.vColor.a = min(Out.vColor.a, g_Alpha); // ������ ������ ����.

	return Out;
}

PS_OUT_DISTORTION PS_MAIN_DISTORTION(VS_OUT_DISTORTION In)
{
	PS_OUT_DISTORTION		Out = (PS_OUT_DISTORTION)0;

	// ---------------------------------------
	// Noise �̹����� ���� �� UV �帮��
	// ---------------------------------------
	float4 noise1;
	float4 noise2;
	float4 noise3;
	float4 finalNoise;
	float perturb;
	float2 noiseCoords;
	float4 fireColor;
	float4 alphaColor;

	// 3 ���� ���� �ٸ� �ؽ�ó ��ǥ�� ����Ͽ� ������ ������ �ؽ�ó�� ���ø��Ͽ� 3 ���� �ٸ� ������ �������� ����ϴ�.
	noise1 = g_NoiseTexture.Sample(SampleType, In.texCoords1);
	noise2 = g_NoiseTexture.Sample(SampleType, In.texCoords2);
	noise3 = g_NoiseTexture.Sample(SampleType, In.texCoords3);

	// (0, 1) �������� (-1, +1) ������ ����� �̵��մϴ�.
	noise1 = (noise1 - 0.5f) * 2.0f;
	noise2 = (noise2 - 0.5f) * 2.0f;
	noise3 = (noise3 - 0.5f) * 2.0f;

	// 3 ���� ���� �ٸ� �ְ� x�� y ������ �� ���� ������ x�� y ��ǥ�� �ְ��մϴ�.
	noise1.xy = noise1.xy * distortion1.xy;
	noise2.xy = noise2.xy * distortion2.xy;
	noise3.xy = noise3.xy * distortion3.xy;

	// �� ���� �ְ� �� ������ ����� ��� �ϳ��� ������ ����� �����մϴ�.
	finalNoise = noise1 + noise2 + noise3;

	// �ְ� ������ �� ���̾ ���� ���� �Է� �ؽ�ó Y ��ǥ�� �ְ��մϴ�.
	// ���� ȿ������ �Ҳ��� ���� �Ÿ��� �ؽ�ó�� ���� �����̸� ������ �������ϴ�.
	perturb = ((1.0f - In.vTexUV.y) * distortionScale) + distortionBias;

	// ���� ȭ�� ���� �ؽ�ó�� ���ø��ϴ� �� ����� �����ǰ� �ְ� �� �ؽ�ó ���ø� ��ǥ�� ����ϴ�.
	noiseCoords.xy = (finalNoise.xy * perturb) + In.vTexUV.xy;

	// �����ǰ� �ְ� �� �ؽ�ó ���ø� ��ǥ�� ����Ͽ� ȭ�� �ؽ�ó���� ������ ���ø��մϴ�.
	// ȭ���� ���¸� ���δ� ��� Ŭ���� ���� ���¸� ����Ͽ� ȭ���� ���δ� ���� �����մϴ�.
	Out.vBackBuffer = g_DiffuseTexture.Sample(SampleType2, noiseCoords.xy);

	// �����ǰ� �ְ� �� �ؽ�ó ���ø� ��ǥ�� ����Ͽ� ���� �ؽ�ó���� ���� ���� ���ø��մϴ�.
	// �̰��� ���� ������ ���˴ϴ�.
	// ȭ���� ���¸� ���δ� ��� Ŭ���� ���� ���¸� ����Ͽ� ȭ���� ���δ� ���� �����մϴ�.
	alphaColor = g_AlphaTexture.Sample(SampleType2, noiseCoords.xy);

	// ȭ���� ���� ������ �Ҿ����ϰ� �ְ� �� ���� �ؽ�ó ������ �����մϴ�.
	Out.vBackBuffer.a = alphaColor;

	// ---------------------------------------
	// Distortion
	// ---------------------------------------
	float4	DistortionOut = { 0.f, 0.f, 0.f, 0.f }; // Clear Color�� _float4(0.f, 0.f, 0.f, 0.f)
	DistortionOut = g_NoiseTexture_HeatHaze.Sample(SampleType, In.vTexUV); // Noise�� ����Ѵ�.
	DistortionOut.w = g_AlphaTexture_HeatHaze.Sample(SampleType, In.vTexUV); // ���ĸ� ����Ѵ�.
	Out.vDistortion = DistortionOut;

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
	pass ZIgnoreNAlphablending
	{
		SetBlendState(AlphaBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestNonZWriteDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// 2
	pass ZIgnoreNAlphablending_AlphaControl
	{
		SetBlendState(AlphaBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestNonZWriteDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN2();
	}

	// 3
	pass Distortion_Alpha
	{
		SetBlendState(AlphaBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN_DISTORTION();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
	}

	// 4
	pass ZIgnoreNAlphablending_HollowLord
	{
		SetBlendState(AlphaBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestNonZWriteDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HollowLord();
	}
}
