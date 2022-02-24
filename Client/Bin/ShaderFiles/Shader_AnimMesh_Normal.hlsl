
#include "Shader_Defines.hlsl"
#include "LightHelper.hlsl" 

cbuffer cbPerFrame
{
	DirectionalLight g_DirLight;
	PointLight g_PointLight;
	SpotLight g_SpotLight;

	float  gFogStart;
	float  gFogRange;
};

cbuffer cbPerObject
{
	matrix		g_WorldMatrix;
	matrix		g_ViewMatrix;
	matrix		g_ProjMatrix;
	Material	g_Material;
};

cbuffer CameraDesc
{
	vector		g_vCamPosition = vector(0.f, 0.f, 0.f, 0.f);
};

struct tagBoneMatrixArray
{
	matrix		Bones[192];
};

cbuffer BoneMatrices
{
	tagBoneMatrixArray		g_BoneMatrices;
};

texture2D		g_DiffuseTexture; // Diffuse Map
texture2D		g_NormalTexture; // Normal Map


// --------------------
// sampler_state
// --------------------
sampler DefaultSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;

	Filter = min_mag_mip_linear;
	
};

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};


// --------------------
// VS
// --------------------
struct VS_IN
{
	float3		vPosL : POSITION;
	float3		vNormalL : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosP : SV_POSITION;
	float4		vNormalW : NORMAL;
	float3		TangentW : TANGENT;
	float2		vTexUV : TEXCOORD0;
	float4		vPosW : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	// 해당 정점에 영향을 주는 뼈들의 가중 변환 행렬 값을 구한다. 
	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		BoneMatrix = 
		g_BoneMatrices.Bones[In.vBlendIndex.x] * In.vBlendWeight.x + 
		g_BoneMatrices.Bones[In.vBlendIndex.y] * In.vBlendWeight.y + 
		g_BoneMatrices.Bones[In.vBlendIndex.z] * In.vBlendWeight.z + 
		g_BoneMatrices.Bones[In.vBlendIndex.w] * fWeightW;

	vector		vPosBone = mul(vector(In.vPosL, 1.f), BoneMatrix);
	Out.vPosP = mul(vPosBone, matWVP);
	Out.vNormalW = normalize(mul(vector(In.vNormalL, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vPosW = mul(vector(In.vPosL, 1.f), g_WorldMatrix);

	return Out;
}


// --------------------
// PS
// --------------------
struct PS_IN
{
	float4		vPosP : SV_POSITION;
	float4		vNormalW : NORMAL;
	float3		TangentW : TANGENT;
	float2		vTexUV : TEXCOORD0;
	float4		vPosW : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Interpolating normal can unnormalize it, so normalize it.
	In.vNormalW = normalize(In.vNormalW);

	float3 toEyeW = g_vCamPosition.xyz - In.vPosW.xyz;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEyeW);

	// Normalize.
	toEyeW /= distToEye;

	// Default to multiplicative identity.
	float4 texColor = float4(1, 1, 1, 1);
	// if (gUseTexure)
	{
		// Sample texture.
		texColor = g_DiffuseTexture.Sample(samAnisotropic, In.vTexUV);

		//if (gAlphaClip)
		{
			// Discard pixel if texture alpha < 0.1.  Note that we do this
			// test as soon as possible so that we can potentially exit the shader 
			// early, thereby skipping the rest of the shader code.
			clip(texColor.a - 0.1f);
		}
	}


	// --------------------------
	//	Normal mapping
	// --------------------------
	float3 normalMapSample = g_NormalTexture.Sample(samLinear, In.vTexUV).rgb;
	float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample, In.vNormalW.xyz, In.TangentW);

	// --------------------------
	//	Light
	// --------------------------
	Out.vColor = texColor;
	{
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// Sum the light contribution from each light source.
		float4 A, D, S;

		ComputeDirectionalLight(g_Material, g_DirLight, In.vNormalW, toEyeW, A, D, S);
		//ComputeDirectionalLight(g_Material, g_DirLight, float4(bumpedNormalW, 0.f) , toEyeW.xyz, A, D, S);
		ambient += A;
		diffuse += D;
		spec += S;

		// Modulate with late add.
		Out.vColor = texColor * (ambient + diffuse) + spec;

		// 거울 같은거 반사 효과 낼때 사용 gCubeMap를 사용하네 여기서 
		/*if (gReflectionEnabled)
		{
			float3 incident = -toEye;
			float3 reflectionVector = reflect(incident, bumpedNormalW);
			float4 reflectionColor = gCubeMap.Sample(samLinear, reflectionVector);

			litColor += gMaterial.Reflect * reflectionColor;
		}*/
	}

	//
	// Fogging
	//

	//if (gFogEnabled)
	{
		// float fogLerp = saturate( (distToEye - gFogStart) / gFogRange ); 
		float fogLerp = saturate((distToEye - 15.0f) / 50.f);

		// Blend the fog color and the lit color.
		Out.vColor = lerp(Out.vColor, vector(0.75f, 0.75f, 0.75f, 1.0f), fogLerp);
	}


	// Common to take alpha from diffuse material and texture.
	Out.vColor.a = g_Material.vMtrlDiffuse.a * texColor.a;

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
