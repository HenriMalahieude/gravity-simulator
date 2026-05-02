#define RAYGUI_IMPLEMENTATION
#include <time.h>
#include <string>
#include <sstream>

#include "raylib.h"
#include "raygui.h"
#include "raymath.h"

#include "globals.hpp"
#include "Simulator/Simulator.hpp"
#include "Objects/Objects.hpp"

using namespace std;

int Debug_Scope::context = 0;

int main(){
    srand(time(NULL));
    InitWindow(window_height, window_length, "Gravity Simulator");
    SetTargetFPS(target_fps);

    Simulator sim;
    sim.ResetWorld();

    Object *details = &sim.world[0];
    stringstream ss; ss.precision(2); ss << fixed;
    bool minimizeDetails = false;

    bool insertingObjects = true;
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
                bool reset = GuiButton(Rectangle{4, 118, 32, 32}, "");
                GuiDrawIcon(ICON_CROSS, 4, 118, 2, BLACK);
                if (reset){
                    sim.ClearAll();
                }

                GuiToggle(Rectangle{42, 42, 32, 32}, "", &predictOrNot);
                int predictIcon = predictOrNot ? ICON_EYE_ON : ICON_EYE_OFF;
                GuiDrawIcon(predictIcon, 42, 42, 2, BLACK);

                bool def = GuiButton(Rectangle{4, 42, 32, 32}, "");
                GuiDrawIcon(ICON_STAR, 5, 42, 2, BLACK);
                if (def) {
                    sim.ResetWorld();
                }

                bool chaos = GuiButton(Rectangle{4, 80, 32, 32}, "");
                GuiDrawIcon(ICON_DEMON, 3, 80, 2, BLACK);
                if (chaos) {
                    sim.ChaosWorld();
                }
            }

            //Time Control
            bool change = GuiButton(Rectangle{window_length-36, 4, 32, 32}, "");
            GuiDrawIcon(ICON_CLOCK, window_length-35, 4, 2, BLACK);
            DrawText((to_string((int)sim.timeConstant) + "x").c_str(), window_length-32, 40, 20, WHITE);
            if (change) {
                sim.timeConstant += 1;
                if (sim.timeConstant > 5.f) sim.timeConstant = 1.f;
            }

            if (insertingObjects){ //Inserting Object
                details = nullptr;

                Rectangle pos = Rectangle{4, 0, 32, 32};
                pos.x = (!minimizeOptions) ? 190 : 4;
                pos.y = (!minimizeOptions) ? (window_height - 240 + 20) : window_height - 36;
                GuiToggle(pos, "", &minimizeOptions);
                int minIcon = (!minimizeOptions) ? ICON_ARROW_DOWN : ICON_ARROW_UP;
                GuiDrawIcon(minIcon, pos.x, pos.y, 2, BLACK);

                if (!minimizeOptions){
                    GuiGroupBox(Rectangle{0, window_height-240, 250, 240}, "Click&Drag to Insert Object");
                    GuiColorPicker(Rectangle{10, window_height-240 + 10, 120, 120}, "Object Color", &addColor);

                    ss << addMass;
                    string massStr = "Mass: " + ss.str();
                    GuiSliderBar(Rectangle{10, window_height-240 + 140, 140, 20}, "", massStr.c_str(), &addMass, 50.f, 10000.f);
                    ss.str(""); ss.clear();

                    ss << addRadius;
                    string radiusStr = "Radius: " + ss.str();
                    GuiSliderBar(Rectangle{10, window_height-240 + 170, 140, 20}, "", radiusStr.c_str(), &addRadius, 1.f, 25.f);
                    ss.str(""); ss.clear();

                    GuiCheckBox(Rectangle{10, window_height-240 + 200, 20, 20}, "Anchored?", &addAnchored);
                    GuiCheckBox(Rectangle{150, window_height-240 + 200, 20, 20}, "Invincible?", &addInvincible);
                }

                int mX = GetMouseX();
                int mY = GetMouseY();

                bool buttonLimit1 = mX < 100 && mY < 170; //Limit it so that it cannot accidentally happen when clicking the buttons
                bool buttonLimit2 = mX < 250 && mY > (window_height-240) && !minimizeOptions;
                bool buttonLimit3 = mX < 50 && mY > (window_height-50);
                bool buttonLimit4 = mX > (window_length-50) && mY < 50;

                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !buttonLimit1 && !buttonLimit2 && !buttonLimit3 && !buttonLimit4) { 
                    if (!holding) {
                        addPosition = Vector2{(float)mX, (float)mY};
                        holding = true;
                    }

                    addVelocity = Vector2{(float)mX - addPosition.x, (float)mY - addPosition.y};

                    DrawLine(addPosition.x, addPosition.y, mX, mY, addColor);
                    DrawCircle(addPosition.x, addPosition.y, addRadius, addColor);
                }else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && holding){
                    holding = false;

                    Object obj = Object{addPosition, addVelocity, Vector2Zero(), addMass, addRadius, addAnchored, addInvincible, addColor};
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
                    Object nullobj;
                    if (details == nullptr) details = &nullobj;

                    string anchored = (details->anchored) ? "true" : "false";
                    string invincible = (details->invincible) ? "true" : "false";

                    GuiGroupBox(Rectangle{window_length-200, window_height-200, 200, 200}, "Click on Object to See Details");
                    DrawCircle(window_length - 200 + 40, window_height - 200 + 40, details->radius, details->clr);

                    ss << details->mass;
                    string massStr = "Mass: " + ss.str();
                    GuiLabel(Rectangle{window_length - 200 + 10, window_height - 200 + 70, 190, 25}, massStr.c_str());
                    ss.str(""); ss.clear();
                    
                    ss << details->radius;
                    string radiusStr = "Radius: " + ss.str();
                    GuiLabel(Rectangle{window_length - 200 + 10, window_height - 200 + 90, 190, 25}, radiusStr.c_str());
                    ss.str(""); ss.clear();

                    ss << details->position.x;
                    string posStr = "Position: <" + ss.str();
                    ss.str(""); ss.clear();
                    ss << details->position.y;
                    posStr += ", " + ss.str() + ">";
                    GuiLabel(Rectangle{window_length - 200 + 10, window_height -200 + 110, 190, 25}, posStr.c_str());
                    ss.str(""); ss.clear();
                    
                    ss << details->velocity.x;
                    string velStr = "Velocity: <" + ss.str();
                    ss.str(""); ss.clear();
                    ss << details->velocity.y;
                    velStr += ", " + ss.str() + ">";
                    GuiLabel(Rectangle{window_length - 200 + 10, window_height - 200 + 150, 190, 25}, velStr.c_str());
                    ss.str(""); ss.clear();
                    
                    ss << details->acceleration.x;
                    string accStr = "Acceleration X: " + ss.str();
                    ss.str(""); ss.clear();
                    ss << details->acceleration.y;
                    accStr += ", " + ss.str() + ">";
                    GuiLabel(Rectangle{window_length - 200 + 10, window_height - 200 + 170, 190, 25}, accStr.c_str());
                    ss.str(""); ss.clear();

                    if (details == &nullobj) details = nullptr;
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

Vector2 operator+=(Vector2 &lhs, Vector2 rhs){
    lhs = lhs + rhs;
    return lhs;
}