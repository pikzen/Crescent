#include "CSystem.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow)
{
	CSystem* system = new CSystem;
	if (!system)
		return 0;

	if (system->Initialize())
		system->Run();

	system->Shutdown();
	delete system;
	system = 0;

	return 0;
}