#include "raylib.h"

#ifndef __OBJECTS_HPP__
#define __OBJECTS_HPP__

struct Object{
    Vector2 position = Vector2{0, 0};
    Vector2 velocity = Vector2{0, 0};
    float mass = 100.f;
    float radius = 1.f;
    bool anchored = false;
    bool invincible = false;
    Color clr = WHITE;
};

#endif