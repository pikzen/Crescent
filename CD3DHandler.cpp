#include "CD3DHandler.h"

CD3DHandler::CD3DHandler()
{
	swapChain = 0;
	device = 0;
	deviceContext = 0;
	renderTargetView = 0;
	depthStencilView = 0;
	depthStencilBuffer = 0;
	depthStencilState = 0;
	depthDisabledStencilState = 0;
	rasterState = 0;
}

bool CD3DHandler::Initialize(int width, int height, bool vsync, HWND hwnd, bool fullscreen, float screendepth, float screennear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* output;
	unsigned int numModes, numerator = 0, denominator = 0;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBuffer;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fov, screenAspect;
	

	vSync = vsync;

	// AdapterFactoryFactorySingletonFactoryOfAdapter
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		CLog::Write("CD3DHandler::Initialize() : could not create a factory");
		return false;
	}

	// Use the factory to create an adapter for the video card
	result = factory->EnumAdapters(Settings::AdapterNum, &adapter);
	if (FAILED(result))
	{
		CLog::Write("CD3DHandler::Initialize() : could not enumerate the adapters");
		return false;
	}

	// Enumerate the primary adapter output
	result = adapter->EnumOutputs(Settings::AdapterNum, &output);
	if (FAILED(result))
	{
		CLog::Write("CD3DHandler::Initialize() : could not enumerate the primary adapter output");
		return false;
	}

	// Find out which modes fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter
	result = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		CLog::Write("CD3DHandler::Initialize() : could not get a display mode list");
		return false;
	}

	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		CLog::Write("CD3DHandler::Initialize() : the display mode list description has not been initialized");
		return false;
	}

	result = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		CLog::Write("CD3DHandler::Initialize() : could not fill the display mode list descriptions");
		return false;
	}

	for (unsigned int i = 0; i < numModes; i++)
	{
		if(displayModeList[i].Width == width && displayModeList[i].Height == height)
		{
			numerator = displayModeList[i].RefreshRate.Numerator;
			denominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	// Get the adapter description
	result = adapter->GetDesc(&adapterDesc);
	if( FAILED(result))
	{
		CLog::Write("CD3DHandler::Initialize() : could not get the adapter description");
		return false;
	}
	videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
	
	unsigned int stringLength, error;
	error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		CLog::Write("CD3DHandler::Initialize() : could not get the video card description");
		return false;
	}
	delete [] displayModeList;
	displayModeList = 0;

	output->Release();
	output = 0;

	adapter->Release();
	adapter = 0;

	factory->Release();
	factory = 0;

	// Initializing the swap chain description
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// use only one back buffer
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer
	if (vSync)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set how the back buffer will be used
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the window on which it will be rendered
	swapChainDesc.OutputWindow = hwnd;

	// Set multisampling
	swapChainDesc.SampleDesc.Count = Settings::MSAACount;
	swapChainDesc.SampleDesc.Quality = Settings::MSAAQuality;

	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after displaying
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// No particular flags
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX11
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swapchain, device, device context
	result = D3D11CreateDeviceAndSwapChain(NULL, 
										   D3D_DRIVER_TYPE_HARDWARE, 
										   NULL,
										   0,
										   &featureLevel,
										   1,
										   D3D11_SDK_VERSION,
										   &swapChainDesc,
										   &swapChain,
										   &device,
										   NULL,
										   &deviceContext);
	if (FAILED(result))
	{
		CLog::Write("CD3DHandler::Initialize() : Failed to create the swapchain/device/device context");
		return false;
	}

	// Get a pointer to the back buffer
	result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(result))
	{
		CLog::Write("CD3DHandler::Initialize() : could not get a pointer to the back buffer");
		return false;
	}

	// create a render target view using the backbuffer
	result = device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);
	if (FAILED(result))
	{
		CLog::Write("CD3DHandler::Initialize() : could not create a render target view");
		return false;
	}

	// Release the back buffer pointer, it is no longer needed
	backBuffer->Release();
	backBuffer = 0;

	// Setting up the depth buffer
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = Settings::MSAACount;
	depthBufferDesc.SampleDesc.Quality = Settings::MSAAQuality;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.MiscFlags = 0;

	// Create the depth buffer
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
	if (FAILED(result))
	{
		CLog::Write("CD3DHandler::Initialize() : could not create the texture for the depth buffer");
		return false;
	}

	// Settings up the stencil desc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// pixel is front facing:
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// pixel is back facing:
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state
	result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	if (FAILED(result))
	{
		CLog::Write("CD3DHandler::Initialize() : could not create the depth stencil state");
		return false;
	}

	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	// Setup the depth stencil description view
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	if (FAILED(result))
	{
		CLog::Write("CD3DHandler::Initialize() : could not create the depth stencil buffer");
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output pipeline
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	rasterDesc.AntialiasedLineEnable = Settings::LineAA;
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = Settings::EnableMSAA;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.f;

	result = device->CreateRasterizerState(&rasterDesc, &rasterState);
	if (FAILED(result))
	{
		CLog::Write("CD3DHandler::Initialize() : could not create the rasterizer state");
	}

	// Bind the rasterizer
	deviceContext->RSSetState(rasterState);

	// Setting up the viewport
	// May probably switch top (0;0) being the bottom left corner.
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	deviceContext->RSSetViewports(1, &viewport);

	// Setup the projection matrix
	fov = Settings::FieldOfView;
	screenAspect = (float)width / (float)height;

	// Create the projection matrix for 3D rendering
	D3DXMatrixPerspectiveFovLH(&projectionMatrix, fov, screenAspect, screennear, screendepth);
	// The world matrix is set to the identity matrix
	D3DXMatrixIdentity(&worldMatrix);
	// Create an orthographic projection matrix for 2D rendering
	D3DXMatrixOrthoLH(&orthoMatrix, (float)width, (float)height, screennear, screendepth);

	// Create the depth disabled stencil state;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = device->CreateDepthStencilState(&depthDisabledStencilDesc, &depthDisabledStencilState);
	if (FAILED(result))
	{
		CLog::Write("CD3DHandler::Initialize : Could not create the depth disabled stencil state");
		return false;
	}

	return true;
}

