#ifndef CD3DHANDLER_H
#define CD3DHANDLER_H

// Linking
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

// Including
#include <DXGI.h>
#include <D3Dcommon.h>
#include <D3D11.h>
#include <D3DX10math.h>
#include "CLog.h"
#include "Settings.h"

class CD3DHandler
{
public:
	
	CD3DHandler();
	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void ClearBackground(int, int, int, int);
	void ClearBackground(float, float, float, float);
	void Display();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

	void GetVideoCardInfo(char*, int&);

	void TurnZBufferOn();
	void TurnZBufferOff();

private:
	bool vSync;
	int videoCardMemory;
	char videoCardDescription[128];

	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilState* depthDisabledStencilState;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RasterizerState* rasterState;
	D3DXMATRIX projectionMatrix;
	D3DXMATRIX worldMatrix;
	D3DXMATRIX orthoMatrix;

	

};

#endif
