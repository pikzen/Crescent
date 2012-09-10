#include "CShader.h"

CShader::CShader()
{
	vertexShader = 0;
	pixelShader = 0;
	layout = 0;
	matrixBuffer = 0;
}

bool CShader::Initialize(ID3D11Device* device, HWND hwnd, char* vertex, char* pixel)
{
	bool result;

	result = InitializeShader(device, hwnd, vertex, pixel);
	if (!result)
	{
		CLog::Write("CColorShader::Initialize() : could not initialize the shaders");
		return false;
	}
}

void CShader::Shutdown()
{
	ShutdownShader();

	return;
}

bool CShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX wMatrix, D3DXMATRIX vMatrix, D3DXMATRIX pMatrix)
{
	bool result;
	result = SetShaderParameters(deviceContext, wMatrix, vMatrix, pMatrix);
	if (!result)
	{
		CLog::Write("CColorShader::Render() : could not set the shader parameters");
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool CompileShaders(char* vsFile, char* vsEntryPoint, char* psFile, char* psEntryPoint)
{
	result = D3DX11CompileFromFile(vsFile,
								   NULL,
								   NULL,
								   vsEntryPoint,
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
			OutputShaderErrorMessage(errorMessage, NULL, vsFile);
		else
			CLog::Write("CShader::InitializeShader() : Missing vertex shader file");
		return false;
	}

	// Compile the pixel shader code
	result = D3DX11CompileFromFile(psFile,
								   NULL,
								   NULL,
								   psEntryPoint,
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
			OutputShaderErrorMessage(errorMessage, NULL, psFile);
		else
			CLog::Write("CShader::InitializeShader() : Missing pixel shader file");
		return false;
	}
}

bool CShader::InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader code
	

	// Create the vertex shader from the buffer
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(result))
	{
		CLog::Write("CColorShader::InitializeShader() : could not create the vertex shader");
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(result))
	{
		CLog::Write("CColorShader::InitializeShader() : could not create the pixel shader");
		return false;
	}

	// Setup the layout of the data that goes into the shader
	// It matches the VertexType struct in CMesh and the shader
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;	

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	
	// create the vertex input layout
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layout);

	if (FAILED(result))
	{
		CLog::Write("CColorShader::InitializeShader() : could not create the input layout");
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer to access the vertex shader
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
	if (FAILED(result))
	{
		CLog::Write("CColorShader::InitializeShader() : could not create the constant buffer to access the vertex shader");
		return false;
	}

	return true;
}

void CShader::ShutdownShader()
{
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

	return;
}

void CShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, char* shaderFilename)
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

bool CShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX wMatrix, D3DXMATRIX vMatrix, D3DXMATRIX pMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* data;
	unsigned int bufferNumber;

	// Transpose the matrices beofre sending them in the shader
	D3DXMatrixTranspose(&wMatrix, &wMatrix);
	D3DXMatrixTranspose(&vMatrix, &vMatrix);
	D3DXMatrixTranspose(&pMatrix, &pMatrix);

	// Lock the matrix buffer so it can be written to
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		CLog::Write("CColorShader::SetShaderParameters() : could not lock the matrix buffer");
		return false;
	}

	data = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer
	data->world = wMatrix;
	data->projection = pMatrix;
	data->view = vMatrix;

	// Unlock the buffer
	deviceContext->Unmap(matrixBuffer, 0);

	// Set the position of the buffer in the vertex shader
	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

	return true;
}

void CShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(layout);

	// Set which vertex and pixel shaders that will be used
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	// Render the triangle
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}