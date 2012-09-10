#ifndef CWINDOW_H
#define CWINDOW_H

#include <Windows.h>

class CWindow {
public: 
	HWND GetHandle();
	HINSTANCE GetHInstance();

	bool GetWindow(int width, int height, WNDPROC wndProc);
	void Destroy();

private:
	HWND Handle;
	HINSTANCE Instance;
};

#endif