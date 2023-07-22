#define RAYGUI_IMPLEMENTATION
#include <string>

#include "raylib.h"
#include "raygui.h"
#include "raymath.h"

#include "globals.hpp"
#include "Simulator/Simulator.hpp"
#include "Objects/Objects.hpp"

using namespace std;

int Debug_Scope::context = 0;

int main(){
    InitWindow(window_height, window_length, "Gravity Simulator");
    SetTargetFPS(60);

    Simulator sim;

    Object sun = Object{Vector2{window_length/2, window_height/2}, Vector2{0, 0}, 10000.f, 20.f, true, true, YELLOW};
    Object planet = Object{Vector2{window_length/2 + 150, window_height/2}, Vector2{0, -18.5f}, 1000.f, 5.f, false, false, MAROON};
    Object moon = Object{Vector2{planet.position.x + 15, planet.position.y}, Vector2{0, 2.f}, 10.f, 2.f, false, false, GRAY};
    sim.AddObject(sun);
    sim.AddObject(planet);
    sim.AddObject(moon);

    Object details = sun;
    bool minimizeDetails = false;

    bool insertingObjects = false;
    bool minimizeOptions = false;

    bool predictOrNot = true;

    //Data for adding objects
    bool holding = false;
    Vector2 addPosition = Vector2Zero();
    Vector2 addVelocity = Vector2Zero();
    Color   addColor = WHITE;
    bool    addAnchored = false;
    bool    addInvincible = false;
    float   addRadius = 10.f;
    float   addMass = 1000.f;
    
    while (!WindowShouldClose()){
        //Input Management

        //State Update
        float frameTime = GetFrameTime();
        sim.Update(frameTime);

        //Rendering State
        BeginDrawing();
            ClearBackground(BLACK);
            if (GetTouchPointCount() > 0){
                DrawText("Some Features (like object insertion) do not have mobile support.", 90, 5, 15, RED);
            }

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
            int place_icon = insertingObjects ? ICON_BOX_CIRCLE_MASK : ICON_CURSOR_CLASSIC;
            GuiDrawIcon(place_icon, 42, 4, 2, BLACK);

            if (sim.timeConstant <= 0.1f){
                bool reset = GuiButton(Rectangle{4, 42, 32, 32}, "");
                GuiDrawIcon(ICON_CROSS, 4, 42, 2, BLACK);
                if (reset){
                    sim.ClearAll();
                }

                GuiToggle(Rectangle{42, 42, 32, 32}, "", &predictOrNot);
                int predictIcon = predictOrNot ? ICON_EYE_ON : ICON_EYE_OFF;
                GuiDrawIcon(predictIcon, 42, 42, 2, BLACK);
            }

            if (insertingObjects){ //Inserting Object
                details = Object{};

                Rectangle pos = Rectangle{4, 0, 32, 32};
                pos.x = (!minimizeOptions) ? 190 : 4;
                pos.y = (!minimizeOptions) ? (window_height - 240 + 20) : window_height - 36;
                GuiToggle(pos, "", &minimizeOptions);
                int minIcon = (!minimizeOptions) ? ICON_ARROW_DOWN : ICON_ARROW_UP;
                GuiDrawIcon(minIcon, pos.x, pos.y, 2, BLACK);

                if (!minimizeOptions){
                    GuiGroupBox(Rectangle{0, window_height-240, 250, 240}, "Object Options");
                    GuiColorPicker(Rectangle{10, window_height-240 + 10, 120, 120}, "Object Color", &addColor);
                    GuiSliderBar(Rectangle{10, window_height-240 + 140, 140, 20}, "", ("Mass: " + to_string(addMass)).c_str(), &addMass, 50.f, 10000.f);
                    GuiSliderBar(Rectangle{10, window_height-240 + 170, 140, 20}, "", ("Radius: " + to_string(addRadius)).c_str(), &addRadius, 1.f, 25.f);
                    GuiCheckBox(Rectangle{10, window_height-240 + 200, 20, 20}, "Anchored?", &addAnchored);
                    GuiCheckBox(Rectangle{150, window_height-240 + 200, 20, 20}, "Invincible?", &addInvincible);
                }

                int mX = GetMouseX();
                int mY = GetMouseY();

                bool buttonLimit1 = mX < 100 && mY < 100; //Limit it so that it cannot accidentally happen when clicking the buttons
                bool buttonLimit2 = mX < 250 && mY > (window_height-240) && !minimizeOptions;
                bool buttonLimit3 = mX < 50 && mY > (window_height-50);

                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !buttonLimit1 && !buttonLimit2 && !buttonLimit3){ 
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
            } else { //Selecting Objects
                Rectangle pos = Rectangle{window_length-36, 0, 32, 32};
                pos.y = (!minimizeDetails) ? window_height-160 : window_height - 40;
                GuiToggle(pos, "", &minimizeDetails);
                int minIcon = (!minimizeDetails) ? ICON_ARROW_DOWN : ICON_ARROW_UP;
                GuiDrawIcon(minIcon, pos.x, pos.y, 2, BLACK);

                if (!minimizeDetails){
                    string anchored = (details.anchored) ? "true" : "false";
                    string invincible = (details.invincible) ? "true" : "false";

                    GuiGroupBox(Rectangle{window_length-200, window_height-200, 200, 200}, "Object Freeze-Frame");
                    DrawCircle(window_length - 200 + 40, window_height - 200 + 40, details.radius, details.clr);
                    GuiLabel(Rectangle{window_length - 200 + 10, window_height - 200 + 70, 190, 25}, ("Mass: " + to_string(details.mass)).c_str());
                    GuiLabel(Rectangle{window_length - 200 + 10, window_height - 200 + 90, 190, 25}, ("Radius: " + to_string(details.radius)).c_str());
                    GuiLabel(Rectangle{window_length - 200 + 10, window_height - 200 + 110, 190, 25}, ("Velocity: (" + to_string(details.velocity.x) + ", " + to_string(details.velocity.y) + ")").c_str());
                    GuiLabel(Rectangle{window_length - 200 + 10, window_height - 200 + 130, 190, 25}, ("Position: (" + to_string(details.position.x) + ", " + to_string(details.position.y) + ")").c_str());
                    GuiLabel(Rectangle{window_length - 200 + 10, window_height - 200 + 150, 190, 25}, ("Anchored? " + anchored).c_str());
                    GuiLabel(Rectangle{window_length - 200 + 10, window_height - 200 + 170, 190, 25}, ("Invincible? " + invincible).c_str());
                }

                int mX = GetMouseX();
                int mY = GetMouseY();

                bool buttonLimit1 = mX < 100 && mY < 100; //Limit it so that it cannot accidentally happen when clicking the buttons
                bool buttonLimit3 = mX > pos.x && mY > pos.y;

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !buttonLimit1 && !buttonLimit3) {
                    details = sim.SelectObject(mX, mY);
                }
            }

            if (sim.timeConstant <= 0.1f && predictOrNot){
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