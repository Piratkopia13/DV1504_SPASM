#pragma once

#include "../../RenderableTexture.h"

class PostProcessStage {
public:
	PostProcessStage(UINT width, UINT height);
	virtual ~PostProcessStage();

	virtual void run(RenderableTexture& inputTexture) = 0;

protected:
	RenderableTexture OutputTexture;

};