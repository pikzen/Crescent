#ifndef CINPUT_H
#define CINPUT_H

class CInput
{
public:
    CInput();

    void Initialize();
    void KeyDown(unsigned int);
    void KeyUp(unsigned int);
    bool IsKeyDown(unsigned int);

private:
    bool keys[256];
    /* data */
};

#endif