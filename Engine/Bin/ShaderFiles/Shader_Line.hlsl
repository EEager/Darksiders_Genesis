
cbuffer Matrices 
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float4		vColor : COLOR0;
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vColor : COLOR0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	
	Out.vColor = In.vColor;

	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vColor : COLOR0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};



PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = In.vColor;

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
