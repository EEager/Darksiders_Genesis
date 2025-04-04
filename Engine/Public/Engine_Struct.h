#pragma once

namespace Engine
{
	typedef struct tagKeyFrame
	{
		struct tagKeyFrame()
		{
			ZeroMemory(this, sizeof(this));
		}

		_float3		vScale;
		_float4		vRotation;
		_float3		vPosition;
		_double		Time;
	}KEYFRAME;

	typedef struct tagMeshMaterial
	{
		class CTexture* pTexture[AI_TEXTURE_TYPE_MAX];
	}MESHMATERIAL;

	typedef struct tagMaterialDesc
	{
		tagMaterialDesc()
			: 
			vMtrlDiffuse(XMVectorSplatOne()),
			vMtrlAmbient(XMVectorSplatOne()),
			vMtrlSpecular(XMVectorSplatOne()),
			vMtrlReflect(XMVectorSplatOne()),
			vMtrlEmissive(XMVectorSplatOne()),
			fMtrlPower(20.f)
		{}

		XMVECTOR			vMtrlDiffuse; // 방향
		XMVECTOR			vMtrlAmbient; 
		XMVECTOR			vMtrlSpecular; 
		XMVECTOR			vMtrlReflect; 
		XMVECTOR			vMtrlEmissive;
		_float				fMtrlPower; 
		_float3				pad;
	}MTRLDESC;

	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOT, TYPE_END };

		TYPE				eType;
		XMFLOAT3			vDirection; // 방향
		XMFLOAT3			vPosition; // 위치
		float				fRadiuse; // 거리? 

		XMFLOAT4			vDiffuse;
		XMFLOAT4			vAmbient;
		XMFLOAT4			vSpecular;

	}LIGHTDESC;

	typedef struct tagPassDesc
	{
		ComPtr<ID3D11InputLayout>		pInputlayout = nullptr;
		ComPtr<ID3DX11EffectPass>		pPass = nullptr;

	}PASSDESC;

	// ------------------
	// Buffer Struct Types
	// ------------------
	typedef struct tagParticle
	{
		XMFLOAT3 InitialPos;
		XMFLOAT3 InitialVel;
		XMFLOAT2 Size;
		float Age;
		unsigned int Type;
	}VTXPTC;

	typedef struct tagVertex_Matrix
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vPosition;
	}VTXMATRIX;

	typedef struct tagVertex_Mesh
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
		XMFLOAT3			vTangent;		
	}VTXMESH;

	typedef struct tagVertex_Anim : public VTXMESH
	{
		XMUINT4				vBlendIndex;
		XMFLOAT4			vBlendWeight;
	}VTXMESH_ANIM;

	typedef struct tagVertex_Color
	{
		XMFLOAT3			vPosition;
		XMFLOAT4			vColor;
	} VTXCOL;

	typedef struct tagVertex_Point
	{
		XMFLOAT3			vPosition;
		float				fPSize;
	} VTXPOINT;

	typedef struct tagVertex_Point_HpBar
	{
		XMFLOAT3			vPosition;
		float				fPSizeX;
		float				fPSizeY;

		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;

	} VTXPOINTHPBAR;

	typedef struct tagVertexCubeTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexUV;
	} VTXCUBETEX;

	typedef struct tagVertex_Texture
	{
		XMFLOAT3			vPosition;
		XMFLOAT2			vTexUV;
	} VTXTEX;

	typedef struct tagVertex_Normal_Texture
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
	} VTXNORTEX;

	typedef struct tagVertex
	{
		tagVertex()
			: vPosition(0.f, 0.f, 0.f) {}
		tagVertex(_float3 pos)
			: vPosition(pos) {}
		tagVertex(float px, float py, float pz)
			: vPosition(px, py, pz) {}
		XMFLOAT3			vPosition;
	} VTX;

	

	typedef struct tagLineIndices16
	{
		unsigned short		_0, _1;
	} LINEINDICES16;

	typedef struct tagFaceIndices16
	{
		unsigned short		_0, _1, _2;
	} FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long		_0, _1, _2;
	} FACEINDICES32;


	// ------------------
	// For Magic Struct 
	// ------------------
	enum class MagicKeyType
	{
		MAGIC_TYPE_AUTOGENNAVI,
	};

	typedef struct tagMagicAutoGenNavi
	{
		tagMagicAutoGenNavi()
			: eMagicKey(MagicKeyType::MAGIC_TYPE_AUTOGENNAVI) {}
		MagicKeyType eMagicKey;
		vector<_float> vecHeight;
	} MAGIC_AUTOGENNAVI;

	// ---------------------
	// For GameInfo Struct
	// ---------------------
	typedef struct tagGameInfo
	{
		struct tagGameInfo()
		{
			ZeroMemory(this, sizeof(this));
		}
		int		iAtt;
		int		iMaxHp;		// 최대체력 0이면 무적
		int		iHp;		// 현재체력

		// 죽으면 나오는것들
		int		iEnergy;   
		int		iSoul;		// 소울이라는것이있때
	}GAMEINFO;


	// ---------------
	// LightHelper에 사용되는 구조체
	// ---------------
	struct DirectionalLight
	{
		DirectionalLight() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Specular;
		XMFLOAT3 Direction;
		float Pad; // Pad the last float so we can set an array of lights if we wanted.
	};

	struct PointLight
	{
		PointLight() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Specular;

		// Packed into 4D vector: (Position, Range)
		XMFLOAT3 Position;
		float Range;

		// Packed into 4D vector: (A0, A1, A2, Pad)
		XMFLOAT3 Att;
		float Pad; // Pad the last float so we can set an array of lights if we wanted.
	};

	struct SpotLight
	{
		SpotLight() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Specular;

		// Packed into 4D vector: (Position, Range)
		XMFLOAT3 Position;
		float Range;

		// Packed into 4D vector: (Direction, Spot)
		XMFLOAT3 Direction;
		float Spot;

		// Packed into 4D vector: (Att, Pad)
		XMFLOAT3 Att;
		float Pad; // Pad the last float so we can set an array of lights if we wanted.
	};
}

