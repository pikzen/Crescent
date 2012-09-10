#ifndef CTEXTURE_H
#define CTEXTURE_H

#include <D3D11.h>
#include <D3DX11tex.h>
#include <string>
#include "CLog.h"

class CTexture
{
public: 
	CTexture();

	bool Initialize(ID3D11Device*, std::string);
	void Shutdown();
	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView* texture;
};

#endif