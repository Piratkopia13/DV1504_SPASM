#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include <limits>
#include "../../utils/Utils.h"
#include "Transform.h"
#include "spatial/AABB.h"

// Forward declarations
class ShaderSet;
class Material;

class Model {
public: 

	struct Data {
		Data() : numIndices(0), numInstances(0), indices(nullptr), numVertices(0), normals(nullptr), positions(nullptr), colors(nullptr), texCoords(nullptr), tangents(nullptr), bitangents(nullptr) {};
		void deepCopy(const Data& other) {
			this->numIndices = other.numIndices;
			this->numVertices = other.numVertices;
			this->numInstances = other.numInstances;
			if (other.indices) {
				this->indices = new ULONG[other.numIndices];
				for (UINT i = 0; i < other.numIndices; i++)
					this->indices[i] = other.indices[i];
			}
			UINT numVerts = (other.numIndices > 0) ? other.numIndices : other.numVertices;
			if (other.positions) {
				this->positions = new DirectX::SimpleMath::Vector3[numVerts];
				for (UINT i = 0; i < numVerts; i++)
					this->positions[i] = other.positions[i];
			}
			if (other.normals) {
				this->normals = new DirectX::SimpleMath::Vector3[numVerts];
				for (UINT i = 0; i < numVerts; i++)
					this->normals[i] = other.normals[i];
			}
			if (other.colors) {
				this->colors = new DirectX::SimpleMath::Vector4[numVerts];
				for (UINT i = 0; i < numVerts; i++)
					this->colors[i] = other.colors[i];
			}
			if (other.texCoords) {
				this->texCoords = new DirectX::SimpleMath::Vector2[numVerts];
				for (UINT i = 0; i < numVerts; i++)
					this->texCoords[i] = other.texCoords[i];
			}
			if (other.tangents) {
				this->tangents = new DirectX::SimpleMath::Vector3[numVerts];
				for (UINT i = 0; i < numVerts; i++)
					this->tangents[i] = other.tangents[i];
			}
			if (other.bitangents) {
				this->bitangents = new DirectX::SimpleMath::Vector3[numVerts];
				for (UINT i = 0; i < numVerts; i++)
					this->bitangents[i] = other.bitangents[i];
			}
		}
		UINT numIndices;
		ULONG* indices;
		UINT numVertices;
		UINT numInstances;
		DirectX::SimpleMath::Vector3* positions;
		DirectX::SimpleMath::Vector3* normals;
		DirectX::SimpleMath::Vector4* colors;
		DirectX::SimpleMath::Vector2* texCoords;
		DirectX::SimpleMath::Vector3* tangents;
		DirectX::SimpleMath::Vector3* bitangents;
	};

	Model(Data& buildData);
	Model();
	~Model();

	void setBuildData(Data& buildData);
	const Data& getBuildData() const;
	void buildBufferForShader(ShaderSet* shader);

	// Does the same as shader.draw(model)
	// You do not have to specify the shader if you use this method with true parameter
	// The shader is already set when the buffer was built
	void draw(bool bindShader = true);

	UINT getNumVertices() const;
	UINT getNumIndices() const;
	UINT getNumInstances() const;
	ID3D11Buffer* const* getVertexBuffer() const;
	ID3D11Buffer* getIndexBuffer() const;
	ID3D11Buffer* getInstanceBuffer() const;
	void setTransform(Transform* newTransform);
	Transform& getTransform();
	ShaderSet* getShader() const;
	Material* getMaterial();
	const AABB& getAABB() const;
	void updateAABB();

private:
	void calculateAABB();

private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	ID3D11Buffer* m_instanceBuffer;
	ShaderSet* m_shader;

	bool m_transformChanged;
	Transform* m_transform;
	Material* m_material;

	Data m_data;

	AABB m_aabb;

};
