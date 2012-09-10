#ifndef CCAMERA_H
#define CCAMERA_H

#include <D3DX10math.h>

class CCamera
{
public:
	CCamera();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	void Render();
	void GetViewMatrix(D3DXMATRIX&);

private:
	float posX, posY, posZ;
	float rotX, rotY, rotZ;
	D3DXMATRIX viewMatrix;
};

#endif