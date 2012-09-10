#ifndef CSYSTEM_H
#define CSYSTEM_H

#include <windows.h>

#include "CInput.h"
#include "CRenderer.h"
#include "CLog.h"
#include "CWindow.h"
#include "Settings.h"

class CSystem
{
public:
    CSystem();

    /* data */
    bool Initialize();
    void Shutdown();
    void Run();

    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
    bool Frame();
    void InitializeWindows(int&, int&);
    void ShutdownWindows();

private:
    LPCWSTR applicationName;
	
	CWindow* window;
    CInput* input;
    CRenderer* graphics;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static CSystem* applicationHandle = 0;
#endif