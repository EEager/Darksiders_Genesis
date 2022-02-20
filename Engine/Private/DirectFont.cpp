#include "..\public\DirectFont.h"
#include "Graphic_Device.h"

IMPLEMENT_SINGLETON(CDirectFont)

ID2D1DeviceContext* CDirectFont::deviceContext = NULL;
IDWriteFactory* CDirectFont::writeFactory = NULL;
ID2D1Bitmap1* CDirectFont::targetBitmap = NULL;
IDXGISurface* CDirectFont::dxgiSurface = NULL;

vector<FontBrushDesc> CDirectFont::fontBrush;
vector<FontTextDesc> CDirectFont::fontText;


CDirectFont::CDirectFont()
{
	assert(CGraphic_Device::GetInstance()->GetDevice());
	assert(CGraphic_Device::GetInstance()->GetDC());

	HRESULT hr;

	// shared 쓰는 이유 3D랑 동일 시키려고 쓰는거
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&writeFactory);
	assert(SUCCEEDED(hr));

	D2D1_FACTORY_OPTIONS option;
	option.debugLevel = D2D1_DEBUG_LEVEL_NONE;
#ifdef _DEBUG
	option.debugLevel = D2D1_DEBUG_LEVEL_ERROR;
#endif
	// 2D때는 싱글 스레드로해도 문제없는데 3D면 싱크 안맞으면 터져버림
	//hr = D2D1CreateFactory(
	//	D2D1_FACTORY_TYPE_MULTI_THREADED, &factory);

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, option, &factory);
	assert(SUCCEEDED(hr));

	// DirectX Graphic Interface - GPU 
	IDXGIDevice* dxgiDevice;  
	hr = CGraphic_Device::GetInstance()->GetDevice()->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	assert(SUCCEEDED(hr));

	hr = factory->CreateDevice(dxgiDevice, &device);
	assert(SUCCEEDED(hr));

	dxgiDevice->Release();

	hr = device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS,
		&deviceContext);
	assert(SUCCEEDED(hr));

	CreateSurface();

}

void CDirectFont::CreateSurface()
{
	HRESULT hr = CGraphic_Device::GetInstance()->GetSwapChain()->GetBuffer(0, __uuidof(IDXGISurface), (void**)&dxgiSurface);
	assert(SUCCEEDED(hr));

	D2D1_BITMAP_PROPERTIES1 bp;
	bp.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE; // 알파 안쓸꺼
													   // dot per inch 실제 윈도우 크기가 다르더라도 맞추려고 쓰는거
	bp.dpiX = 96;
	bp.dpiY = 96;
	bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	bp.colorContext = NULL;

	hr = deviceContext->CreateBitmapFromDxgiSurface(dxgiSurface, &bp, &targetBitmap);
	assert(SUCCEEDED(hr));

	deviceContext->SetTarget(targetBitmap);
}

void CDirectFont::DeleteSurface()
{
	deviceContext->SetTarget(NULL);

	for (FontBrushDesc desc : fontBrush)
		Safe_Release(desc.Brush);
	fontBrush.clear();

	for (FontTextDesc desc : fontText)
		Safe_Release(desc.Format);
	fontText.clear();

	Safe_Release(targetBitmap);
	Safe_Release(dxgiSurface);
}

void CDirectFont::RenderText(wstring text, RECT rect, float size, wstring font, _float4 color, DWRITE_FONT_WEIGHT weight, DWRITE_FONT_STYLE style, DWRITE_FONT_STRETCH stretch)
{
	FontBrushDesc brushDesc;
	brushDesc.Color = color;

	FontBrushDesc* findBrush = NULL;
	for (FontBrushDesc& desc : fontBrush)
	{
		if (desc == brushDesc)
		{
			findBrush = &desc;
			break;
		}
	}

	if (findBrush == NULL)
	{
		D2D1::ColorF colorF = D2D1::ColorF(color.x, color.y, color.z);
		deviceContext->CreateSolidColorBrush(colorF, &brushDesc.Brush);

		fontBrush.push_back(brushDesc);
		findBrush = &brushDesc;
	}

	FontTextDesc textDesc;
	textDesc.Font = font;
	textDesc.FontSize = size;
	textDesc.Stretch = stretch;
	textDesc.Style = style;
	textDesc.Weight = weight;

	FontTextDesc* findText = NULL;
	for (FontTextDesc& desc : fontText)
	{
		if (desc == textDesc)
		{
			findText = &desc;
			break;
		}
	}

	if (findText == NULL)
	{
		writeFactory->CreateTextFormat(
			textDesc.Font.c_str(), NULL, textDesc.Weight, textDesc.Style, textDesc.Stretch,
			textDesc.FontSize, L"ko", &textDesc.Format
		);

		fontText.push_back(textDesc);
		findText = &textDesc;
	}

	D2D1_RECT_F rectF;
	rectF.left = (float)rect.left;
	rectF.top = (float)rect.top;
	rectF.right = (float)rect.right;
	rectF.bottom = (float)rect.bottom;

	deviceContext->DrawTextW(
		text.c_str(), text.size(), findText->Format, rectF, findBrush->Brush
	);
}

void CDirectFont::Free()
{
	DeleteSurface();

	Safe_Release(factory);
	Safe_Release(writeFactory);

	Safe_Release(deviceContext);
	Safe_Release(device);
}
