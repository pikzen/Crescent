#ifndef CTEXTURESHADER_H
#define CTEXTURESHADER_H

#include <D3D11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include <fstream>
#include "Settings.h"
#include "CLog.h"

class CTextureShader
{
	private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

public:
	CTextureShader();
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);

private:
	bool InitializeShader(ID3D11Device*, HWND, char*, char*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, char*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
};


#endif