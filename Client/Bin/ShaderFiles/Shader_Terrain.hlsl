
#include "Shader_Defines.hlsl"

cbuffer LightDesc
{
	vector		g_vLightDir;
	vector		g_vLightDiffuse;
	vector		g_vLightAmbient;
	vector		g_vLightSpecular;
};

cbuffer CameraDesc
{
	vector		g_vCamPosition = vector(0.f, 0.f, 0.f, 0.f);
};

cbuffer BrushDesc
{
	vector		g_vBrushPosition;
	float		g_fRange = 0.f;
};


cbuffer cbPerObject
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
	Material		g_Material;
};

texture2D		g_SourTexture;
texture2D		g_DestTexture;
texture2D		g_FilterTexture;
texture2D		g_BrushTexture;


struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
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
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};



PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vSourDiffuse = g_SourTexture.Sample(DefaultSampler, In.vTexUV * 10.f);
	vector		vDestDiffuse = g_DestTexture.Sample(DefaultSampler, In.vTexUV * 20.f);
	vector		vFilterDesc = g_FilterTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vBrushColor = vector(0.f, 0.f, 0.f, 0.f);

	/* 브러시 색상이 올라가야할 픽셀들이었다면. */
	if (g_vBrushPosition.x - g_fRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPosition.x + g_fRange && 
		g_vBrushPosition.z - g_fRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPosition.z + g_fRange)	
	{
		float2		vTexUV;
		vTexUV.x = (In.vWorldPos.x - (g_vBrushPosition.x - g_fRange)) / (2.f * g_fRange);
		vTexUV.y = ((g_vBrushPosition.z + g_fRange) - In.vWorldPos.z) / (2.f * g_fRange);

		vBrushColor = g_BrushTexture.Sample(DefaultSampler, vTexUV);
	}

	float		fShade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);

	vector		vReflect = reflect(normalize(g_vLightDir), In.vNormal);
	vector		vLook = normalize(In.vWorldPos - g_vCamPosition);

	float		fSpecular = pow(max(dot(normalize(vReflect) * -1.f, vLook), 0.f), g_Material.fMtrlPower);

	vector		vDiffuseMtrl = vSourDiffuse * vFilterDesc + vDestDiffuse * (1.f - vFilterDesc) + vBrushColor;

	Out.vColor = (g_vLightDiffuse * vDiffuseMtrl) * saturate(fShade + (g_vLightAmbient * g_Material.vMtrlAmbient))
		+ (g_vLightSpecular * g_Material.vMtrlSpecular) * fSpecular;

	return Out;
}



technique11	DefaultTechnique
{
	pass DefaultPass
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}
