#include "raylib.h"

#ifndef __OBJECTS_HPP__
#define __OBJECTS_HPP__

struct Object{
    Vector2 position = Vector2{-1, -1};
    Vector2 velocity = Vector2{-1, -1};
    float mass = 10.f;
    float radius = 1.f;
    bool anchored = false;
    bool invincible = false;
    Color clr = RED;
};

#endif