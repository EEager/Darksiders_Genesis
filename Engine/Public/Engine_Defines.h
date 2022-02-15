#pragma once

#pragma warning (disable : 4251)

#include <d3d11.h>
#include <typeinfo>

#include <DirectXMath.h>
#include <d3dx11effect.h>
#include <d3dcompiler.h>
#include "DirectXTex.h"
using namespace DirectX;


#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <process.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>


using namespace std;


// -----------------------
//	ImGUI
// -----------------------
#define USE_IMGUI



#ifndef USE_IMGUI

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // USE_IMGUI


#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"


namespace Engine { }

using namespace Engine;