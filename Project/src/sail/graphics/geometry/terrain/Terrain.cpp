#include "Terrain.h"
#include "../../../Application.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Terrain::Terrain(const std::string& heightMapFilename, ShaderSet* shader, unsigned int mapSize, float maxHeight)
	: m_mapSize(mapSize)
	, m_maxHeight(maxHeight)
{
	
	// Load the texture file it if is not loaded already
	if (!Application::getInstance()->getResourceManager().hasTextureData(heightMapFilename)) {
		Application::getInstance()->getResourceManager().LoadTextureData(heightMapFilename);
	}
	m_heightMap = &Application::getInstance()->getResourceManager().getTextureData(heightMapFilename);

	// Number of triangles wide
	m_mapResolution = m_heightMap->getWidth() - 1;
	// Lenght of each triangle's side
	m_triangleSide = (float)m_mapSize / m_mapResolution;

	const int numVerts = (m_mapResolution + 1) * (m_mapResolution + 1);
	const int numIndices = m_mapResolution * m_mapResolution * 6;

	Vector3* positions = new Vector3[numVerts];
	Vector3* normals = new Vector3[numVerts];
	Vector2* texCoords = new Vector2[numVerts];
	Vector3* tangents = new Vector3[numVerts];
	Vector3* bitangents = new Vector3[numVerts];
	ULONG* indices = new ULONG[numIndices];

	float tile = 100.f;

	// Loop through every vertex
	for (unsigned int z = 0; z < m_mapResolution + 1; z++) {
		for (unsigned int x = 0; x < m_mapResolution + 1; x++) {

			float height = getHeightAt(x, z);

			unsigned int vertexIndex = (z * (m_mapResolution + 1) + x);

			positions[vertexIndex] = Vector3(x * m_triangleSide, height, z * -m_triangleSide);
			normals[vertexIndex] = calculateNormal(x, z);
			texCoords[vertexIndex] = Vector2((float)x / m_mapResolution, (float)z / m_mapResolution) * tile;
			tangents[vertexIndex] = Vector3(1.f, 0.f, 0.f);
			bitangents[vertexIndex] = normals[vertexIndex].Cross(tangents[vertexIndex]);

		}
	}

	// Loop through every grid square (with two trianges each)
	for (unsigned int z = 0; z < m_mapResolution; z++) {
		for (unsigned int x = 0; x < m_mapResolution; x++) {

			ULONG topLeft = (z * (m_mapResolution + 1) + x);
			ULONG topRight = topLeft + 1;
			ULONG bottomLeft = ((z + 1) * (m_mapResolution + 1) + x);
			ULONG bottomRight = bottomLeft + 1;

			unsigned int indexIndex = (z * m_mapResolution + x) * 6;
			indices[indexIndex + 0] = topLeft;
			indices[indexIndex + 1] = topRight;
			indices[indexIndex + 2] = bottomLeft;
			indices[indexIndex + 3] = topRight;
			indices[indexIndex + 4] = bottomRight;
			indices[indexIndex + 5] = bottomLeft;

		}
	}

	Model::Data buildData;
	buildData.numVertices = numVerts;
	buildData.positions = positions;
	buildData.numIndices = numIndices;
	buildData.indices = indices;
	buildData.normals = normals;
	buildData.texCoords = texCoords;
	buildData.tangents = tangents;
	buildData.bitangents = bitangents;

	m_model.setBuildData(buildData);
	m_model.buildBufferForShader(shader);

	// Set the color to some mountain-ish looking
	m_model.getMaterial()->setColor(Vector4(.47f, .52f, .44f, 1.0f));

}

Model& Terrain::getModel() {
	return m_model;
}

float Terrain::getHeightAt(unsigned int x, unsigned int z) {

	return m_heightMap->getPixel(x, z).x / 255.f * m_maxHeight;

}

Vector3 Terrain::calculateNormal(unsigned int x, unsigned int z) {
	float heightL = getHeightAt(x - 1, z);
	float heightR = getHeightAt(x + 1, z);
	float heightU = getHeightAt(x, z - 1);
	float heightD = getHeightAt(x, z + 1);
	Vector3 normal(heightL - heightR, 2.f, heightD - heightU);
	normal.Normalize();

	return normal;
}

float Terrain::getHeightInTriangle(const Vector3& p0, const Vector3& p1, const Vector3& p2, float x, float z) {
	// Calculate barycentric coordinates using Cramer's rule
	float det = ((p1.x - p0.x)*(p2.z - p0.z) - (p2.x - p0.x)*(p1.z - p0.z));
	float u = ((x - p0.x)*(p2.z - p0.z) - (p2.x - p0.x)*(z - p0.z)) / det;
	float v = ((p1.x - p0.x)*(z - p0.z) - (x - p0.x)*(p1.z - p0.z)) / det;

	// Calculate the y value in the triangle using the u,v and w values we just calculated
	float y = (1 - u - v)*p0.y + u*p1.y + v*p2.y;

	return y;
}

float Terrain::getHeightAtWorldCoords(float x, float z) {

	// Apply model translation to coordinates to convert from world to model space
	x -= getModel().getTransform().getTranslation().x;
	z -= getModel().getTransform().getTranslation().z;

	// Convert world coords to terrain grid
	unsigned int gridX = (unsigned int)std::floor(x / m_mapSize * m_mapResolution);
	unsigned int gridZ = (unsigned int)std::floor(-z / m_mapSize * m_mapResolution);

	// Return 0 if outside terrain
	if (gridX < 0 || gridZ < 0 || gridX > (m_mapResolution - 1) || gridZ > (m_mapResolution - 1))
		return 0.f;

	// Calculate local coordinates (0-1 within the grid square)
	float localX = x / m_triangleSide - gridX;
	float localZ = -z / m_triangleSide - gridZ;

	Vector3* positions = m_model.getBuildData().positions;
	// Calculate which of the two triangles in this grid slot the coordinates are in
	if (localX > 1 - localZ) {
		// Bottom right triangles

		// Get the height at (x,z) on the triangle which 3 points are given
		float y = getHeightInTriangle(	positions[gridZ * (m_mapResolution + 1) + gridX + 1],
										positions[(gridZ + 1) * (m_mapResolution + 1) + gridX],
										positions[(gridZ + 1) * (m_mapResolution + 1) + gridX + 1],
			x, z);
		return y;

	} else {
		// Top left triangle

		// Get the height at (x,z) on the triangle which 3 points are given
		float y = getHeightInTriangle(	positions[gridZ * (m_mapResolution + 1) + gridX],
										positions[gridZ * (m_mapResolution + 1) + gridX + 1],
										positions[(gridZ + 1) * (m_mapResolution + 1) + gridX],
			x, z);
		return y;

	}

	return 0.f;

}
