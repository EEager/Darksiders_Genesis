
#include "Shader_Defines.hlsl"

cbuffer Matrices 
{
	matrix			gWorldMatrix;
	matrix			gViewMatrix;
	matrix			gProjMatrix;
};

cbuffer cbPerFrame
{
	float4 gEyePosW; // 카메라 위치 
	// for when the emit position/direction is varying
	float3 gEmitPosW;
	float3 gEmitColor;
	float2 gEmitSize;
	float gTimeStep;
	float gGameTime;
	vector gRandomDir;

	float gFloorHeight; // 파티클의 바닥 높이이다.

	float maxAge;

	bool EmitEnable;
};

cbuffer cbFixed
{
	// Net constant acceleration used to accerlate the particles.
	float3 gAccelW; // 강하게 부는 바람. 불같은 경우 위쪽으로 간다.
	float gRandomPwr; // 랜덤하게 가는 방향의 세기.

	// Texture coordinates used to stretch texture over quad 
	// when we expand point particle into a quad.
	float2 gQuadTexC[4] =
	{
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f)
	};
};

texture2D		g_DiffuseTexture;
Texture1D		g_RandomTex;

//***********************************************
// HELPER FUNCTIONS                             *
//***********************************************
float3 RandUnitVec3(float offset)
{
	// Use game time plus offset to sample random texture.
	float u = (gGameTime + offset);

	// coordinates in [-1,1]
	float3 v = g_RandomTex.SampleLevel(samLinear, u, 0).xyz;

	// project onto unit sphere
	return normalize(v);
}

float3 RandVec3(float offset)
{
	// Use game time plus offset to sample random texture.
	float u = (gGameTime + offset);

	// coordinates in [-1,1]
	float3 v = g_RandomTex.SampleLevel(samLinear, u, 0).xyz;

	return v;
}


//***********************************************
// STREAM-OUT TECH                              *
//***********************************************

#define PT_EMITTER 0
#define PT_FLARE 1

struct Particle
{
	float3 InitialPosW : POSITION;
	float3 InitialVelW : VELOCITY;
	float2 SizeW       : SIZE;
	float Age : AGE;
	uint Type          : TYPE;
};

Particle StreamOutVS(Particle vin)
{
	return vin;
}



// 51개짜리 한방 파티클
const static int maxVsize = 51;
[maxvertexcount(maxVsize)]
void StreamOutGS(point Particle gin[1],
	inout PointStream<Particle> ptStream)
{
	gin[0].Age += gTimeStep;

	if (gin[0].Type == PT_EMITTER)
	{
		for (int i = 0; i < maxVsize-1; ++i)
		{
			// 랜덤사용할경우
			float3 vRandom = RandVec3((float)i / (float)maxVsize-1);//  gRandomDir.xyz;

			Particle p;
			p.InitialPosW = gEmitPosW.xyz;
			p.InitialVelW = gRandomPwr * vRandom;
			p.SizeW = gEmitSize;
			p.Age = 0.0f;
			p.Type = PT_FLARE;

			ptStream.Append(p);
		}

		// reset the time to emit
		gin[0].Age = 0.0f;
	}
	else
	{
		// Specify conditions to keep particle; this may vary from system to system.
		// 아직 생존시간이 남아있으면 gs_5_0에 넣는다.
		if (gin[0].Age <= maxAge)
			ptStream.Append(gin[0]);
	}
}

// 100개지만 50만 넣는 한방짜리
const static int maxVsize_300 = 100; 
[maxvertexcount(maxVsize_300)]
void StreamOutGS_300(point Particle gin[1],
	inout PointStream<Particle> ptStream)
{
	gin[0].Age += gTimeStep;

	if (gin[0].Type == PT_EMITTER)
	{
		for (int i = 0; i < maxVsize - 1; ++i)
		{
			// 랜덤사용할경우
			float3 vRandom = RandVec3((float)i / (float)maxVsize - 1);//  gRandomDir.xyz;

			Particle p;
			p.InitialPosW = gEmitPosW.xyz;
			p.InitialVelW = gRandomPwr * vRandom;
			p.SizeW = gEmitSize;
			p.Age = 0.0f;
			p.Type = PT_FLARE;

			ptStream.Append(p);
		}

		// reset the time to emit
		gin[0].Age = 0.0f;
	}
	else
	{
		// Specify conditions to keep particle; this may vary from system to system.
		// 아직 생존시간이 남아있으면 gs_5_0에 넣는다.
		if (gin[0].Age <= maxAge)
			ptStream.Append(gin[0]);
	}
}

