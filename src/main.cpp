#define RAYGUI_IMPLEMENTATION //Because we love extra configs
#include <string>

#include "raylib.h"
#include "raymath.h"
#include "raygui.h"

#include "globals.hpp"
#include "Input/Input.hpp"
#include "Simulator/Simulator.hpp"
#include "Objects/Objects.hpp"

using namespace std;

int Debug_Scope::context = 0;

int main(){
    InitWindow(window_height, window_length, "Gravity Simulator");
    SetTargetFPS(60);

    Simulator sim;

    Object sun = Object{Vector2{window_length/2, window_height/2}, Vector2{0, 0}, 10000.f, 20.f, true, false, YELLOW};
    Object planet = Object{Vector2{window_length/2 + 100, window_height/2}, Vector2{0, -50}, 500.f, 5.f, false, false, BROWN};
    sim.AddObject(sun);
    sim.AddObject(planet);

    bool insertingObjects = false;

    //Data for adding objects
    bool holding = false;
    Vector2 addPosition = Vector2Zero();
    Vector2 addVelocity = Vector2Zero();
    Color   addColor = WHITE;
    bool    addAnchored = false;
    bool    addInvincible = false;
    float   addRadius = 5.f;
    float   addMass = 300.f;

    while (!WindowShouldClose()){
        //Input Management

        //State Update
        float frameTime = GetFrameTime();
        sim.Update(frameTime);

        //Rendering State
        BeginDrawing();
            ClearBackground(BLACK);
            GuiLabel(Rectangle{100, 100, 50, 50}, "Attempt 1");
            //GuiSliderBar(Rectangle{0, 0, window_length / 3, 10}, "", ("Time Constant: " + to_string(sim.timeConstant)).c_str(), &sim.timeConstant, 0, 2.f);

            bool pause_time = GuiButton(Rectangle{4, 4, 32, 32}, "");
            int time_icon = (sim.timeConstant > 0) ? ICON_PLAYER_PAUSE : ICON_PLAYER_PLAY;

            GuiDrawIcon(time_icon, 4, 4, 2, BLACK);
            if (pause_time){
                if (time_icon == ICON_PLAYER_PAUSE){
                    sim.timeConstant = 0.0f;
                }else{
                    sim.timeConstant = 1.f;
                }
            }



            GuiToggle(Rectangle{42, 4, 32, 32}, "", &insertingObjects);
            int place_icon = insertingObjects ? ICON_BOX_CIRCLE_MASK : ICON_BOX;
            GuiDrawIcon(place_icon, 42, 4, 2, BLACK);

            if (insertingObjects){
                GuiGroupBox(Rectangle{0, window_height-250, 250, 250}, "Object Options");
                GuiColorPicker(Rectangle{10, window_height-250 + 10, 120, 120}, "Object Color", &addColor);
                GuiSliderBar(Rectangle{10, window_height-250 + 140, 140, 20}, "", ("Mass: " + to_string(addMass)).c_str(), &addMass, 50.f, 10000.f);
                GuiSliderBar(Rectangle{10, window_height-250 + 170, 140, 20}, "", ("Radius: " + to_string(addRadius)).c_str(), &addRadius, 1.f, 20.f);
                GuiCheckBox(Rectangle{10, window_height-250 + 200, 20, 20}, "Anchored?", &addAnchored);
                GuiCheckBox(Rectangle{150, window_height-250 + 200, 20, 20}, "Invincible?", &addInvincible);

                int mX = GetMouseX();
                int mY = GetMouseY();

                bool buttonLimit1 = mX < 100 && mY < 50; //Limit it so that it cannot accidentally happen when clicking the buttons
                bool buttonLimit2 = mX < 250 && mY > (window_height-250);

                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !buttonLimit1 && !buttonLimit2){ 
                    if (!holding) {
                        addPosition = Vector2{(float)mX, (float)mY};
                        holding = true;
                    }

                    addVelocity = Vector2{(float)mX - addPosition.x, (float)mY - addPosition.y};

                    DrawLine(addPosition.x, addPosition.y, mX, mY, addColor);
                    DrawCircle(addPosition.x, addPosition.y, addRadius, addColor);
                }else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && holding){
                    holding = false;

                    Object obj = Object{addPosition, addVelocity, addMass, addRadius, addAnchored, addInvincible, addColor};
                    bool succ = sim.AddObject(obj);
                    if (!succ){
                        //TODO
                    }
                }
            }

            if (sim.timeConstant <= 0.1f){
                sim.DrawPredictions(frameTime);
            }
            
            sim.DrawObjects();
        EndDrawing();
    }

    return 0;
}

bool operator==(Vector2 lhs, Vector2 rhs){
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator!=(Vector2 lhs, Vector2 rhs){
    return !(lhs == rhs);
}

Vector2 operator/(Vector2 lhs, float rhs){
    return Vector2{lhs.x / rhs, lhs.y / rhs};
}

Vector2 operator*(Vector2 lhs, float rhs){
    return Vector2{lhs.x * rhs, lhs.y * rhs};
}

Vector2 operator*(float lhs, Vector2 rhs){
    return rhs * lhs;
}

Vector2 operator+(Vector2 lhs, Vector2 rhs){
    return Vector2{lhs.x + rhs.x, lhs.y + rhs.y};
}

Vector2 operator-(Vector2 lhs, Vector2 rhs){
    return lhs + (-1 * rhs);
}