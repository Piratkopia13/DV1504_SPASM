#include "SailFont.h"

using namespace DirectX;

SailFont::SailFont(LPCWSTR fontFilename) {

	m_font = std::make_unique<SpriteFont>(Application::getInstance()->getDXManager()->getDevice(), (DEFAULT_SPRITEFONT_LOCATION+fontFilename).c_str());

}
SailFont::~SailFont() {}

DirectX::SpriteFont* SailFont::get() {
	return m_font.get();
}