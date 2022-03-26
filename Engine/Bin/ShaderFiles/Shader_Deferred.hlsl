
cbuffer Matrices 
{
	matrix			g_TransformMatrix;
	matrix			g_ProjMatrix;
};

cbuffer Light
{
	vector			g_vLightDir;
};

texture2D		g_TargetTexture;
texture2D		g_NormalTexture;
texture2D		g_DiffuseTexture;
texture2D		g_ShadeTexture;

sampler DefaultSampler = sampler_state {
	filter = min_mag_mip_linear;
};

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


PS_OUT_DIRECTIONAL PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_DIRECTIONAL		Out = (PS_OUT_DIRECTIONAL)0;

	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	Out.vShade = saturate(dot(normalize(g_vLightDir) * -1.f, vNormal));
	Out.vShade.a = 1.f;

	return Out;

}

PS_OUT PS_MAIN_FINAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = vDiffuse * vShade;

	if (0.f == Out.vColor.a)
		discard;

	return Out;
}

BlendState NonBlendState
{
	BlendEnable[0] = false;
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
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
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
