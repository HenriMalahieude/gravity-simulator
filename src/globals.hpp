#include <iostream>
#include "raylib.h"

#ifndef __GLOBALS__HPP_
#define __GLOBALS__HPP_

const int window_height = 800;
const int window_length = 800;

struct Debug_Scope{
    static int context;

    Debug_Scope(){context++;}
    ~Debug_Scope(){context--;}
};

/*template<class ... Args>
static void Debug_Print(Args&&... args){
    for (int i = 0; i < Debug_Scope::context; i++) std::cout << "\t";
    (std::cout << ... << std::forward<Args>(args)) << std::endl;
}*/

bool operator==(Vector2 lhs, Vector2 rhs);

bool operator!=(Vector2 lhs, Vector2 rhs);

Vector2 operator/(Vector2 lhs, float rhs);

Vector2 operator*(Vector2 lhs, float rhs);
Vector2 operator*(float lhs, Vector2 rhs);

Vector2 operator+(Vector2 lhs, Vector2 rhs);

Vector2 operator-(Vector2 lhs, Vector2 rhs);

#endif