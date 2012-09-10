#include "CTexture.h"

CTexture::CTexture()
{
	texture = 0;
}

bool CTexture::Initialize(ID3D11Device* device, std::string filename)
{
	HRESULT result;

	// Load the texture
	result = D3DX11CreateShaderResourceViewFromFile(device, filename.c_str(), NULL, NULL, &texture, NULL);
	if (FAILED(result))
	{
		std::string texError = "CTexture::Initialize() : could not load " + filename;
		CLog::Write(texError);
	}

	return true;
}

void CTexture::Shutdown()
{
	if (texture)
	{
		texture->Release();
		texture = 0;
	}
	return;
}

ID3D11ShaderResourceView* CTexture::GetTexture()
{
	return texture;
}