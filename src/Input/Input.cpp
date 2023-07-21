#include "../globals.hpp"
#include "Input.hpp"

MouseManager::MouseManager(){ }

void MouseManager::Update(float frameTime){
    newInput = false;

    if (GetTouchPointCount() > 0){
        Vector2 touchReported = GetTouchPosition(0);
        if (touchReported != touchDeb){
            newInput = true;
            mouseLocation = touchReported;
        }
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        newInput = true;
        mouseLocation = GetMousePosition();
        return;
    }
}