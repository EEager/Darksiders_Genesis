
#ifndef _SHADER_DEFINE
#define _SHADER_DEFINE

struct Material
{
	vector vMtrlDiffuse;
	vector vMtrlAmbient;
	vector vMtrlSpecular; // w = SpecPower
	vector vMtrlReflect;
	vector vMtrlEmissive;
	float  fMtrlPower;
	float3 pad;
};

#endif
