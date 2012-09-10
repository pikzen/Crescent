#include "CCamera.h"

CCamera::CCamera()
{
	posX = posY = posZ = 0.0f;
	rotX = rotY = rotZ = 0.0f;
}

void CCamera::SetPosition(float x, float y, float z)
{
	posX = x;
	posY = y;
	posZ = z;
}

void CCamera::SetRotation(float x, float y, float z)
{
	rotX = x;
	rotY = y;
	rotZ = z;
}

D3DXVECTOR3 CCamera::GetPosition()
{
	return D3DXVECTOR3(posX, posY, posZ);
}
D3DXVECTOR3 CCamera::GetRotation()
{
	return D3DXVECTOR3(rotX, rotY, rotZ);
}

void CCamera::Render()
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;

	// Setup the upwards pointing vector
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world
	position.x = posX;
	position.y = posY;
	position.z = posZ;

	// Setup where the camera is looking at by default
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Convert the axis in radians
	pitch = rotX * (float)(D3DX_PI/180); // Magic number for radians converting. 
	yaw = rotY * (float)(D3DX_PI/180); 
	roll = rotZ * (float)(D3DX_PI/180);

	// Create the rotation matrix 
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	lookAt = position + lookAt;

	// Create the view matrix from those vectors
	D3DXMatrixLookAtLH(&viewMatrix, &position, &lookAt, &up);

	return;
}

void CCamera::GetViewMatrix(D3DXMATRIX& vMatrix)
{
	vMatrix = viewMatrix;
	return;
}