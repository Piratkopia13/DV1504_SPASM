#pragma once

#include "../../RenderableTexture.h"
#include "../../geometry/Model.h"
//#include "../../shader/ShaderSet.h"

class PostProcessStage {
public:
	PostProcessStage(UINT width, UINT height, Model* fullscreenQuad);
	virtual ~PostProcessStage();

	virtual void run(RenderableTexture& inputTexture) = 0;
	void resize(UINT width, UINT height);
	RenderableTexture& getOutput();

protected:
	RenderableTexture OutputTexture;
	Model* FullscreenQuad;

	struct Vertex {
		DirectX::SimpleMath::Vector3 position;
	};

};