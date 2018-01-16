#pragma once

#include "../resources/DXTexture.h"

class RenderableTexture {

public:
	RenderableTexture(UINT aaSamples = 1, UINT width = 320, UINT height = 180, bool createDepthStencilView = true, bool createOnlyDSV = false);
	~RenderableTexture();
	
	void begin();
	void end();
	void clear(const DirectX::XMVECTORF32& color);
	void resize(int width, int height);
	ID3D11ShaderResourceView** getColorSRV();
	ID3D11ShaderResourceView** getDepthSRV();
	ID3D11RenderTargetView** getRenderTargetView();
	ID3D11DepthStencilView** getDepthStencilView();
	D3D11_VIEWPORT* getViewPort();

private:
	void createTextures();

private:
	UINT m_width, m_height;
	D3D11_VIEWPORT m_viewport;
	DXTexture* m_dxColorTexture;
	DXTexture* m_dxDepthTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	UINT m_aaSamples;
	ID3D11DepthStencilView* m_depthStencilView;
	bool m_hasDepthStencilView;
	bool m_onlyDSV;

	ID3D11Texture2D* m_nonMSAAColorTexture2D;
	ID3D11ShaderResourceView* m_nonMSAAColorSRV;
	ID3D11ShaderResourceView* m_nonMSAADepthSRV;

};