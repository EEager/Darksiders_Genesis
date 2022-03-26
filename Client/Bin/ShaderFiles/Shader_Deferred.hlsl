
cbuffer Matrices 
{
	matrix			g_TransformMatrix;
	matrix			g_ProjMatrix;
};

cbuffer InverseMatrices
{
	matrix			g_ViewMatrixInverse;
	matrix			g_ProjMatrixInverse;
};

cbuffer Camera
{
	vector			g_vCamPosition;
};


cbuffer Light
{
	vector			g_vLightDir;
	vector			g_vLightPos;
	float			g_fRange;
	vector			g_vLightDiffuse;
	vector			g_vLightAmbient;
	vector			g_vLightSpecular;
};

cbuffer Material
{
	vector			g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
	vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
};

texture2D		g_TargetTexture;
texture2D		g_NormalTexture;
texture2D		g_DepthTexture;
texture2D		g_DiffuseTexture;
texture2D		g_ShadeTexture;
texture2D		g_SpecularTexture;

sampler DefaultSampler = sampler_state {
	filter = min_mag_mip_linear;
};


// ----------
// VS
// ----------
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


	matrix		matTP;

	matTP = mul(g_TransformMatrix, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matTP);
	
	Out.vTexUV = In.vTexUV;

	return Out;
}

// ----------
// PS
// ----------
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

	Out.vColor = g_TargetTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}


struct PS_OUT_DIRECTIONAL
{
	float4		vShade : SV_TARGET0;
};


struct PS_OUT_LIGHT
{
	float4		vShade : SV_TARGET0;
	float4		vSpecular : SV_TARGET1;
};


PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);

	float		fViewZ = vDepthDesc.y * 300.f;

	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	Out.vShade = g_vLightDiffuse * saturate(saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient));
	Out.vShade.a = 1.f;

	vector		vWorldPos;

	/* 투영공간상의 위치. */
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	/* 뷰스페이스상의 위치. */
	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);

	/* 월드스페이스상의 위치. */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);


	vector			vReflect = normalize(reflect(normalize(g_vLightDir), vNormal));
	vector			vLook = normalize(vWorldPos - g_vCamPosition);

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(vReflect * -1.f, vLook)), 30.f);

	return Out;
}


PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);

	float		fViewZ = vDepthDesc.y * 300.f;

	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vWorldPos;

	/* 투영공간상의 위치. */
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	/* 뷰스페이스상의 위치. */
	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);

	/* 월드스페이스상의 위치. */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);

	vector		vLightDir = vWorldPos - g_vLightPos;
	float		fDistance = length(vLightDir);

	float		fAtt = max((g_fRange - fDistance), 0.f) / g_fRange;

	Out.vShade = g_vLightDiffuse * saturate(saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
	Out.vShade.a = 1.f;

	vector			vReflect = normalize(reflect(normalize(vLightDir), vNormal));
	vector			vLook = normalize(vWorldPos - g_vCamPosition);

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(vReflect * -1.f, vLook)), 30.f) * fAtt;
	Out.vSpecular.a = 0.f;

	return Out;
}

PS_OUT PS_MAIN_FINAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = vDiffuse * vShade + vSpecular;

	if (0.f == Out.vColor.a)
		discard;

	return Out;
}

BlendState NonBlendState
{
	BlendEnable[0] = false;
};

BlendState LightBlendState
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	BlendOp = Add;
	SrcBlend = one;
	DestBlend = one;
};


DepthStencilState NonZTestDepthStencilState
{
	DepthEnable = false;
	DepthWriteMask = all;
	DepthFunc = less;
};

RasterizerState DefaultRasterizerState
{
	FillMode = solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};


technique11	DefaultTechnique
{
	pass DefaultPass
	{			
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Light_Direction
	{
		SetBlendState(LightBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetBlendState(LightBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	pass FinalRender
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FINAL();
	}

}
