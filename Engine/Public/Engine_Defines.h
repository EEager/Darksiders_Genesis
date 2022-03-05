#pragma once

#pragma warning (disable : 4251)

//#pragma warning (disable : 2579)
//#pragma warning (disable : 2599)
//#pragma warning (disable : 3308)

#pragma warning (disable : 4099) // warning LNK4099: 'Effects11d.pdb' PDB를 'Effects11d.lib(d3dxGlobal.obj)' 또는 'C:\Users\JJLEE\Desktop\DarksidersGenesis\Darksiders_Genesis\Engine\Bin\Effects11d.pdb'에서 찾을 수 없습니다.
#pragma warning (disable : 4244)



// -----------------------
// DirectX
// -----------------------
#include <d3d11.h>
#include <typeinfo>

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <d3dx11effect.h>
#include <d3dcompiler.h>

// DirectXTex
#include "DirectXTex.h"
using namespace DirectX;


// 물방울책
#include <DirectXPackedVector.h>
using namespace DirectX::PackedVector;

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

// -------------------
// DirectXTK
// -------------------
#ifdef _DEBUG
#pragma comment(lib, "DirectXTKd.lib")
#else
#pragma comment(lib, "DirectXTK.lib")
#endif
#include <DirectXTK\PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/Effects.h>

// Font
#include <DirectXTK\SpriteBatch.h>
#include <DirectXTK\SpriteFont.h>

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


#include <wrl.h>
using namespace Microsoft::WRL;

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