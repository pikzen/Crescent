#include "CMesh.h"

CMesh::CMesh()
{
	vertexBuffer = 0;
	indexBuffer = 0;
	texture = 0;
	mesh = 0;
}

bool CMesh::Initialize(ID3D11Device* device, std::string textureFilename, std::string meshFilename)
{
	bool result;
	result = LoadCSM(meshFilename);
	if (!result)
	{
		return false;
	}

	if (!InitializeBuffers(device))
	{
		return false;
	}

	if (!LoadTexture(device, textureFilename))
	{
		return false;
	}

	return true;
}

void CMesh::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
	ReleaseMesh();
	return;
}

void CMesh::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);

	return;
}

int CMesh::GetIndexCount()
{
	return indexCount;
}

bool CMesh::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	vertices = new VertexType[vertexCount];
	if (!vertices)
	{
		CLog::Write("CMesh::InitializeBuffers() : could not create the vertex array");
		return false;
	}

	indices = new unsigned long[indexCount];
	if (!indices)
	{
		CLog::Write("CMesh::InitializeBuffers() : could not create the index array");
		return false;
	}

	for (int i = 0; i < vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(mesh[i].x, mesh[i].y, mesh[i].z);
		vertices[i].texture = D3DXVECTOR2(mesh[i].tu, mesh[i].tv);
		vertices[i].normal = D3DXVECTOR3(mesh[i].nx, mesh[i].ny, mesh[i].nz);

		indices[i] = i;
	}
	// Setup the vertex buffer description
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
	{
		CLog::Write("CMesh::InitializeBuffers() : could not create the vertex buffer");
		return false;
	}

	// Setup the index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if (FAILED(result))
	{
		CLog::Write("CMesh::InitializeBuffers() : could not create the index buffer");
		return false;
	}

	delete [] vertices;
	vertices = 0;
	delete [] indices;
	indices = 0;

	return true;
}

bool CMesh::LoadTexture(ID3D11Device* device, std::string filename)
{
	bool result;

	texture = new CTexture;
	if (!texture)
	{
		CLog::Write("CMesh::LoadTexture() : Could not load the following texture :" + filename);
		return false;
	}

	result = texture->Initialize(device, filename);
	if (!result)
	{
		CLog::Write("Could not initialize the texture: " + filename);
	}

	return true;
}

void CMesh::ReleaseTexture()
{
	if (texture)
	{
		texture->Shutdown();
		delete texture;
		texture = 0;
	}

	return;
}

bool CMesh::LoadCSM(std::string filename)
{
	std::ifstream fin;
	char input;
	int i;

	fin.open(filename);

	if (fin.fail())
	{
		CLog::Write("CMesh::LoadMesh() : could not load the mesh file. The file probably does not exists");
		return false;
	}

	// Check for the first ':'
	// vcount:
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	fin >> vertexCount;
	indexCount = vertexCount;

	mesh = new MeshType[vertexCount];
	if (!mesh)
	{
		return false;
	}

	// check for the second ':'
	// data:
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}
	fin.get(input); // Go over the ':'
	fin.get(input);


	for (i = 0; i < vertexCount; i++)
	{
		// ifstream magic
		fin >> mesh[i].x >> mesh[i].y >> mesh[i].z;
		fin >> mesh[i].tu >> mesh[i].tv;
		fin >> mesh[i].nx >> mesh[i].ny >> mesh[i].nz;
	}

	fin.close();

	return true;
}

void CMesh::ReleaseMesh()
{
	if (mesh)
	{
		delete mesh;
		mesh = 0;
	}

	return;
}
void CMesh::ShutdownBuffers()
{
	if (indexBuffer)
	{
		indexBuffer->Release();
		indexBuffer = 0;
	}

	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = 0;
	}
}

void CMesh::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	// Set the index buffer to active in the input assembler
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}
ID3D11ShaderResourceView* CMesh::GetTexture()
{
	return texture->GetTexture();
}