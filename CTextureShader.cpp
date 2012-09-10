#include "CTextureShader.h"

CTextureShader::CTextureShader()
{
	vertexShader = 0;
	pixelShader = 0;
	layout = 0;
	matrixBuffer = 0;
	sampleState = 0;
}

bool CTextureShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = InitializeShader(device, hwnd, "Shaders/texture.vs", "Shaders/texture.ps");
	if (!result)
	{
		return false;
	}

	return true;
}

void CTextureShader::Shutdown()
{
	ShutdownShader();

	return;
}

bool CTextureShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX wMatrix, D3DXMATRIX vMatrix, D3DXMATRIX pMatrix, ID3D11ShaderResourceView* texture)
{
	bool result;

	result = SetShaderParameters(deviceContext, wMatrix, vMatrix, pMatrix, texture);
	if (!result)
	{
		CLog::Write("CTextureShader::Render() : could not set the shader parameters");
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool CTextureShader::InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// compile the vertex shader
	result = D3DX11CompileFromFile(vsFilename, 
								  NULL, 
								  NULL, 
								  "TextureVertexShader", 
								  "vs_5_0",
								  D3D10_SHADER_ENABLE_STRICTNESS,
								  0,
								  NULL, 
								  &vertexShaderBuffer,
								  &errorMessage,
								  NULL);

	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		else
			CLog::Write("CTextureShader::InitializeShader() : Missing vertex shader file");
		return false;
	}

	result = D3DX11CompileFromFile(psFilename,
								   NULL,
								   NULL,
								   "TexturePixelShader",
								   "ps_5_0",
								   D3D10_SHADER_ENABLE_STRICTNESS,
								   0,
								   NULL,
								   &pixelShaderBuffer,
								   &errorMessage,
								   NULL);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		else
			CLog::Write("CTextureShader::InitializeShader() : Missing pixel shader file");
		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(result))
	{
		CLog::Write("CTextureShader::InitializeShader() : could not create the vertex shader");
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(result))
	{
		CLog::Write("CTextureShader::InitializeShader() : could not create the vertex shader");
		return false;
	}

	// Create the vertex input layout
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout)/sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layout);
	if (FAILED(result))
	{
		CLog::Write("CTextureShader::InitializeShader() : could not create the input layout");
		return false;
	}

	// release the vertex and pixel shader buffers
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
	if (FAILED(result))
	{
		CLog::Write("CTextureShader::InitializeShader() : could not create the matrix buffer");
		return false;
	}

	// Create a texture sampler state description
	samplerDesc.Filter = Settings::TextureFilter;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state
	result = device->CreateSamplerState(&samplerDesc, &sampleState);
	if (FAILED(result))
	{
		CLog::Write("CTextureShader::InitializeShader() : could not create the sampler state");
		return false;
	}

	return true;
}

void CTextureShader::ShutdownShader()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	if (pixelShader)
	{
		pixelShader->Release();
		pixelShader = 0;
	}

	if (vertexShader)
	{
		vertexShader->Release();
		vertexShader = 0;
	}
}

void CTextureShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, char* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize;

	compileErrors = (char*)(errorMessage->GetBufferPointer());
	bufferSize = errorMessage->GetBufferSize();

	CLog::Write(compileErrors);

	errorMessage->Release();
	errorMessage = 0;

	return;
}

bool CTextureShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX wMatrix, D3DXMATRIX vMatrix, D3DXMATRIX pMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* data;
	unsigned int bufferNumber;

	D3DXMatrixTranspose(&wMatrix, &wMatrix);
	D3DXMatrixTranspose(&vMatrix, &vMatrix);
	D3DXMatrixTranspose(&pMatrix, &pMatrix);

	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		CLog::Write("CTextureShader::SetShaderParameters() : could not lock the matrix buffer");
		return false;
	}

	data = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices in the buffer
	data->world = wMatrix;
	data->view = vMatrix;
	data->projection = pMatrix;

	// Unlock the constant buffer
	deviceContext->Unmap(matrixBuffer, 0);
	bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void CTextureShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout
	deviceContext->IASetInputLayout(layout);

	// Set which vertex and pixel shader which will be used
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &sampleState);
	
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}