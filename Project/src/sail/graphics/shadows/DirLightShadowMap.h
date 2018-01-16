#pragma once

#include "ShadowMap.h"
#include "../RenderableTexture.h"
#include "../geometry/spatial/Quadtree.h"
#include "../../Application.h"
#include "../camera/OrthographicCamera.h"
#include "../shader/deferred/DeferredDirectionalLightShader.h"

class DirLightShadowMap : public ShadowMap {

public:
	DirLightShadowMap(int width, int height);
	~DirLightShadowMap();

	ID3D11DepthStencilView* const * getDSV();
	ID3D11ShaderResourceView** getSRV();
	D3D11_VIEWPORT* getViewPort();
	//DeferredDirectionalLightShader* getDLShader();

private:
	RenderableTexture m_depthTexture;
	//DeferredDirectionalLightShader* m_shader;

};