// 루프 방출
[maxvertexcount(2)]
void StreamOutGS_Loop(point Particle gin[1],
	inout PointStream<Particle> ptStream)
{
	gin[0].Age += gTimeStep;

	if (gin[0].Type == PT_EMITTER)
	{
		// time to emit a new particle?
		if (gin[0].Age > 0.005f)
		{
			float3 vRandom = RandVec3(0.0f);

			Particle p;
			p.InitialPosW = gEmitPosW.xyz;
			p.InitialVelW = gRandomPwr * vRandom;
			p.SizeW = gEmitSize;
			p.Age = 0.0f;
			p.Type = PT_FLARE;

			ptStream.Append(p);

			// reset the time to emit
			gin[0].Age = 0.0f;
		}

		// always keep emitters
		if (EmitEnable)
			ptStream.Append(gin[0]);
	}
	else
	{
		// Specify conditions to keep particle; this may vary from system to system.
		if (gin[0].Age <= maxAge)
			ptStream.Append(gin[0]);
	}
}

GeometryShader gsStreamOut = ConstructGSWithSO(
	CompileShader(gs_5_0, StreamOutGS()),
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x");

GeometryShader gsStreamOut_300 = ConstructGSWithSO(
	CompileShader(gs_5_0, StreamOutGS_300()),
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x");

GeometryShader gsStreamOut_Loop = ConstructGSWithSO(
	CompileShader(gs_5_0, StreamOutGS_Loop()),
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x");

technique11 StreamOutTech
{
	pass P0 // 한방 파티클 50개
	{
		SetVertexShader(CompileShader(vs_5_0, StreamOutVS()));
		SetGeometryShader(gsStreamOut);

		// disable pixel shader for stream-out only
		SetPixelShader(NULL);

		// we must also disable the depth buffer for stream-out only
		SetDepthStencilState(DisableDepth, 0);
	}

	pass P1 // 한방 파티클 300개
	{
		SetVertexShader(CompileShader(vs_5_0, StreamOutVS()));
		SetGeometryShader(gsStreamOut_300);

		// disable pixel shader for stream-out only
		SetPixelShader(NULL);

		// we must also disable the depth buffer for stream-out only
		SetDepthStencilState(DisableDepth, 0);
	}

	pass P2 // 계속 방출 
	{
		SetVertexShader(CompileShader(vs_5_0, StreamOutVS()));
		SetGeometryShader(gsStreamOut_Loop);

		// disable pixel shader for stream-out only
		SetPixelShader(NULL);

		// we must also disable the depth buffer for stream-out only
		SetDepthStencilState(DisableDepth, 0);
	}
}


//***********************************************
// DRAW TECH     								*
//***********************************************
struct VertexOut
{
	float3 PosW  : POSITION;
	float2 SizeW : SIZE;
	float4 Color : COLOR;
	uint   Type  : TYPE;
};

VertexOut DrawVS(Particle vin)
{
	VertexOut vout;

	float t = vin.Age;

	// constant acceleration equation
	vout.PosW = 0.5f * t * t * gAccelW + t * vin.InitialVelW + vin.InitialPosW;

	// fade color with time
	float opacity = 1.0f - smoothstep(0.0f, 1.0f, t / maxAge);
	//vout.Color = float4(1.f, 1.f, 1.f, opacity);
	vout.Color.xyz = gEmitColor;
	vout.Color.w = opacity;

	vout.SizeW = vin.SizeW;
	vout.Type = vin.Type;

	return vout;
}

VertexOut DrawVSSword(Particle vin)
{
	VertexOut vout;

	float t = vin.Age;

	// constant acceleration equation
	vout.PosW = 0.5f * t * t * gAccelW + t * vin.InitialVelW + vin.InitialPosW;

	// 플레이어 현재 월드 높이에 닿은 경우에는 높이는 움직이지 않는다. 
	vout.PosW.y = max(gFloorHeight, vout.PosW.y);

	// fade color with time
	float opacity = 1.0f - smoothstep(0.0f, 1.0f, t / maxAge);
	vout.Color.xyz = gEmitColor;
	vout.Color.w = opacity;

	vout.SizeW = vin.SizeW;
	vout.Type = vin.Type;

	return vout;
}

VertexOut DrawVSBlood(Particle vin)
{
	VertexOut vout;

	float t = vin.Age;

	// constant acceleration equation
	vout.PosW = 0.5f * t * t * gAccelW + t * vin.InitialVelW + vin.InitialPosW;
	vout.Color.xyz = gEmitColor; // 피의 경우 빨강일것이다.
	vout.Color.w = 1.f; // 피.
	vout.SizeW = vin.SizeW;
	vout.Type = vin.Type;
	return vout;
}

VertexOut DrawVSDashHorse(Particle vin)
{
	VertexOut vout;

	float t = vin.Age;

	// constant acceleration equation
	vout.PosW = 0.5f * t * t * gAccelW + t * vin.InitialVelW + vin.InitialPosW;
	// fade color with time
	float opacity = 1.0f - smoothstep(0.0f, 1.0f, t / maxAge);
	vout.Color.xyz = gEmitColor;
	vout.Color.w = opacity;

	vout.SizeW = vin.SizeW;
	vout.Type = vin.Type;

	return vout;
}



struct GeoOut
{
	float4 PosH  : SV_Position;
	float4 Color : COLOR;
	float2 Tex   : TEXCOORD;
};


// The draw GS just expands points into camera facing quads.
[maxvertexcount(4)]
void DrawGS(point VertexOut gin[1],
	inout TriangleStream<GeoOut> triStream)
{
	// do not draw emitter particles.
	if (gin[0].Type != PT_EMITTER)
	{
		//
		// Compute world matrix so that billboard faces the camera.
		//
		float3 look = normalize(gEyePosW.xyz - gin[0].PosW);
		float3 right = normalize(cross(float3(0, 1, 0), look));
		float3 up = cross(look, right);

		//
		// Compute triangle strip vertices (quad) in world space.
		//
		float halfWidth = 0.5f * gin[0].SizeW.x;
		float halfHeight = 0.5f * gin[0].SizeW.y;

		float4 v[4];
		v[0] = float4(gin[0].PosW + halfWidth * right - halfHeight * up, 1.0f);
		v[1] = float4(gin[0].PosW + halfWidth * right + halfHeight * up, 1.0f);
		v[2] = float4(gin[0].PosW - halfWidth * right - halfHeight * up, 1.0f);
		v[3] = float4(gin[0].PosW - halfWidth * right + halfHeight * up, 1.0f);

		//
		// Transform quad vertices to world space and output 
		// them as a triangle strip.
		//
		GeoOut gout;
		[unroll]
		for (int i = 0; i < 4; ++i)
		{
			matrix		matVP;
			matVP = mul(gViewMatrix, gProjMatrix);
			gout.PosH = mul(v[i], matVP);
			gout.Tex = gQuadTexC[i];
			gout.Color = gin[0].Color;
			triStream.Append(gout);
		}
	}
}


float4 DrawPS(GeoOut pin) : SV_TARGET
{
	return g_DiffuseTexture.Sample(DefaultSampler, pin.Tex)* pin.Color;
}

float4 DrawPSSword(GeoOut pin) : SV_TARGET
{
	return g_DiffuseTexture.Sample(DefaultSampler, pin.Tex) * pin.Color;
}

float4 DrawPSBlood(GeoOut pin) : SV_TARGET
{
	return g_DiffuseTexture.Sample(DefaultSampler, pin.Tex) * pin.Color;
}


technique11 DrawTech
{
	pass P0 // 처음꺼
	{
		SetVertexShader(CompileShader(vs_5_0, DrawVS()));
		SetGeometryShader(CompileShader(gs_5_0, DrawGS()));
		SetPixelShader(CompileShader(ps_5_0, DrawPS()));

		SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		SetDepthStencilState(NoDepthWrites, 0);
	}

	pass P1 // 검이 몹과 부딫혔을때 검에서 나오는 불꽃.
	{
		SetVertexShader(CompileShader(vs_5_0, DrawVSSword()));
		SetGeometryShader(CompileShader(gs_5_0, DrawGS()));
		SetPixelShader(CompileShader(ps_5_0, DrawPSSword()));

		SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		SetDepthStencilState(NoDepthWrites, 0);
	}

	pass P2 // 피
	{
		SetVertexShader(CompileShader(vs_5_0, DrawVSBlood()));
		SetGeometryShader(CompileShader(gs_5_0, DrawGS()));
		SetPixelShader(CompileShader(ps_5_0, DrawPSBlood()));

		SetBlendState(AlphaBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		SetDepthStencilState(NoDepthWrites, 0);
	}

	pass P3 // 대쉬 및 말 연기
	{
		SetVertexShader(CompileShader(vs_5_0, DrawVSDashHorse()));
		SetGeometryShader(CompileShader(gs_5_0, DrawGS()));
		SetPixelShader(CompileShader(ps_5_0, DrawPSSword()));

		SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		SetDepthStencilState(NoDepthWrites, 0);
	}


}