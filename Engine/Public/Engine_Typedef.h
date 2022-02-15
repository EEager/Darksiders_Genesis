#pragma once

namespace Engine
{
	typedef unsigned char			_ubyte;
	typedef signed char				_byte;

	typedef unsigned short			_ushort;
	typedef signed short			_short;

	typedef unsigned int			_uint;
	typedef signed int				_int;

	typedef unsigned long			_ulong;
	typedef signed long				_long;

	typedef float					_float;
	typedef double					_double;

	typedef bool					_bool;
	typedef wchar_t					_tchar;

	typedef XMFLOAT2				_float2;
	typedef XMFLOAT3				_float3;


	typedef struct tagFloat4_Derived : public XMFLOAT4
	{
		tagFloat4_Derived() { }
		tagFloat4_Derived(_float _x, _float _y, _float _z, _float _w) 
		: XMFLOAT4(_x, _y, _z, _w)
		{

		}

		tagFloat4_Derived(_float3 vFloat3, _float fW)
		:  XMFLOAT4(vFloat3.x, vFloat3.y, vFloat3.z, fW)
		{ 

		}
	}_float4;
	
	typedef XMVECTOR				_vector;
	typedef FXMVECTOR				_fvector;

	typedef XMFLOAT4X4				_float4x4;	
	typedef XMMATRIX				_matrix;
	typedef FXMMATRIX				_fmatrix;

}
