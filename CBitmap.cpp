#include "CBitmap.h"
#include "CLog.h"

// Fuck yo couch

CBitmap::CBitmap()
{
	vertexBuffer = 0;
	indexBuffer = 0;
	texture = 0;
}
CBitmap::~CBitmap()
{
}

bool CBitmap::Initialize(ID3D11Device* device, int width, int height, std::string filename, int bmpWidth, int bmpHeight)
{
	bool result;
	screenWidth = width;
	screenHeight = height;

	bitmapHeight = bmpHeight;
	bitmapWidth = bmpWidth;

	previousPosX = -1;
	previousPosY = -1;

	result = InitializeBuffers(device);
	if (!result)
	{
		CLog::Write("CBitmap::Initialize : Could not initialize the buffers");
		return false;
	}

	result = LoadTexture(device, filename);
	if (!result)
	{
		CLog::Write("CBitmap::Initialize : could not load the texture " + filename);
		return false;
	}

	return true;
}



void CBitmap::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();

	return;
}

bool CBitmap::Render(ID3D11DeviceContext* deviceContext, int posX, int posY)
{
	bool result;

	// Rebuild the dynamic vertex buffer for rendering to a different location
	// Only if posX != previousPosX
	if (posX != previousPosX ||posY != previousPosY)
	{
		result = UpdateBuffers(deviceContext, posX, posY);
		if (!result)
		{
			CLog::Write("CBitmap::Render : could not update the dynamic vertex buffers");
			return false;
		}
	}

	RenderBuffers(deviceContext);

	return true;
}

int CBitmap::GetIndexCount()
{
	return indexCount;
}

ID3D11ShaderResourceView* CBitmap::GetTexture()
{
	return texture->GetTexture();
}

bool CBitmap::InitializeBuffers(ID3D11Device* device)
{
	Vertex* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// A rectangle is made of 6 vertices (two triangles)
	vertexCount = 6;
	indexCount = vertexCount;

	vertices = new Vertex[vertexCount];
	if (!vertices)
	{
		CLog::Write("CBitmap::InitializeBuffers : could not create the vertex array");
		return false;
	}

	indices = new unsigned long[indexCount];
	if (!indices)
	{
		CLog::Write("CBitmap::InitializeBuffers : could not create the indices array");
		return false;
	}

	for (int i = 0; i < indexCount; i++) 
	{ 
		indices[i] = i;
	}

	// Vertex buffer creation
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex)	* vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
	{
		CLog::Write("CBitmap::InitializeBuffers : could not create the vertex buffer");
		return false;
	}

	// Index buffer creation
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if (FAILED(result))
	{
		CLog::Write("CBitmap::InitializeBuffers : could not create the index buffer");
		return false;
	}

	delete [] vertices;
	delete [] indices;

	return true;
}

void CBitmap::ShutdownBuffers()
{
	if (indexBuffer)
		indexBuffer->Release();

	if (vertexBuffer)
		vertexBuffer->Release();
}

bool CBitmap::UpdateBuffers(ID3D11DeviceContext* context, int posX, int posY)
{
	float left, right, top, bottom;
	Vertex* vertices;
	D3D11_MAPPED_SUBRESOURCE resource;
	Vertex* verticesPointer;
	HRESULT result;

	previousPosX = posX;
	previousPosY = posY;

	// (0; 0) is the center of the screen.
	left = (float)((screenWidth / 2) * -1) + (float)posX;
	right = left + (float)bitmapWidth; 
	top = (float)(screenHeight / 2) - (float)posY;
	bottom = top - (float)bitmapHeight;

	vertices = new Vertex[vertexCount];
	if (!vertices)
	{
		CLog::Write("CBitmap::UpdateBuffers : could not create the vertex array;");
		return false;
	}

	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

	// Second triangle.
	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);  // Top right.
	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

	result = context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(result))
	{
		CLog::Write("CBitmap::UpdateBuffers : could not map the vertex buffer");
		return false;
	}

	verticesPointer = (Vertex*)resource.pData;

	memcpy(verticesPointer, (void*)vertices, (sizeof(Vertex)*vertexCount));
	context->Unmap(vertexBuffer, 0);

	delete [] vertices;

	return true;
}

void CBitmap::RenderBuffers(ID3D11DeviceContext* context)
{
	unsigned int stride;
	unsigned int offset;

	stride = (sizeof(Vertex));
	offset = 0;

	// Set both buffers to active in the input assembler
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool CBitmap::LoadTexture(ID3D11Device* device, std::string filename)
{
	bool result;

	texture = new CTexture;
	if (!texture)
	{
		CLog::Write("CBitmap::LoadTexture : could not create the texture object");
		return false;
	}

	result = texture->Initialize(device, filename);
	if (!result)
	{
		CLog::Write("CBitmap::LoadTexture : could not initialize the texture object (" + filename + ")");
		return false;
	}

	return true;
}

void CBitmap::ReleaseTexture()
{
	if (texture)
	{
		texture->Shutdown();
		delete texture;
	}

	return;
}