#include "CRenderer.h"

CRenderer::CRenderer()
{
	d3d = 0;
	camera = 0;
	mesh = 0;
	lightShader = 0;
	light = 0;
	bitmap = 0;
}
bool CRenderer::Initialize(int width, int height, HWND hwnd)
{
	bool success;

	d3d = new CD3DHandler;
	if (!d3d)
		return false;

	success = d3d->Initialize(width, 
						      height, 
							  Settings::WaitForVSync, 
							  hwnd, 
							  Settings::Fullscreen, 
							  Settings::ScreenDepth, 
							  Settings::ScreenNear);

	if (!success)
	{
		CLog::Write("CRenderer::Initialize() : could not initialize Direct3D 11");
		return false;
	}

	camera = new CCamera;
	if (!camera)
	{
		CLog::Write("CRenderer::Initialize() : could not create the camera");
		return false;
	}
	camera->SetPosition(0.0f, 300.0f, 100.0f);
	camera->SetRotation(100.0f, 0.0f, 0.0f);

		mesh = new CMesh;
	if (!mesh)
	{
		CLog::Write("CRenderer::Initialize() : could not create the triangle mesh");
		return false;
	}
	success = mesh->Initialize(d3d->GetDevice(), "Assets/crate.dds", "Models/frost.csm");
	if (!success)
	{
		CLog::Write("CRenderer::Initialize() : could not initialize the mesh");
		return false;
		// Crashing when a model cannot load ? GREAT IDEA
	}

	textureShader = new CTextureShader;
	if (!textureShader)
	{
		return false;
	}
	success = textureShader->Initialize(d3d->GetDevice(), hwnd);
	if (!success)
	{
		return false;
	}

	lightShader = new CLightShader;
	if (!lightShader)
	{
		CLog::Write("CRenderer::Initialize() : could not create the shader");
		return false;
	}

	success = lightShader->Initialize(d3d->GetDevice(), hwnd);
	if (!success)
	{
		CLog::Write("CRenderer::Initialize() : could not initialize the shader");
		return false;
	}

	light = new CLight;
	if (!light)
	{
		CLog::Write("CRenderer::Initialize() : could not initialize the diffuse light");
		return false;
	}

	light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	light->SetDiffuseColor(0.75f, 0.74f, 0.69f, 1.0f);
	light->SetDirection(1.0f, -0.5f, 0.5f);
	light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	light->SetSpecularPower(32.0f);

	bitmap = new CBitmap;
	if (!bitmap)
	{
		CLog::Write("CRenderer::Initialize : could not create the bitmap");
		return false;
	}

	success = bitmap->Initialize(d3d->GetDevice(), width, height, "Assets/hud.dds", 256, 256);
	if (!success)
	{
		CLog::Write("Could not initialize the bitmap object");
		return false;
	}
    return true;
}
void CRenderer::Shutdown()
{
	if (bitmap)
	{
		bitmap->Shutdown();
		delete bitmap;
	}
	if (light)
	{
		delete light;
		light = 0;
	}
	if (lightShader)
	{
		lightShader->Shutdown();
		delete lightShader;
		lightShader = 0;
	}

	if (mesh)
	{
		mesh->Shutdown();
		delete mesh;
		mesh = 0;
	}

	if (camera)
	{
		delete camera;
		camera = 0;
	}

    // If the pointer is not initialized, we can assume it was never instanced
	if (d3d)
	{
		d3d->Shutdown();
		delete d3d;
		d3d = 0;
	}

	return;
}

bool CRenderer::Frame()
{	
	static float rotationY = 0.0f;

	rotationY += (float)D3DX_PI * 0.01f;
	if (rotationY > 360.0f)
	{
		rotationY -= 360.0f;
	}

	if (!Render(rotationY))
		return false;

    return true;
}

bool CRenderer::Render(float rotationY)
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix, orthoMatrix;
	bool result;

	d3d->ClearBackground(0, 0, 0, 255);

	// Generate the view matrix
	camera->Render();

	// Get the world, view and projection matrices
	camera->GetViewMatrix(viewMatrix);
	d3d->GetWorldMatrix(worldMatrix);
	d3d->GetProjectionMatrix(projectionMatrix);
	d3d->GetOrthoMatrix(orthoMatrix);

	// Start UI Rendering
	d3d->TurnZBufferOff();
	result = bitmap->Render(d3d->GetDeviceContext(), 0, 0);
	if (!result)
	{
		return false;
	}

	result = textureShader->Render(d3d->GetDeviceContext(), bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, bitmap->GetTexture());
	if (!result)
	{
		return false;
	}

	// Turn the Z buffer back on for 3D Rendering
	d3d->TurnZBufferOn();

	D3DXMatrixRotationZ(&worldMatrix, rotationY);
	

	// Prepare the model vertex and index buffers on the raphics pipeline
	mesh->Render(d3d->GetDeviceContext());

	// Render the model using the color shader
	result = lightShader->Render(d3d->GetDeviceContext(),
										mesh->GetIndexCount(), 
										worldMatrix, 
										viewMatrix, 
										projectionMatrix, 
										mesh->GetTexture(), 
										light->GetDirection(),
										light->GetAmbientColor(),
										light->GetDiffuseColor(),
										camera->GetPosition(),
										light->GetSpecularColor(),
										light->GetSpecularPower());
	if (!result)
	{
		CLog::Write("CRenderer::Render() : could not render the mesh");
		return false;
	} 


	d3d->Display();

	return true;
}