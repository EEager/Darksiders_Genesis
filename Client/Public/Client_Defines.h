#pragma once



// -----------------------
// Level 
// -----------------------
enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };
// Same as ¡ã and ¡å 
static const char* g_level_items[] = { "LEVEL_STATIC", "LEVEL_LOADING", "LEVEL_LOGO",  "LEVEL_GAMEPLAY" ,"LEVEL_END" };



// -----------------------
// global variables
// -----------------------
namespace Client {}
using namespace Client;

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

static const unsigned int		g_iWinCX = 1280;
static const unsigned int		g_iWinCY = 720;

// -----------------------
// For Console Debug
// -----------------------
#ifdef _DEBUG
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#endif

// -----------------------
// LightHelper.h
// -----------------------
#include "LightHelper.h"
