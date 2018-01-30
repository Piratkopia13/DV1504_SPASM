#include "PostProcessPass.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

PostProcessPass::PostProcessPass() {

}

PostProcessPass::~PostProcessPass() {
}

void PostProcessPass::run(RenderableTexture & inputTexture) {

	m_horiGaussianBlurStage.run(inputTexture);
	m_vertGaussianBlurStage.run(m_horiGaussianBlurStage.getOutput());
	m_flushStage.run();

}

void PostProcessPass::createFullscreenQuad() {

	Vector2 halfSizes(1.f, 1.f);

	const int numVerts = 4;
	Vector3* positions = new Vector3[numVerts]{
		Vector3(-halfSizes.x, -halfSizes.y, 0.f),
		Vector3(-halfSizes.x, halfSizes.y, 0.f),
		Vector3(halfSizes.x, -halfSizes.y, 0.f),
		Vector3(halfSizes.x, halfSizes.y, 0.f),
	};

	const int numIndices = 6;
	ULONG* indices = new ULONG[numIndices]{
		0, 1, 2, 2, 1, 3
	};

	// Tex coords not used in shader, only set to get rid of warning
	Vector2* texCoords = new Vector2[numVerts]{
		Vector2(0.f, 1.f),
		Vector2(0.f, 0.f),
		Vector2(1.f, 1.f),
		Vector2(1.f, 0.f)
	};

	Model::Data data;
	data.numVertices = numVerts;
	data.numIndices = numIndices;
	data.positions = positions;
	data.indices = indices;
	data.texCoords = texCoords;

	m_fullScreenQuad.setBuildData(data);
	m_fullScreenQuad.buildBufferForShader(&m_postProcessFlushShader);
}
