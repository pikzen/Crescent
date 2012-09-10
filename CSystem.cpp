#include "CSystem.h"
CSystem::CSystem()
{
    input = 0;
    graphics = 0;
	window = 0;
}

bool CSystem::Initialize()
{
    int screenWidth, screenHeight;
    bool result;
	
    screenWidth = 0;
    screenHeight = 0;

    InitializeWindows(screenWidth, screenHeight);

    input = new CInput;
    if (!input)
    {
        CLog::Write("CSystem::Initialize() : Could not create an instance of the Input class");
        return false;
    }
    input->Initialize();

    graphics = new CRenderer;
    if (!graphics)
    {
        CLog::Write("CSystem::Initialize() : Could not create an instance of the Renderer class");
        return false;
    }
	result = graphics->Initialize(screenWidth, screenHeight, window->GetHandle());
    if (!result)
    {
        CLog::Write("CSystem::Initialize() : The renderer has not been initialized");
        return false;
    }

    return true;
}

void CSystem::Shutdown()
{
    if (graphics)
    {
        graphics->Shutdown();
        delete graphics;
        graphics = 0;
    }
    if (input)
    {
        delete input;
        input = 0;
    }

    ShutdownWindows();

    return;
}

void CSystem::Run()
{
    MSG msg;
    bool done, result;

    // The msg struct must be zeroed
    ZeroMemory(&msg, sizeof(MSG));

    // Loop until a quit message is received
    done = false;
    while(!done)
    {
        // Basic message handling
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
        {
            done = true;
        }
        else
        {
            result = Frame();
            if (!result)
            {
                done = true;
            }
        }
    }

    return;
}

bool CSystem::Frame()
{
    bool result;

    if (input->IsKeyDown(VK_F9))
    {
        return false;
    }

    result = graphics->Frame();
    {
        if (!result)
        {
            return false;
        }
    }

    return true;
}

LRESULT CALLBACK CSystem::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch (umsg)
    {
        case WM_KEYDOWN:
        {
            input->KeyDown((unsigned int)wparam);
            return 0;
        }
        case WM_KEYUP:
        {
            input->KeyUp((unsigned int)wparam);
            return 0;
        }

        default:
        {
            return DefWindowProc(hwnd, umsg, wparam, lparam);
        }
    }
}

void CSystem::InitializeWindows(int& screenWidth, int& screenHeight)
{
	applicationHandle = this;
	window->GetWindow(screenWidth, screenHeight, WndProc);

	ShowWindow(window->GetHandle(), SW_SHOW);


}

void CSystem::ShutdownWindows()
{
	window->Destroy();
    ShowCursor(true);

    if (Settings::Fullscreen)
    {
        ChangeDisplaySettings(NULL, 0);
    }

    applicationHandle = NULL;

    return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch (umsg)
    {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        }

        default:
        {
            return applicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
        }
    }
}