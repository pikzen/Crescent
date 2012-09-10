#include "CLight.h"

CLight::CLight()
{
}

void CLight::SetAmbientColor(float r, float g, float b, float a)
{
	ambientColor = D3DXVECTOR4(r, g, b, a);
	return;
}

void CLight::SetDiffuseColor(float r, float g, float b, float a)
{
	diffuseColor = D3DXVECTOR4(r, g, b, a);
	return;
}

void CLight::SetDirection(float x, float y, float z)
{
	direction = D3DXVECTOR3(x, y, z);
	return;
}

void CLight::SetSpecularColor(float r, float g, float b, float a)
{
	specularColor = D3DXVECTOR4(r, g, b, a);
	return;
}

void CLight::SetSpecularPower(float power)
{
	specularPower = power;
	return;
}

D3DXVECTOR4 CLight::GetAmbientColor()
{
	return ambientColor;
}

D3DXVECTOR4 CLight::GetDiffuseColor()
{
	return diffuseColor;
}

D3DXVECTOR3 CLight::GetDirection()
{
	return direction;
}

D3DXVECTOR4 CLight::GetSpecularColor()
{
	return specularColor;
}

float CLight::GetSpecularPower()
{
	return specularPower;
}