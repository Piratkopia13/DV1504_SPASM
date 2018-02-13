#pragma once

#include <memory>
#include "../Model.h"

namespace ModelFactory {

	using namespace DirectX::SimpleMath;

	class InstancedTestModel {
	public:
		static std::unique_ptr<Model> Create(UINT instances) {

			const int numVerts = 1;
			Vector3* positions = new Vector3[numVerts]{
				Vector3(0.f, 0.f, 0.f)
			};

			Model::Data buildData;
			buildData.numVertices = numVerts;
			buildData.positions = positions;

			buildData.numInstances = instances;

			std::unique_ptr<Model> model = std::make_unique<Model>(buildData);

			return model;

		}
	};
}

