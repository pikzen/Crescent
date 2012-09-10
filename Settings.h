#ifndef SETTINGS_H
#define SETTINGS_H
#pragma once
#include <windows.h>
#include <D3D11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>


namespace Settings
{
    extern char* Name;
    extern char* WindowName;
    extern bool Fullscreen;
    extern int FSWidth;
    extern int FSHeight;
    extern int WinWidth;
    extern int WinHeight;
    extern DWORD WinStyle;
    extern bool WaitForVSync;
	extern float ScreenDepth;
	extern float ScreenNear;
	extern UINT AdapterNum;
	extern int MSAACount;
	extern int MSAAQuality;
	extern bool LineAA;
	extern bool EnableMSAA;
	extern float FieldOfView;
	extern D3D11_FILTER TextureFilter;

    /* data */
}
#endif