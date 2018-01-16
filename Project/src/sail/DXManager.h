#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>
#include "utils/Utils.h"

class DXManager {

public:
	DXManager();
	~DXManager();

	int initDirect3D(const HWND* hwnd, UINT windowWidth, UINT windowHeight, UINT aaSamples = 1);

	ID3D11Device* getDevice() const;
	ID3D11DeviceContext* getDeviceContext() const;

	ID3D11DepthStencilView* getDepthStencilView() const;
	UINT getAASamples();
	ID3D11RenderTargetView* const* getBackBufferRTV() const;

	void resize(int width, int height);
	void renderToBackBuffer() const;
	void enableDepthBuffer() const;
	void disableDepthBuffer() const;
	void enableAlphaBlending();
	void disableAlphaBlending();
	void enableAdditiveBlending();
	void enableFrontFaceCulling() const;
	void enableBackFaceCulling() const;
	void disableFaceCulling() const;
	void clear(const DirectX::XMVECTORF32& color) const;
	void present(bool vsync = false) const;

private:
	void createDepthStencilBufferAndBind(UINT windowWidth, UINT windowHeight);

private:

	// DirectX attributes
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	IDXGISwapChain* m_swapChain;
	ID3D11RenderTargetView* m_renderTargetView;
	D3D_DRIVER_TYPE m_driverType;
	D3D_FEATURE_LEVEL m_featureLevel;
	D3D11_VIEWPORT m_viewport;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilStateEnabled;
	ID3D11DepthStencilState* m_depthStencilStateDisabled;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11ShaderResourceView* m_depthStencilSRV;
	ID3D11RasterizerState* m_rasterStateBackfaceCulling;
	ID3D11RasterizerState* m_rasterStateFrontfaceCulling;
	ID3D11RasterizerState* m_rasterStateNoCulling;
	ID3D11BlendState* m_blendStateEnabled;
	ID3D11BlendState* m_blendStateDisabled;
	ID3D11BlendState* m_blendStateAdditive;
	ID3D11Debug* m_debug;

	UINT m_aaSamples;

	bool m_CW;
	
};