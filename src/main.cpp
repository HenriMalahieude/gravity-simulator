#include "raylib.h"
#include "raygui.h"

#include "../Input/Input.hpp"

const int window_height = 900;
const int window_length = 900;

int main(){
    InitWindow(window_height, window_length, "Gravity Simulator");
    SetTargetFPS(60);



    while (!WindowShouldClose()){
        //Input Management

        //State Update

        //Rendering State
        BeginDrawing();
            ClearBackground(BLACK);

        EndDrawing();
    }

    return 0;
}