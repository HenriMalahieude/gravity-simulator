#include "raylib.h"

#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

class MouseManager{
    private:
        Vector2 touchDeb = Vector2{-1, -1};

        bool newInput = false;
        Vector2 mouseLocation = Vector2{0, 0};
    public:
        MouseManager();

        void Update(float frameTime);

        bool NewInput() {return newInput;}
        Vector2 GetInputLocation() {return mouseLocation;}
};

#endif