#include "Model.h"

#include "../shader/basic/SimpleColorShader.h"
#include "../shader/ShaderSet.h"
#include "Material.h"

using namespace DirectX::SimpleMath;

Model::Model(Data& buildData)
	: m_data(buildData)
	, m_vertexBuffer(nullptr)
	, m_indexBuffer(nullptr)
	, m_instanceBuffer(nullptr)
	, m_aabb(Vector3::Zero, Vector3(.2f, .2f, .2f))
{
	m_material = new Material();
	m_transform = new Transform();
	m_transformChanged = false;
}
Model::Model() 
	: m_aabb(Vector3::Zero, Vector3(.2f, .2f, .2f))
	, m_vertexBuffer(nullptr)
	, m_indexBuffer(nullptr)
{
	m_material = new Material();
	m_transform = new Transform();
	m_transformChanged = false;
}
Model::~Model() {
	Memory::safeRelease(m_vertexBuffer);
	Memory::safeRelease(m_indexBuffer);
	Memory::safeRelease(m_instanceBuffer);

	Memory::safeDeleteArr(m_data.indices);
	Memory::safeDeleteArr(m_data.positions);
	Memory::safeDeleteArr(m_data.normals);
	Memory::safeDeleteArr(m_data.bitangents);
	Memory::safeDeleteArr(m_data.colors);
	Memory::safeDeleteArr(m_data.tangents);
	Memory::safeDeleteArr(m_data.texCoords);

	Memory::safeDelete(m_material);
	if (!m_transformChanged) {
		Memory::safeDelete(m_transform);
	}
}

void Model::setBuildData(Data& buildData) {
	m_data = buildData;
}
const Model::Data& Model::getBuildData() const {
	return m_data;
}

void Model::buildBufferForShader(ShaderSet* shader) {

	shader->createBufferFromModelData(&m_vertexBuffer, &m_indexBuffer, &m_instanceBuffer, &m_data);
	m_shader = shader;
	calculateAABB();

}

void Model::draw(bool bindShader) {
	if (m_transform == nullptr) {
		Logger::Error("Model has not been assigned with a transform.");
		return;
	}


	if (!m_shader) {
		Logger::Error("Buffer was not built for this model, can not draw!");
		return;
	}

	m_shader->draw(*this, bindShader);
}

UINT Model::getNumVertices() const {
	return m_data.numVertices;
}
UINT Model::getNumIndices() const {
	return m_data.numIndices;
}
UINT Model::getNumInstances() const {
	return m_data.numInstances;
}
ID3D11Buffer* const* Model::getVertexBuffer() const {
	return const_cast<ID3D11Buffer**>(&m_vertexBuffer);
}
ID3D11Buffer* Model::getIndexBuffer() const {
	return const_cast<ID3D11Buffer*>(m_indexBuffer);
}

ID3D11Buffer* Model::getInstanceBuffer() const {
	return const_cast<ID3D11Buffer*>(m_instanceBuffer);
}

void Model::setTransform(Transform* newTransform) {
	if (!m_transformChanged) {
		Memory::safeDelete(m_transform);
		m_transformChanged = true;
	}
	m_transform = newTransform;
}

Transform& Model::getTransform() {
	return *m_transform;
}

ShaderSet* Model::getShader() const {
	return m_shader;
}

Material* Model::getMaterial() {
	return m_material;
}

const AABB& Model::getAABB() const {
	return m_aabb;
}
void Model::updateAABB() {
	m_aabb.updateTransform(m_transform->getMatrix());
}

void Model::calculateAABB() {

	Vector3 minCorner(FLT_MAX, FLT_MAX, FLT_MAX );
	Vector3 maxCorner(-FLT_MIN, -FLT_MIN, -FLT_MIN);

	for (UINT i = 0; i < m_data.numVertices; i++) {
		Vector3& p = m_data.positions[i];

		if (p.x < minCorner.x) minCorner.x = p.x;
		if (p.y < minCorner.y) minCorner.y = p.y;
		if (p.z < minCorner.z) minCorner.z = p.z;

		if (p.x > maxCorner.x) maxCorner.x = p.x;
		if (p.y > maxCorner.y) maxCorner.y = p.y;
		if (p.z > maxCorner.z) maxCorner.z = p.z;

	}

	m_aabb.setMinPos(minCorner);
	m_aabb.setMaxPos(maxCorner);

}