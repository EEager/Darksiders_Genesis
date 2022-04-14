
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

	// 아래 3개는 distortion 용도
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
	Out.vColor.a = min(Out.vColor.a, g_Alpha); // 서서히 나오게 하자.

	return Out;
}

PS_OUT PS_MAIN_HollowLord(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	// 현재 체력을 표시하자. 알파가 166~167사이이면 체력 배경이다. 
	if (In.vTexUV.x <= g_HollowLordCurHpUVX)
	{
		if (0.6509f <= Out.vColor.a && Out.vColor.a < 0.6549f)
			Out.vColor.rgb = float3(1.f, 0.f, 0.f);
	}
	Out.vColor.a = min(Out.vColor.a, g_Alpha); // 서서히 나오게 하자.

	return Out;
}

PS_OUT_DISTORTION PS_MAIN_DISTORTION(VS_OUT_DISTORTION In)
{
	PS_OUT_DISTORTION		Out = (PS_OUT_DISTORTION)0;

	// ---------------------------------------
	// Noise 이미지를 통한 불 UV 흐리기
	// ---------------------------------------
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
	Out.vBackBuffer = g_DiffuseTexture.Sample(SampleType2, noiseCoords.xy);

	// 교란되고 왜곡 된 텍스처 샘플링 좌표를 사용하여 알파 텍스처에서 알파 값을 샘플링합니다.
	// 이것은 불의 투명도에 사용됩니다.
	// 화스랩 상태를 감싸는 대신 클램프 샘플 상태를 사용하여 화염을 감싸는 것을 방지합니다.
	alphaColor = g_AlphaTexture.Sample(SampleType2, noiseCoords.xy);

	// 화재의 알파 블렌딩을 불안정하고 왜곡 된 알파 텍스처 값으로 설정합니다.
	Out.vBackBuffer.a = alphaColor;

	// ---------------------------------------
	// Distortion
	// ---------------------------------------
	float4	DistortionOut = { 0.f, 0.f, 0.f, 0.f }; // Clear Color는 _float4(0.f, 0.f, 0.f, 0.f)
	DistortionOut = g_NoiseTexture_HeatHaze.Sample(SampleType, In.vTexUV); // Noise를 사용한다.
	DistortionOut.w = g_AlphaTexture_HeatHaze.Sample(SampleType, In.vTexUV); // 알파를 사용한다.
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
