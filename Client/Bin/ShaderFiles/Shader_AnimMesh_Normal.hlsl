
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

	bool		g_UseNormalMap;
	bool		g_UseEmissiveMap;

	bool		g_UseRoughnessMap;
	bool		g_UseMetalMap;

	float		g_DissolvePwr;
};

cbuffer cbGameObject
{
	vector		g_vHitPower;
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

// --------------------
// Global Variables
// --------------------
texture2D		g_DiffuseTexture; // Diffuse Map
texture2D		g_NormalTexture; // Normal Map
texture2D		g_EmissiveTexture; // Emissive Map
texture2D		g_MetalRoughnessTexture; // Roughness Metal Map
texture2D		g_DissolveTexture; // Dissolve Map


bool		g_DrawOutLine = false;

// --------------------
// VS
// --------------------
struct VS_IN
{
	float3		vPosL : POSITION;
	float3		vNormalL : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangentL : TANGENT;
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosP : SV_POSITION; // ��ġ����
	float4		vNormalW : NORMAL; // �븻����
	float3		TangentW : TANGENT; // ź��Ʈ����
	float2		vTexUV : TEXCOORD0; // �ؽ�����ǥ����
	float4		vPosW : TEXCOORD1; // ������ġ
	float4		vPosPTexcoord : TEXCOORD2; // Depth Map ���鶧 ���

};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	// �ش� ������ ������ �ִ� ������ ���� ��ȯ ��� ���� ���Ѵ�. 
	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		BoneMatrix = 
		g_BoneMatrices.Bones[In.vBlendIndex.x] * In.vBlendWeight.x + 
		g_BoneMatrices.Bones[In.vBlendIndex.y] * In.vBlendWeight.y + 
		g_BoneMatrices.Bones[In.vBlendIndex.z] * In.vBlendWeight.z + 
		g_BoneMatrices.Bones[In.vBlendIndex.w] * fWeightW;

	vector		vPosBone = mul(vector(In.vPosL, 1.f), BoneMatrix);
	Out.vPosP = mul(vPosBone, matWVP);


	vector		vNormal = mul(vector(In.vNormalL, 0.f), BoneMatrix);
	Out.vNormalW = normalize(mul(vNormal, g_WorldMatrix));
	//Out.vNormalW = normalize(mul(vector(In.vNormalL, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.TangentW = mul(In.vTangentL, (float3x3)g_WorldMatrix);
	Out.vPosW = mul(vector(In.vPosL, 1.f), g_WorldMatrix);
	Out.vPosPTexcoord = Out.vPosP;
	// Generate projective tex-coords to project shadow map onto scene.
	//Out.vShadowPosH = mul(float4(In.vPosL, 1.0f), g_ShadowTransform);

	return Out;
}

struct VS_SHADOW_OUT
{
	float4 vPosP : SV_POSITION;
	float2 vTexUV  : TEXCOORD;
};

VS_SHADOW_OUT VS_SHADOW(VS_IN In)
{
	VS_SHADOW_OUT		Out = (VS_SHADOW_OUT)0;
	matrix		matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	// �ش� ������ ������ �ִ� ������ ���� ��ȯ ��� ���� ���Ѵ�. 
	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		BoneMatrix =
		g_BoneMatrices.Bones[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.Bones[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.Bones[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.Bones[In.vBlendIndex.w] * fWeightW;
	vector		vPosBone = mul(vector(In.vPosL, 1.f), BoneMatrix);
	Out.vPosP = mul(vPosBone, matWVP);
	Out.vTexUV = In.vTexUV;
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
	float4		vPosPTexcoord : TEXCOORD2;
};

struct PS_DEFERRED_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormalW : SV_TARGET1;
	float4		vDepthW : SV_TARGET2;
	float4		vEmissive : SV_TARGET3;
	float4		vHitPower : SV_TARGET4;
};

PS_DEFERRED_OUT PS_DEFERRED_MAIN(PS_IN In)
{
	PS_DEFERRED_OUT		Out = (PS_DEFERRED_OUT)0;

	// ------------------------------------
	// #1. vDiffuse : SV_TARGET0;
	float4 texColor = float4(1, 1, 1, 1); // Default to multiplicative identity.
	texColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);	// Sample texture.
	clip(texColor.a - 0.1f);
	Out.vDiffuse = texColor;

	//	Emissive mapping
	if (g_UseEmissiveMap)
		Out.vDiffuse += g_EmissiveTexture.Sample(DefaultSampler, In.vTexUV);

	// Dissolve
	if (g_DissolvePwr > 0)
	{
		float Dissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV).g; // r:�߰�, g:�ε巴��, b:���ʸ���
		float dissolveTest = Dissolve - g_DissolvePwr;
		clip(dissolveTest);

		if (dissolveTest > 0.03 && dissolveTest <= 0.05)
		{
			Out.vDiffuse = float4(1, 0, 0, 1); // ��
		}
		else if (dissolveTest <= 0.03f && dissolveTest > 0.01f)
		{
			Out.vDiffuse = float4(1, 1, 0, 1); // ��
		}
		else if (dissolveTest <= 0.01f)
		{
			Out.vDiffuse = float4(1, 1, 1, 1); // ��
		}
	}

	// ------------------------------------
	// #2. vNormalW : SV_TARGET1;
	if (g_UseNormalMap) // Normal Map ����
	{
		float3 normalMapSample = g_NormalTexture.Sample(samAnisotropic, In.vTexUV).rgb;
		Out.vNormalW.xyz = NormalSampleToWorldSpace(normalMapSample, In.vNormalW.xyz, In.TangentW);
		Out.vNormalW = vector(Out.vNormalW.xyz * 0.5f + 0.5f, 0.f);
	}
	else
	{
		Out.vNormalW = vector(In.vNormalW.xyz * 0.5f + 0.5f, 0.f);
	}

	// ------------------------------------
	// #3. vDepthW : SV_TARGET2
	// x,y���� ���̸� ��������. 700 : far
	Out.vDepthW = vector(In.vPosPTexcoord.z / In.vPosPTexcoord.w, In.vPosPTexcoord.w / 700.f, 0.f, 0.f);

	// -----------------------------
	// #4. vEmissive : SV_TARGET3
	// Emissive �� ���
	if (g_UseEmissiveMap)
		Out.vEmissive = g_EmissiveTexture.Sample(samLinear, In.vTexUV);

	// -----------------------------
	// #5. vEmissive : SV_TARGET3
	// g_vHitPower �� ���
	Out.vHitPower += g_vHitPower;

	return Out;
}

