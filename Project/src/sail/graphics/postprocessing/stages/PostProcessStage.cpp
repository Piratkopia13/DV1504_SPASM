#include "PostProcessStage.h"

PostProcessStage::PostProcessStage(UINT width, UINT height, Model* fullscreenQuad)
	: OutputTexture(1, width, height, true)
	, FullscreenQuad(fullscreenQuad)
{
	
}

PostProcessStage::~PostProcessStage() {

}

void PostProcessStage::resize(UINT width, UINT height) {
	OutputTexture.resize(width, height);
}

RenderableTexture& PostProcessStage::getOutput() {
	return OutputTexture;
}