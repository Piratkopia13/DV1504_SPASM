#pragma once

// Settings
#define _SAIL_THROW_ON_WARNING
#define _SAIL_THROW_ON_ERROR

// Exclude some less used APIs to speed up the build process
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <memory>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>
#include <string>
#include <Memory>
#include <comdef.h> 
#include <iostream>
// DirectX Toolkit includes
#include <SimpleMath.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <GamePad.h>

#include "Application.h"
#include "Utils/Utils.h"
#include "graphics/Scene.h"
#include "graphics/text/Text.h"
#include "graphics/text/SailFont.h"
#include "graphics/camera/PerspectiveCamera.h"
#include "graphics/camera/OrthographicCamera.h"
#include "graphics/camera/FlyingCameraController.h"
#include "graphics/geometry/Model.h"
#include "graphics/shader/basic/SimpleColorShader.h"
#include "graphics/shader/basic/SimpleTextureShader.h"
#include "graphics/shader/basic/DirectionalLightShader.h"
#include "graphics/shader/basic/CubeMapShader.h"
#include "graphics/shader/material/MaterialShader.h"
#include "graphics/shader/instanced/ParticleShader.h"
#include "graphics/geometry/factory/CubeModel.h"
#include "graphics/geometry/factory/PlaneModel.h"
#include "graphics/models/FbxModel.h"
#include "graphics/RenderableTexture.h"
#include "graphics/Skybox.h"
#include "graphics/renderer/DeferredRenderer.h"
#include "graphics/ParticleEmitter.h"
#include "resources/DXCubeMap.h"
#include "resources/audio/SoundManager.h"
#include "states/StateStack.h"