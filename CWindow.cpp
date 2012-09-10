#include "CWindow.h"
#include "Settings.h"

bool CWindow::GetWindow(int width, int height, WNDPROC wndProc)
{
	WNDCLASSEX wc;
    DEVMODE dmScreenSettings;
    int posX, posY;
    
    Instance = (HINSTANCE)GetModuleHandle(NULL);

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = wndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = Instance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = Settings::Name;
    wc.cbSize = sizeof(WNDCLASSEX);

    RegisterClassEx(&wc);

    DWORD WinStyle;

    if (Settings::Fullscreen == true)
    {
        width = Settings::FSWidth;
        height = Settings::FSHeight;

        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = (unsigned long)width;
        dmScreenSettings.dmPelsHeight = (unsigned long)height;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

        WinStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;

        posX = posY = 0;
    }
    else
    {
        WinStyle = WS_CAPTION;
        width = Settings::WinWidth;
        height = Settings::WinHeight;

        posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
    }

    Handle = CreateWindowEx(WS_EX_APPWINDOW,
                          Settings::Name,
                          Settings::Name,
                          WinStyle,
                          posX,
                          posY,
                          width,
                          height,
                          NULL,
                          NULL,
                          Instance,
                          NULL);
	return true;
}

HWND CWindow::GetHandle()
{
	return Handle;
}

HINSTANCE CWindow::GetHInstance()
{
	return Instance;
}
void CWindow::Destroy()
{
	DestroyWindow(Handle);
	Handle = NULL;


	UnregisterClass(Settings::Name, Instance);
}