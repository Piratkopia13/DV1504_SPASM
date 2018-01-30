#include "PostProcessStage.h"

PostProcessStage::PostProcessStage(UINT width, UINT height, Model* fullscreenQuad)
	: OutputTexture(1, width, height, true)
	, FullscreenQuad(fullscreenQuad)
{
	
}

PostProcessStage::~PostProcessStage() {

}

RenderableTexture& PostProcessStage::getOutput() {
	return OutputTexture;
}