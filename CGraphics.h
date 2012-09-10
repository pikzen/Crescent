#ifndef CGRAPHICS_H
#define CGRAPHICS_H
#include <windows.h>

const float screenDepth = 1000.0f;
const float screenNear = 0.1f;

class CGraphics
{
public:
    CGraphics();

    bool Initialize(int, int, HWND);
    void Shutdown();
    bool Frame();

private:
    bool Render();

    /* data */
};


#endif