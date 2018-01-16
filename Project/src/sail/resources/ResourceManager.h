#pragma once

#include <Map>
#include "TextureData.h"
#include "DXTexture.h"
#include "parsers/FBXParser.h"

class ResourceManager {
public:
	ResourceManager() {}
	~ResourceManager() {}

	//
	// TextureData
	//

	void LoadTextureData(const std::string& filename) {
		m_textureDatas.insert({filename, std::make_unique<TextureData>(filename)});
	}
	TextureData& getTextureData(const std::string& filename) {
		auto pos = m_textureDatas.find(filename);
		if (pos == m_textureDatas.end())
			Logger::Error("Tried to access a resource that was not loaded. (" + filename + ") \n Use Application::getInstance()->getResourceManager().LoadTextureData(\"filename\") before accessing it.");
		
		return *pos->second;
	}
	bool hasTextureData(const std::string& filename) {
		return m_textureDatas.find(filename) != m_textureDatas.end();
	}

	//
	// DXTexture
	//

	void LoadDXTexture(const std::string& filename) {
		m_dxTextures.insert({ filename, std::make_unique<DXTexture>(filename) });
	}
	DXTexture& getDXTexture(const std::string& filename) {
		auto pos = m_dxTextures.find(filename);
		if (pos == m_dxTextures.end())
			Logger::Error("Tried to access a resource that was not loaded. (" + filename + ") \n Use Application::getInstance()->getResourceManager().LoadDXTexture(" + filename + ") before accessing it.");

		return *pos->second;
	}
	bool hasDXTexture(const std::string& filename) {
		return m_dxTextures.find(filename) != m_dxTextures.end();
	}


	// 
	// FBXParser
	//

	FBXParser& getFBXParser() {
		return m_fbxParser;
	}

private:
	std::map<std::string, std::unique_ptr<TextureData>> m_textureDatas;
	std::map<std::string, std::unique_ptr<DXTexture>> m_dxTextures;

	FBXParser m_fbxParser;
};