
#include "Shader_Defines.hlsl"

cbuffer Matrices 
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
};

cbuffer Camera
{
	vector			g_vCamPosition;
};

cbuffer Monster
{
	float			g_fMonsterHpUVX;
	float			g_fHpBarHeight;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float		fPSizeX: PSIZE0;
	float		fPSizeY: PSIZE1;
	float4		vRight : TEXCOORD0;
	float4		vUp : TEXCOORD1;
	float4		vLook : TEXCOORD2;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float		fPSizeX : PSIZE0;
	float		fPSizeY : PSIZE1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	// 몬스터 머리 위에 
	Out.vPosition = mul(vector(In.vPosition.x, In.vPosition.y + g_fHpBarHeight, In.vPosition.z, 1.f), g_WorldMatrix);
	Out.fPSizeX = In.fPSizeX;
	Out.fPSizeY = In.fPSizeY;

	return Out;
}

struct GS_IN
{
	float4		vPosition : POSITION;
	float		fPSizeX : PSIZE0;
	float		fPSizeY : PSIZE1;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
	GS_OUT			Out[4];

	// 정점에서 카메라를 본다.
	float3			vLook = normalize(g_vCamPosition - In[0].vPosition).xyz;
	float3			vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook));
	float3			vUp = normalize(cross(vLook, vRight));

	// 왼위
	Out[0].vPosition = vector(In[0].vPosition.xyz + vRight * In[0].fPSizeX * 0.5f + vUp * In[0].fPSizeY * 0.5f, 1.f);
	Out[0].vTexUV = float2(0.f, 0.f);

	// 오위
	Out[1].vPosition = vector(In[0].vPosition.xyz - vRight * In[0].fPSizeX * 0.5f + vUp * In[0].fPSizeY * 0.5f, 1.f);
	Out[1].vTexUV = float2(1.f, 0.f);

	// 오아
	Out[2].vPosition = vector(In[0].vPosition.xyz - vRight * In[0].fPSizeX * 0.5f - vUp * In[0].fPSizeY * 0.5f, 1.f);
	Out[2].vTexUV = float2(1.f, 1.f);

	// 왼아
	Out[3].vPosition = vector(In[0].vPosition.xyz + vRight * In[0].fPSizeX * 0.5f - vUp * In[0].fPSizeY * 0.5f, 1.f);
	Out[3].vTexUV = float2(0.f, 1.f);

	matrix		matVP;
	matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);	

	// 사각형의 첫번째 삼각형 
	OutStream.Append(Out[0]);
	OutStream.Append(Out[1]);
	OutStream.Append(Out[2]);
	OutStream.RestartStrip();

	// 사각형의 두번째 삼각형 
	OutStream.Append(Out[0]);
	OutStream.Append(Out[2]);
	OutStream.Append(Out[3]);
	OutStream.RestartStrip();
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

// 가로가 세로보다 10배만큼 길다
static const float uvOffsetX = 0.01f;
static const float uvOffsetY = 0.1f;

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// 기본 베이스는 검은색 
	Out.vColor = float4(0.f, 0.f, 0.f, 1.f);

	// 테두리 부분은 빨간색 
	if (In.vTexUV.x <= uvOffsetX ||
		In.vTexUV.x >= (1.f - uvOffsetX) ||
		In.vTexUV.y <= uvOffsetY ||
		In.vTexUV.y >= (1.f - uvOffsetY)
		)
	{
		Out.vColor = float4(1.f, 0.f, 0.f, 1.f);
	}
	
	// 몬스터 현재 체력은 빨간색.
	if (In.vTexUV.x <= (uvOffsetX + g_fMonsterHpUVX))
	{
		Out.vColor = float4(1.f, 0.f, 0.f, 1.f);
	}
	

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
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}
