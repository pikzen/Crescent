#ifndef CRENDERER_H
#define CRENDERER_H
#include <windows.h>
#include "CD3DHandler.h"
#include "CCamera.h"
#include "CMesh.h"
#include "CLightShader.h"
#include "CLight.h"
#include "CBitmap.h"
#include "Settings.h"
#include "CTextureShader.h"
#include "CLog.h"

class CRenderer
{
public:
	CRenderer();

    bool Initialize(int, int, HWND);
    void Shutdown();
    bool Frame();

private:
    bool Render(float);
	CD3DHandler* d3d;
	CCamera* camera;
	CMesh* mesh;
	CTextureShader* textureShader;
	CLightShader* lightShader;
	CLight* light;
	CBitmap* bitmap;
	
};
#endif