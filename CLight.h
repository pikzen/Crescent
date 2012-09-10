#pragma once
#include <D3DX10math.h>


class CLight
{
public:
	CLight();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetSpecularColor(float, float, float, float);
	void SetSpecularPower(float);

	D3DXVECTOR4 GetAmbientColor();
	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR3 GetDirection();
	D3DXVECTOR4 GetSpecularColor();
	float GetSpecularPower();

private:
	D3DXVECTOR4 ambientColor;
	D3DXVECTOR4 diffuseColor;
	D3DXVECTOR3 direction;
	D3DXVECTOR4 specularColor;
	float specularPower;
};