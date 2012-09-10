#include "Settings.h"

    
    char* Settings::Name = "crescent";
    char* Settings::WindowName = "crescent";

    bool Settings::Fullscreen = false;
    int Settings::FSWidth = 1680;
    int Settings::FSHeight = 1050;
    int Settings::WinWidth = 1280;
    int Settings::WinHeight = 720;

    DWORD Settings::WinStyle = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	bool Settings::WaitForVSync = true;

	float Settings::ScreenDepth = 3000.0f;
	float Settings::ScreenNear = 0.1f;

	UINT Settings::AdapterNum = 0;

	int Settings::MSAACount = 1;
	int Settings::MSAAQuality = 0;

	bool Settings::LineAA = false;

	bool Settings::EnableMSAA = false;

	float Settings::FieldOfView = (float)D3DX_PI / 4.0f;

	D3D11_FILTER Settings::TextureFilter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

