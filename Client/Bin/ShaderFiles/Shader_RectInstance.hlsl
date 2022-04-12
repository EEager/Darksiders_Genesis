
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
texture2D		g_DepthTexture; // 소프트 렌더링
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

	// 아래 3개는 distortion 용도
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

	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
	Out.vTexUV = In.vTexUV;

	// 첫 번째 스케일 및 위쪽 스크롤링 속도 값을 사용하여 첫 번째 노이즈 텍스처에 대한 텍스처 좌표를 계산합니다.
	Out.texCoords1 = (In.vTexUV * scales.x);
	Out.texCoords1.y = Out.texCoords1.y + (frameTime * scrollSpeeds.x);

	// 두 번째 스케일 및 위쪽 스크롤링 속도 값을 사용하여 두 번째 노이즈 텍스처의 텍스처 좌표를 계산합니다.
	Out.texCoords2 = (In.vTexUV * scales.y);
	Out.texCoords2.y = Out.texCoords2.y + (frameTime * scrollSpeeds.y);

	// 세 번째 스케일 및 위쪽 스크롤링 속도 값을 사용하여 세 번째 노이즈 텍스처의 텍스처 좌표를 계산합니다.
	Out.texCoords3 = (In.vTexUV * scales.z);
	Out.texCoords3.y = Out.texCoords3.y + (frameTime * scrollSpeeds.z);

	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1; // 소프트 렌더링 용도

	// 아래 3개는 distortion 용도
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

	// 색상을 얻어오자.
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


	// 3 개의 서로 다른 텍스처 좌표를 사용하여 동일한 노이즈 텍스처를 샘플링하여 3 개의 다른 노이즈 스케일을 얻습니다.
	noise1 = g_NoiseTexture.Sample(SampleType, In.texCoords1);
	noise2 = g_NoiseTexture.Sample(SampleType, In.texCoords2);
	noise3 = g_NoiseTexture.Sample(SampleType, In.texCoords3);

	// (0, 1) 범위에서 (-1, +1) 범위로 노이즈를 이동합니다.
	noise1 = (noise1 - 0.5f) * 2.0f;
	noise2 = (noise2 - 0.5f) * 2.0f;
	noise3 = (noise3 - 0.5f) * 2.0f;

	// 3 개의 서로 다른 왜곡 x와 y 값으로 세 개의 노이즈 x와 y 좌표를 왜곡합니다.
	noise1.xy = noise1.xy * distortion1.xy;
	noise2.xy = noise2.xy * distortion2.xy;
	noise3.xy = noise3.xy * distortion3.xy;

	// 세 가지 왜곡 된 노이즈 결과를 모두 하나의 노이즈 결과로 결합합니다.
	finalNoise = noise1 + noise2 + noise3;

	// 왜곡 스케일 및 바이어스 값에 의해 입력 텍스처 Y 좌표를 왜곡합니다.
	// 위쪽 효과에서 불꽃이 깜박 거리는 텍스처를 위로 움직이면 섭동이 강해집니다.
	perturb = ((1.0f - In.vTexUV.y) * distortionScale) + distortionBias;

	// 이제 화재 색상 텍스처를 샘플링하는 데 사용할 교란되고 왜곡 된 텍스처 샘플링 좌표를 만듭니다.
	noiseCoords.xy = (finalNoise.xy * perturb) + In.vTexUV.xy;

	// 섭동되고 왜곡 된 텍스처 샘플링 좌표를 사용하여 화재 텍스처에서 색상을 샘플링합니다.
	// 화스랩 상태를 감싸는 대신 클램프 샘플 상태를 사용하여 화염을 감싸는 것을 방지합니다.
	Out.vColor = g_DiffuseTexture.Sample(SampleType2, noiseCoords.xy);

	// 교란되고 왜곡 된 텍스처 샘플링 좌표를 사용하여 알파 텍스처에서 알파 값을 샘플링합니다.
	// 이것은 불의 투명도에 사용됩니다.
	// 화스랩 상태를 감싸는 대신 클램프 샘플 상태를 사용하여 화염을 감싸는 것을 방지합니다.
	alphaColor = g_AlphaTexture.Sample(SampleType2, noiseCoords.xy);

	// 화재의 알파 블렌딩을 불안정하고 왜곡 된 알파 텍스처 값으로 설정합니다.
	Out.vColor.a = alphaColor;


	//// 
	//// 소프트 렌더링
	//// 

	//// 색상을 얻어오자.
	//// Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//float4		vRealProjPos = In.vProjPos / In.vProjPos.w;
	//float2		vTexUV;
	//vTexUV.x = vRealProjPos.x * 0.5f + 0.5f;
	//vTexUV.y = vRealProjPos.y * -0.5f + 0.5f;
	//vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexUV);
	//float		fViewZ = vDepthDesc.y * 700.f;
	//// In.vProjPos.w : 내 이펙트 픽셀의 뷰스페이스 상의 깊이. 
	//// fViewZ : 내 이펙트 픽셀을 그릴려고했던 위치에 이미 기록되어있던 누군가의깊이. 
	//// 내가 그릴 픽셀이 누군가에 의해 가려진다면, 테스팅하지말고, 알파블랜딩으로 투명하게 만들자. fDistance가 0보다 작아지면 투명해져서 안보인다.
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