void CD3DHandler::Shutdown()
{
	// Set the swap chain to windowed mode before releasing it or bad things happen
	// like non-handled exceptions
	if (swapChain)
	{
		swapChain->SetFullscreenState(false, NULL);
	}

	if (rasterState)
	{
		rasterState->Release();
		rasterState = 0;
	}

	if (depthStencilView)
	{
		depthStencilView->Release();
		depthStencilView = 0;
	}

	if (depthStencilState)
	{
		depthStencilState->Release();
		depthStencilState = 0;
	}

	if (depthDisabledStencilState)
	{
		depthDisabledStencilState->Release();
		depthDisabledStencilState = 0;
	}

	if (depthStencilBuffer)
	{
		depthStencilBuffer->Release();
		depthStencilBuffer = 0;
	}

	if (renderTargetView)
	{
		renderTargetView->Release();
		renderTargetView = 0;
	}

	if (deviceContext)
	{
		deviceContext->Release();
		deviceContext = 0;
	}

	if (device)
	{
		device->Release();
		device = 0;
	}

	if (swapChain)
	{
		swapChain->Release();
		swapChain = 0;
	}

	return;
}

void CD3DHandler::ClearBackground(int r, int g, int b, int a)
{
	float color[4];

	float red = (float)r;
	float green = (float)g;
	float blue = (float)b;
	float alpha = (float)a;

	color[0] = (red / 255);
	color[1] = (green / 255);
	color[2] = (blue / 255);
	color[3] = (alpha / 255);

	// Clear the backbuffer
	deviceContext->ClearRenderTargetView(renderTargetView, color);

	// Clear the depth buffer
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void CD3DHandler::ClearBackground(float r, float g, float b, float a)
{
	float color[4];

	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	// Clear the backbuffer
	deviceContext->ClearRenderTargetView(renderTargetView, color);

	// Clear the depth buffer
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void CD3DHandler::Display()
{
	if (vSync)
	{
		// Lock to the screen refresh rate
		swapChain->Present(1, 0);
	}
	else
	{
		// Present whenever possible
		swapChain->Present(0, 0);
	}

	return;
}

// Shaders need to get pointers to device and matrices
// Just a couple of helper functions
ID3D11Device* CD3DHandler::GetDevice()
{
	return device;
}

ID3D11DeviceContext* CD3DHandler::GetDeviceContext()
{
	return deviceContext;
}

void CD3DHandler::GetProjectionMatrix(D3DXMATRIX& pMatrix)
{
	pMatrix = projectionMatrix;
}

void CD3DHandler::GetWorldMatrix(D3DXMATRIX& wMatrix)
{
	wMatrix = worldMatrix;
}

void CD3DHandler::GetOrthoMatrix(D3DXMATRIX& oMatrix)
{
	oMatrix = orthoMatrix;
}

void CD3DHandler::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, videoCardDescription);
	memory = videoCardMemory;
	return;
}

void CD3DHandler::TurnZBufferOff()
{
	deviceContext->OMSetDepthStencilState(depthDisabledStencilState, 1);
}

void CD3DHandler::TurnZBufferOn()
{
	deviceContext->OMSetDepthStencilState(depthStencilState, 1);
}