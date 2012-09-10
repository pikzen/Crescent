#include "CLightShader.h"

CLightShader::CLightShader()
{
	vertexShader = 0;
	pixelShader = 0;
	layout = 0;
	sampleState = 0;
	matrixBuffer = 0;
	cameraBuffer = 0;
	lightBuffer = 0;
}

bool CLightShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	result = InitializeShader(device, hwnd, "Shaders/specularlight.vs", "Shaders/specularlight.ps");
	if (!result)
	{
		CLog::Write("CLightShader::Initialize() : could not initialize the shader");
		return false;
	}

	return true;
}

void CLightShader::Shutdown()
{
	ShutdownShader();

	return;
}

bool CLightShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX wMatrix, D3DXMATRIX vMatrix, D3DXMATRIX pMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower)
{
	bool result;

	result = SetShaderParameters(deviceContext, wMatrix, vMatrix, pMatrix, texture, lightDirection, ambientColor, diffuseColor, cameraPosition, specularColor, specularPower);
	if (!result)
	{
		CLog::Write("CLightShader::Render() : could not set the shader parameters");
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool CLightShader::InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	// Initialize the pointers
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader
	result = D3DX11CompileFromFile(vsFilename,
								   NULL,
								   NULL,
								   "LightVertexShader",
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
			CLog::Write("CLightShader::InitializeShader() : could not compile the vertex shader");
		return false;
	}

	// Compile the pixel shader
	result = D3DX11CompileFromFile(psFilename,
								   NULL,
								   NULL,
								   "LightPixelShader",
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
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		else
			CLog::Write("CLightShader::InitializeShader() : could not compile the pixel shader");
		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(result))
	{
		CLog::Write("CLightShader::InitializeShader() : could not create the vertex shader");
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(result))
	{
		CLog::Write("CLightShader::InitializeShader() : could not create the pixel shader");
		return false;
	}

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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Get the number of elements in the layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layout);
	if (FAILED(result))
	{
		CLog::Write("CLightShader::InitializeShader() : could not create the input layout");
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Create a texture sampler description
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

	result = device->CreateSamplerState(&samplerDesc, &sampleState);
	if (FAILED(result))
	{
		CLog::Write("CLightShader::InitializeShader() : could not create the sampler state");
		return false;
	}

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
	if (FAILED(result))
	{
		CLog::Write("CLightShader::InitializeShader() : could not create the matrix buffer");
		return false;
	}

	// Setup the camera buffer description
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);
	if (FAILED(result))
	{
		CLog::Write("CLightShader::InitializeShader() : could not create the matrix buffer");
		return false;
	}

	// Setup the description of the light dynamic buffer in the pixel shader (what the fuck does that even mean)
	// ByteWidth needs to be a multiple of 16. If not, CreateBuffer will fail
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
	if (FAILED(result))
	{
		CLog::Write("CLightShader::InitializeShader() : could not create the light buffer");
		return false;
	}
	
	return true;
}

void CLightShader::ShutdownShader()
{
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	if (cameraBuffer)
	{
		cameraBuffer->Release();
		cameraBuffer = 0;
	}

	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
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

	return;
}

void CLightShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, char* shaderFilename)
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

bool CLightShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX wMatrix, D3DXMATRIX vMatrix, D3DXMATRIX pMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* matrixData;
	LightBufferType* lightData;
	CameraBufferType* cameraData;

	D3DXMatrixTranspose(&wMatrix, &wMatrix);
	D3DXMatrixTranspose(&vMatrix, &vMatrix);
	D3DXMatrixTranspose(&pMatrix, &pMatrix);

	// Lock the taskba- constant buffer
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		CLog::Write("CLightShader::SetShaderParameters() : could not lock the matrix buffer");
		return false;
	}

	matrixData = (MatrixBufferType*)mappedResource.pData;

	// copy the matrices in the constant buffer
	matrixData->world = wMatrix;
	matrixData->view = vMatrix;
	matrixData->projection = pMatrix;

	// Unlock the taskba- constant buffer
	deviceContext->Unmap(matrixBuffer, 0);

	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

	result = deviceContext->Map(cameraBuffer, 0,D3D11_MAP_WRITE_DISCARD ,0, &mappedResource);
	if (FAILED(result))
	{
		CLog::Write("CLightShader::SetShaderParameters() : could not lock the camera buffer");
		return false;
	}

	cameraData = (CameraBufferType*)mappedResource.pData;
	cameraData->cameraPosition = cameraPosition;
	cameraData->padding = 0.0f;

	deviceContext->Unmap(cameraBuffer, 0);

	bufferNumber = 1;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &cameraBuffer);
	deviceContext->PSSetShaderResources(0, 1, &texture);

	// Lock the taskba- light constant buffer (ok i'll stop)
	result = deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		CLog::Write("CLightShader::SetShaderParameters() : could not lock the light buffer");
		return false;
	}

	lightData = (LightBufferType*)mappedResource.pData;

	lightData->ambientColor = ambientColor;
	lightData->diffuseColor = diffuseColor;
	lightData->lightDirection = lightDirection;
	lightData->specularColor = specularColor;
	lightData->specularPower = specularPower;

	// Unlock the taskba- light constant buffer (I lied)
	deviceContext->Unmap(lightBuffer, 0);

	// Just in case VSSetConstantBuffers did some stupid stuff
	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &lightBuffer);

	return true;
}

void CLightShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(layout);

	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &sampleState);

	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}