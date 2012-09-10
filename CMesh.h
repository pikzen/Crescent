#ifndef CMESH_H
#define CMESH_H

#include <D3D11.h>
#include <D3DX10math.h>
#include "CLog.h"
#include "CTexture.h"
#include <fstream>
#include <string>

class CMesh
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	struct MeshType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
public:
	CMesh();

	bool Initialize(ID3D11Device*, std::string, std::string);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTexture(ID3D11Device*, std::string);
	void ReleaseTexture();
	bool LoadCSM(std::string);
	void ReleaseMesh();

private:
	ID3D11Buffer *vertexBuffer, *indexBuffer;
	int vertexCount, indexCount, faceCount;
	CTexture* texture;
	MeshType* mesh;
	std::string name;
};

#endif