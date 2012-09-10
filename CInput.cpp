#include "CInput.h"

CInput::CInput()
{
    
}

void CInput::Initialize()
{
    for (int i = 0; i < 256; i++)
    {
        keys[i] = false;
    }

    return;
}

void CInput::KeyDown(unsigned int input)
{
    keys[input] = true;
    return;
}
void CInput::KeyUp(unsigned int input)
{
    keys[input] = false;
    return;
}

bool CInput::IsKeyDown(unsigned int key)
{
    return keys[key];
}



