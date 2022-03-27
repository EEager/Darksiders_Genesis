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

static const unsigned int		g_iWinCX = 1600;
static const unsigned int		g_iWinCY = 900;

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
//#include "LightHelper.h" // Move to Engine

// -----------------------
// Client_Extern.h
// -----------------------
#include "Client_Extern.h" 



// -------------------
// For the Test
// -------------------
//#define ONLY_WAR


// -----------------------
// For VIBuffer_Terrain, Terrain, CEnviroment
// ------------------------
#define Z_MAPSIZE 700
#define X_MAPSIZE 800

#define ENVIROMENT_OFFSET_X 20
#define ENVIROMENT_OFFSET_Z 400
