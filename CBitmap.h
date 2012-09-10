#pragma once

#include <D3D11.h>
#include <D3DX10math.h>
#include <string>

#include "CTexture.h"

class CBitmap {
	private:
		struct Vertex
		{
			D3DXVECTOR3 position;
			D3DXVECTOR2 texture;
		};

	public:
		CBitmap();
		~CBitmap();

		bool Initialize(ID3D11Device*, int, int, std::string, int, int);
		void Shutdown();
		bool Render(ID3D11DeviceContext*, int, int);

		int GetIndexCount();
		ID3D11ShaderResourceView* GetTexture();

	private:
		bool InitializeBuffers(ID3D11Device*);
		void ShutdownBuffers();
		bool UpdateBuffers(ID3D11DeviceContext*, int, int);
		void RenderBuffers(ID3D11DeviceContext*);

		bool LoadTexture(ID3D11Device*, std::string);
		void ReleaseTexture();

	private:
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount;
		CTexture* texture;

		int screenWidth, screenHeight, bitmapWidth, bitmapHeight, previousPosX, previousPosY;

};