// ----------------------
// PS_DEFERRED_ONLY_WAR
// ----------------------
struct PS_DEFERRED_ONLY_WAR_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormalW : SV_TARGET1;
	float4		vDepthW : SV_TARGET2;
	float4		vEmissive : SV_TARGET3;
	float4		vHitPower : SV_TARGET4;
	float4		vDepthW_War : SV_TARGET5;
};

PS_DEFERRED_ONLY_WAR_OUT PS_DEFERRED_ONLY_WAR_MAIN(PS_IN In)
{
	PS_DEFERRED_ONLY_WAR_OUT		Out = (PS_DEFERRED_ONLY_WAR_OUT)0;

	// ------------------------------------
	// #1. vDiffuse : SV_TARGET0;
	float4 texColor = float4(1, 1, 1, 1); // Default to multiplicative identity.
	texColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);	// Sample texture.
	clip(texColor.a - 0.1f);
	Out.vDiffuse = texColor;

	//	Emissive mapping
	if (g_UseEmissiveMap)
		Out.vDiffuse += g_EmissiveTexture.Sample(DefaultSampler, In.vTexUV);

	// Dissolve
	if (g_DissolvePwr > 0)
	{
		float Dissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV).g; // r:�߰�, g:�ε巴��, b:���ʸ���
		float dissolveTest = Dissolve - g_DissolvePwr;
		clip(dissolveTest);

		if (dissolveTest > 0.03 && dissolveTest <= 0.05)
		{
			Out.vDiffuse = float4(1, 0, 0, 1); // ��
		}
		else if (dissolveTest <= 0.03f && dissolveTest > 0.01f)
		{
			Out.vDiffuse = float4(1, 1, 0, 1); // ��
		}
		else if (dissolveTest <= 0.01f)
		{
			Out.vDiffuse = float4(1, 1, 1, 1); // ��
		}
	}

	// ------------------------------------
	// #2. vNormalW : SV_TARGET1;
	if (g_UseNormalMap) // Normal Map ����
	{
		float3 normalMapSample = g_NormalTexture.Sample(samAnisotropic, In.vTexUV).rgb;
		Out.vNormalW.xyz = NormalSampleToWorldSpace(normalMapSample, In.vNormalW.xyz, In.TangentW);
		Out.vNormalW = vector(Out.vNormalW.xyz * 0.5f + 0.5f, 0.f);
	}
	else
	{
		Out.vNormalW = vector(In.vNormalW.xyz * 0.5f + 0.5f, 0.f);
	}

	// ------------------------------------
	// #3. vDepthW : SV_TARGET2
	// x,y���� ���̸� ��������. 700 : far
	Out.vDepthW = vector(In.vPosPTexcoord.z / In.vPosPTexcoord.w, In.vPosPTexcoord.w / 700.f, 0.f, 0.f);
	// vDepthW_War :  : SV_TARGET5
	Out.vDepthW_War = vector(In.vPosPTexcoord.z / In.vPosPTexcoord.w, In.vPosPTexcoord.w / 700.f, 0.f, 0.f);

	// -----------------------------
	// #4. vEmissive : SV_TARGET3
	// Emissive �� ���
	if (g_UseEmissiveMap)
		Out.vEmissive = g_EmissiveTexture.Sample(samLinear, In.vTexUV);

	// -----------------------------
	// #5. vEmissive : SV_TARGET4
	// g_vHitPower �� ���
	Out.vHitPower += g_vHitPower;


	return Out;
}

