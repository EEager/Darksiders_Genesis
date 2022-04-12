
#include "Shader_Defines.hlsl"

// --------------------
// sampler_state
// --------------------
SamplerState SampleType
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;
	MipLODBias = 0.0f;
	MaxAnisotropy = 1;
	ComparisonFunc = ALWAYS;
	BorderColor = float4(0.f, 0.f, 0.f, 0.f);
	MinLOD = 0;
};

SamplerState SampleType2
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
	AddressW = CLAMP;
	MipLODBias = 0.0f;
	MaxAnisotropy = 1;
	ComparisonFunc = ALWAYS;
	BorderColor = float4(0.f, 0.f, 0.f, 0.f);
	MinLOD = 0;
};


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

texture2D		g_DiffuseTexture;
texture2D		g_DepthTexture; // ����Ʈ ������
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

	// �Ʒ� 3���� distortion �뵵
	float2 texCoords1 : TEXCOORD2;
	float2 texCoords2 : TEXCOORD3;
	float2 texCoords3 : TEXCOORD4;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	matrix		TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
	vector		vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);
	Out.vPosition = mul(vPosition, matWVP);
	Out.vProjPos = Out.vPosition;

	// �ȼ� ���̴��� �ؽ�ó ��ǥ�� �����Ѵ�.
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
	float4		vProjPos : TEXCOORD1; // ����Ʈ ������ �뵵

	// �Ʒ� 3���� distortion �뵵
	float2 texCoords1 : TEXCOORD2;
	float2 texCoords2 : TEXCOORD3;
	float2 texCoords3 : TEXCOORD4;
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

// For.Fire Effect With Distortion
PS_OUT PS_MAIN_ALPHA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

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
	Out.vColor = g_DiffuseTexture.Sample(SampleType2, noiseCoords.xy);

	// �����ǰ� �ְ� �� �ؽ�ó ���ø� ��ǥ�� ����Ͽ� ���� �ؽ�ó���� ���� ���� ���ø��մϴ�.
	// �̰��� ���� ������ ���˴ϴ�.
	// ȭ���� ���¸� ���δ� ��� Ŭ���� ���� ���¸� ����Ͽ� ȭ���� ���δ� ���� �����մϴ�.
	alphaColor = g_AlphaTexture.Sample(SampleType2, noiseCoords.xy);

	// ȭ���� ���� ������ �Ҿ����ϰ� �ְ� �� ���� �ؽ�ó ������ �����մϴ�.
	Out.vColor.a = alphaColor;


	//// 
	//// ����Ʈ ������
	//// 

	//// ������ ������.
	//// Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//float4		vRealProjPos = In.vProjPos / In.vProjPos.w;
	//float2		vTexUV;
	//vTexUV.x = vRealProjPos.x * 0.5f + 0.5f;
	//vTexUV.y = vRealProjPos.y * -0.5f + 0.5f;
	//vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexUV);
	//float		fViewZ = vDepthDesc.y * 700.f;
	//// In.vProjPos.w : �� ����Ʈ �ȼ��� �佺���̽� ���� ����. 
	//// fViewZ : �� ����Ʈ �ȼ��� �׸������ߴ� ��ġ�� �̹� ��ϵǾ��ִ� �������Ǳ���. 
	//// ���� �׸� �ȼ��� �������� ���� �������ٸ�, �׽�����������, ���ĺ������� �����ϰ� ������. fDistance�� 0���� �۾����� ���������� �Ⱥ��δ�.
	//float		fDistance = max(fViewZ - In.vProjPos.w, 0.f);
	//Out.vColor.a = Out.vColor.a * fDistance;

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

	pass AlphaBlendingPass // For.FireEffect
	{
		SetBlendState(AlphaBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}

}
