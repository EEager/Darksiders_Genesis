#pragma once
#include "Base.h"

BEGIN(Engine)

#ifndef EASINGLERP
#define EASINGLERP

class ENGINE_DLL EasingLerp final
{
public:		enum class EaseType
{
	easeLiner,
	easeInSine,
	easeOutSine,
	easeInOutSine,
	easeInQuad,
	easeOutQuad,
	easeInOutQuad,
	easeInCubic,
	easeOutCubic,
	easeInOutCubic,
	easeInQuart,
	easeOutQuart,
	easeInOutQuart,
	easeInQuint,
	easeOutQuint,
	easeInOutQuint,
	easeInExpo,
	easeOutExpo,
	easeInOutExpo,
	easeInCirc,
	easeOutCirc,
	easeInOutCirc,
	easeInBack,
	easeOutBack,
	easeInOutBack,
	easeInElastic,
	easeOutElastic,
	easeInOutElastic,
	easeInBounce,
	easeOutBounce,
	easeInOutBounce
};

public:		static float	Lerp(float from, float to, float t, EaseType easeType = EaseType::easeLiner) noexcept;
public:		static float	GetEaseTime(float t, EaseType easeType) noexcept;

public:		static float	easeLiner(float t) noexcept;
public:		static float	easeInSine(float t) noexcept;
public:		static float	easeOutSine(float t) noexcept;
public:		static float	easeInOutSine(float t) noexcept;
public:		static float	easeInQuad(float t) noexcept;
public:		static float	easeOutQuad(float t) noexcept;
public:		static float	easeInOutQuad(float t) noexcept;
public:		static float	easeInCubic(float t) noexcept;
public:		static float	easeOutCubic(float t) noexcept;
public:		static float	easeInOutCubic(float t) noexcept;
public:		static float	easeInQuart(float t) noexcept;
public:		static float	easeOutQuart(float t) noexcept;
public:		static float	easeInOutQuart(float t) noexcept;
public:		static float	easeInQuint(float t) noexcept;
public:		static float	easeOutQuint(float t) noexcept;
public:		static float	easeInOutQuint(float t) noexcept;
public:		static float	easeInExpo(float t) noexcept;
public:		static float	easeOutExpo(float t) noexcept;
public:		static float	easeInOutExpo(float t) noexcept;
public:		static float	easeInCirc(float t) noexcept;
public:		static float	easeOutCirc(float t) noexcept;
public:		static float	easeInOutCirc(float t) noexcept;
public:		static float	easeInBack(float t) noexcept;
public:		static float	easeOutBack(float t) noexcept;
public:		static float	easeInOutBack(float t) noexcept;
public:		static float	easeInElastic(float t) noexcept;
public:		static float	easeOutElastic(float t) noexcept;
public:		static float	easeInOutElastic(float t) noexcept;
public:		static float	easeInBounce(float t) noexcept;
public:		static float	easeOutBounce(float t) noexcept;
public:		static float	easeInOutBounce(float t) noexcept;
};

#endif

END