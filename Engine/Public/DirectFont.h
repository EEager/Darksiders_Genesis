#pragma once

#include "Base.h"

BEGIN(Engine)

struct FontBrushDesc;
struct FontTextDesc;

class ENGINE_DLL CDirectFont final : public CBase
{
	DECLARE_SINGLETON(CDirectFont)
private:
	CDirectFont();
	virtual ~CDirectFont() = default;

public:
	static ID2D1DeviceContext* GetFontDC() { return deviceContext; }

private: // VM_SIZE의 경우 외부에서 사용할 수 도 있음
	void CreateSurface();
	void DeleteSurface();


private:
	ID2D1Factory1* factory;
	static IDWriteFactory* writeFactory;

	ID2D1Device* device;
	static ID2D1DeviceContext* deviceContext;

	static ID2D1Bitmap1* targetBitmap;
	static IDXGISurface* dxgiSurface;

private:
	static vector<FontBrushDesc> fontBrush;
	static vector<FontTextDesc> fontText;

public:
	static void RenderText(wstring text, RECT rect,
		float size = 20.0f, wstring font = L"돋움체", _float4 color = _float4(1.f, 1.f, 1.f, 1.f)
		, DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL
		, DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL
		, DWRITE_FONT_STRETCH stretch = DWRITE_FONT_STRETCH_NORMAL);

public:
	virtual void Free() override;
};


struct FontBrushDesc
{
	_float4 Color;
	ID2D1SolidColorBrush* Brush = NULL;

	bool operator==(const FontBrushDesc& val)
	{
		return Color.x == val.Color.x &&
			Color.y == val.Color.y &&
			Color.z == val.Color.z &&
			Color.w == val.Color.w;
	}
};

struct FontTextDesc
{
	wstring Font;
	float FontSize;

	DWRITE_FONT_WEIGHT Weight;
	DWRITE_FONT_STYLE Style;
	DWRITE_FONT_STRETCH Stretch;

	IDWriteTextFormat* Format = NULL;

	bool operator==(const FontTextDesc& val)
	{
		bool b = true;
		b &= Font == val.Font;
		b &= FontSize == val.FontSize;
		b &= Weight == val.Weight;
		b &= Style == val.Style;
		b &= Stretch == val.Stretch;

		return b;
	}
};

END