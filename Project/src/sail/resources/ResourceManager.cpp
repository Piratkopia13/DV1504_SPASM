#include "ResourceManager.h"
#include "../graphics/models/FbxModel.h"
#include "../graphics/shader/deferred/DeferredGeometryShader.h"

ResourceManager::ResourceManager() {}
ResourceManager::~ResourceManager() {}

//
// TextureData
//

void ResourceManager::LoadTextureData(const std::string& filename) {
	m_textureDatas.insert({ filename, std::make_unique<TextureData>(filename) });
}
TextureData& ResourceManager::getTextureData(const std::string& filename) {
	auto pos = m_textureDatas.find(filename);
	if (pos == m_textureDatas.end())
		Logger::Error("Tried to access a resource that was not loaded. (" + filename + ") \n Use Application::getInstance()->getResourceManager().LoadTextureData(\"filename\") before accessing it.");

	return *pos->second;
}
bool ResourceManager::hasTextureData(const std::string& filename) {
	return m_textureDatas.find(filename) != m_textureDatas.end();
}

//
// DXTexture
//

void ResourceManager::LoadDXTexture(const std::string& filename) {
	m_dxTextures.insert({ filename, std::make_unique<DXTexture>(filename) });
}
DXTexture& ResourceManager::getDXTexture(const std::string& filename) {
	auto pos = m_dxTextures.find(filename);
	if (pos == m_dxTextures.end())
		Logger::Error("Tried to access a resource that was not loaded. (" + filename + ") \n Use Application::getInstance()->getResourceManager().LoadDXTexture(" + filename + ") before accessing it.");

	return *pos->second;
}
bool ResourceManager::hasDXTexture(const std::string& filename) {
	return m_dxTextures.find(filename) != m_dxTextures.end();
}


// 
// FBXParser
//

FBXParser& ResourceManager::getFBXParser() {
	return m_fbxParser;
}

//
// FBXModels
//

void ResourceManager::LoadFBXModel(const std::string& filename) {
	// Insert and get the new model
	FbxModel* model = m_fbxModels.insert({ filename, std::make_unique<FbxModel>(filename + ".fbx") }).first->second.get();
	model->getModel()->buildBufferForShader(&getShaderSet<DeferredGeometryShader>());
}
FbxModel& ResourceManager::getFBXModel(const std::string& filename) {
	auto pos = m_fbxModels.find(filename);
	if (pos == m_fbxModels.end()) {
		// Model was not yet loaded, load it and return
		LoadFBXModel(filename);
		
		return *m_fbxModels.find(filename)->second;
		//Logger::Error("Tried to access an fbx model that was not loaded. (" + filename + ") \n Use Application::getInstance()->getResourceManager().LoadFBXModel(" + filename + ") before accessing it.");
	}

	return *pos->second;
}
bool ResourceManager::hasFBXModel(const std::string& filename) {
	return m_fbxModels.find(filename) != m_fbxModels.end();
}
