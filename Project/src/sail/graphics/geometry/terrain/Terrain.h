#pragma once

#include <memory>
#include "../Model.h"
#include "../Material.h"
#include "../../../resources/TextureData.h"

class Terrain/* : public Renderable*/ {
public:
	Terrain(const std::string& heightMapFilename, ShaderSet* shader, unsigned int mapSize = 20, float maxHeight = 5.f);

	Model& getModel();
	float getHeightAtWorldCoords(float x, float z);

private:
	// Returns the height at the x and y(z) pixel coordinates of the image
	float getHeightAt(unsigned int x, unsigned int z);
	DirectX::SimpleMath::Vector3 calculateNormal(unsigned int x, unsigned int z);
	float getHeightInTriangle(const DirectX::SimpleMath::Vector3& p0, const DirectX::SimpleMath::Vector3& p1, const DirectX::SimpleMath::Vector3& p2, float x, float z);

private:
	Model m_model;
	TextureData* m_heightMap;

	// Map width and height in pixels
	int m_mapSize;

	// Number of triangles wide
	unsigned int m_mapResolution;
	// Lenght of each triangle's side
	float m_triangleSide;

	// Maximum height (0 is the lowest)
	float m_maxHeight;

};

