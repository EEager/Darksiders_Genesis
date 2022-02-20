#pragma once

static const unsigned int		g_iWinCX = 1280;
static const unsigned int		g_iWinCY = 720;

#pragma warning (disable : 4251)

// -----------------------
// DirectX
// -----------------------
#include <d3d11.h>
#include <typeinfo>

#include <DirectXMath.h>
#include <d3dx11effect.h>
#include <d3dcompiler.h>
#include "DirectXTex.h"
using namespace DirectX;

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>


// -----------------------
// Font
// -----------------------
#include <d2d1_1.h>
#include <dwrite.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

// -----------------------
// Library
// -----------------------
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <process.h>
using namespace std;

// -----------------------
// Assimp
// -----------------------
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>



// -----------------------
//	ImGUI
// -----------------------
#ifdef _DEBUG
#define USE_IMGUI
#endif

#ifndef USE_IMGUI

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // USE_IMGUI



// -----------------------
//	Engine
// -----------------------
#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"


namespace Engine { }

using namespace Engine;