#pragma once

#include "Base.h"

BEGIN(Engine)

//***************************************************************************************
// MathHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper math class.
//***************************************************************************************

#ifndef MATHHELPER_H
#define MATHHELPER_H

class ENGINE_DLL MathHelper final
{
public:
	// Returns random float in [0, 1).
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	// Returns random float in [a, b).
	static float RandF(float a, float b)
	{
		return a + RandF() * (b - a);
	}

	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	template<typename T>
	static T Lerp(const T& a, const T& b, float t)
	{
		return a + (b - a) * t;
	}

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x);
	}

	template<typename T>
	static T Saturate(const T& x)
	{
		// Set <0 to 0
		// Set >1 to 1
		return Min<T>(Max<T>(x, 0), 1);
	}

	//// Returns the polar angle of the point (x,y) in [0, 2*PI).
	//static float AngleFromXY(float x, float y);

	//static XMMATRIX InverseTranspose(CXMMATRIX M)
	//{
	//	// Inverse-transpose is just applied to normals.  So zero out 
	//	// translation row so that it doesn't get into our inverse-transpose
	//	// calculation--we don't want the inverse-transpose of the translation.
	//	XMMATRIX A = M;
	//	A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	//	XMVECTOR det = XMMatrixDeterminant(A);
	//	return XMMatrixTranspose(XMMatrixInverse(&det, A));
	//}

	//static XMVECTOR RandHemisphereUnitVec3(XMVECTOR n);

	static XMVECTOR RandUnitVec3()
	{
		XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		XMVECTOR Zero = XMVectorZero();

		// Keep trying until we get a point on/in the hemisphere.
		while (true)
		{
			// Generate random point in the cube [-1,1]^3.
			XMVECTOR v = XMVectorSet(MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), 0.0f);

			// Ignore points outside the unit sphere in order to get an even distribution 
			// over the unit sphere.  Otherwise points will clump more on the sphere near 
			// the corners of the cube.

			if (XMVector3Greater(XMVector3LengthSq(v), One))
				continue;

			return XMVector3Normalize(v);
		}
	}


	static const float Infinity;
	static const float Pi;


};
#endif // MATHHELPER_H

END