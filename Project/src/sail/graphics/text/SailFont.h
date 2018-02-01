#pragma once

#include <d3d11.h>
#include <string>
#include <SimpleMath.h>
#include <SpriteFont.h>

#include "../../Application.h"
#include "../../DXManager.h"

namespace {
	static const std::wstring DEFAULT_SPRITEFONT_LOCATION = L"res/fonts/";
}

class SailFont {

public:
	SailFont(LPCWSTR fontFilename = L"courierNew.spritefont");
	~SailFont();

	DirectX::SpriteFont* get();

private:
	std::unique_ptr<DirectX::SpriteFont> m_font;
	

};