// --------------------
// PS_FOWARD_OUT
// --------------------
struct PS_FOWARD_OUT
{
	float4		vDiffuse : SV_TARGET;
};


PS_FOWARD_OUT PS_FORWARD_MAIN(PS_IN In)
{
	PS_FOWARD_OUT		Out = (PS_FOWARD_OUT)0;

	if (g_DrawOutLine)
	{
		float4 texColor = g_DiffuseTexture.Sample(samLinear, In.vTexUV);
		clip(texColor.a - 0.1f);
		Out.vDiffuse.xyz = float3(1.f / 255.f, 249.f / 255.f, 254.f / 255.f);
		return Out;
	}

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
		texColor = g_DiffuseTexture.Sample(samLinear, In.vTexUV);

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
	float3 normalMapSample = g_NormalTexture.Sample(samAnisotropic, In.vTexUV).rgb;
	float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample, In.vNormalW.xyz, In.TangentW);

	// --------------------------
	//	Light
	// --------------------------
	Out.vDiffuse = texColor;
	{
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// Sum the light contribution from each light source.
		float4 A, D, S;
		if (g_UseNormalMap)
			ComputeDirectionalLight(g_Material, g_DirLight, float4(bumpedNormalW, 0.f), toEyeW.xyz, A, D, S);
		else
			ComputeDirectionalLight(g_Material, g_DirLight, In.vNormalW, toEyeW.xyz, A, D, S);

		ambient += A;
		diffuse += D;

		if (g_UseRoughnessMap)
		{
			spec = spec + S + (S * g_MetalRoughnessTexture.Sample(samLinear, In.vTexUV).r * 20.f);
		}
		else
		{
			spec += S;
		}

		// Modulate with late add.
		Out.vDiffuse = texColor * (ambient + diffuse) + spec;

		// --------------------------
		//	Emissive mapping
		// --------------------------
		if (g_UseEmissiveMap)
			Out.vDiffuse += g_EmissiveTexture.Sample(samLinear, In.vTexUV);

		// �ſ� ������ �ݻ� ȿ�� ���� ��� gCubeMap�� ����ϳ� ���⼭ 
		/*if (gReflectionEnabled)
		{
			float3 incident = -toEye;
			float3 reflectionVector = reflect(incident, bumpedNormalW);
			float4 reflectionColor = gCubeMap.Sample(samLinear, reflectionVector);

			litColor += gMaterial.Reflect * reflectionColor;
		}*/
	}


	// --------------------------
	// Fogging �������� ����
	// --------------------------

	//if (gFogEnabled)
	{
		// float fogLerp = saturate( (distToEye - gFogStart) / gFogRange ); 
		// gFogRange : ũ�� �þ߰� �� �ߺ��δ�
		// gFogStart : �Ȱ� �����ų �þ� ���� ����.
		float fogLerp = 0.f;
		if (In.vPosW.y <= 0) // ������ �Ȱ� ǥ����������
		{
			fogLerp = saturate((-In.vPosW.y - 1.0f) / 100.f);
		}

		// Blend the fog color and the lit color.

		//vector fogColor = vector(0.75f, 0.75f, 0.75f, 1.0f); // Gray
		vector fogColor = vector(0.835, 0.509f, 0.235f, 1.0f); // �������
		Out.vDiffuse = lerp(Out.vDiffuse, fogColor, fogLerp);
	}

	// Common to take alpha from diffuse material and texture.
	Out.vDiffuse.a = g_Material.vMtrlDiffuse.a * texColor.a;

	Out.vDiffuse += g_vHitPower;


	return Out;
}


technique11	DefaultTechnique
{
	// #0
	pass Deferred_Pass
	{			
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(NoCull);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFERRED_MAIN();
	}

	// #1
	pass Deferred_WarOnly_Pass // War���� Depth�� ���� �������Ѵ�
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(NoCull);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFERRED_ONLY_WAR_MAIN();
	}

	// #2
	// Api���� render state ��������. War outline ������ �׸��� �� Pass�� ����
	// War�� �̰��� ����Ѵ�
	pass Forward_ApiRenderState_Pass
	{
		SetRasterizerState(NoCull);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FORWARD_MAIN();
	}

	// #3
	pass BuildShadowMap_Pass
	{
		VertexShader = compile vs_5_0 VS_SHADOW();
		GeometryShader = NULL;
		PixelShader = NULL;

		SetRasterizerState(ShadowDepthNoCull);
	}
}
