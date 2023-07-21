#include "raylib.h"
#include "raymath.h"

#include "../globals.hpp"
#include "Simulator.hpp"

using namespace std;

Vector2 Simulator::calculateForce(int base, int other) {
    //F = (G * m1 * m2) / (dist^2)
    Object baseObj = world[base]; //from
    Object otherObj = world[other]; //to

    float distance = Vector2Distance(baseObj.position, otherObj.position);

    float force = (gConstant * baseObj.mass * otherObj.mass) / powf(distance, 2.f);
    Vector2 direction = Vector2Normalize(otherObj.position - baseObj.position);

    return direction * force;
}

void Simulator::Update(float frameTime){
    if (timeConstant <= 0.001f) return;
    predictionUpdate = true;

    //Debug_Print("Simulator Update Called"); Debug_Scope ds;

    //Out of Screen Check
    Vector2 extraBounds = Vector2{window_length / 2, window_height / 2};
    Vector2 minBounds = Vector2{-1 * extraBounds.x, -1 * extraBounds.y};
    Vector2 maxBounds = Vector2{window_length + extraBounds.x, window_height + extraBounds.y};

    for (vector<Object>::iterator it = world.begin(); it != world.end(); advance(it, 1)){
        Object obj = *it;
        bool horizontal_check = obj.position.x < minBounds.x || obj.position.x > maxBounds.x;
        bool vertical_check = obj.position.y < minBounds.y || obj.position.y > maxBounds.y;

        if (horizontal_check || vertical_check){
            world.erase(it);
            Debug_Print("Deleted Planet");
            advance(it, -1); 
        }
    }

    //Calculate Forces and Update Velocities & Positions
    for (size_t i = 0; i < world.size(); i++){
        Object obj = world[i];
        float timing = frameTime * timeConstant;
        if (obj.anchored) continue;

        Vector2 updatedVelocity = obj.velocity;
        for (size_t j = 0; j < world.size(); j++){
            if (j == i) continue;
            //Debug_Print("Calling Calculate Force between ", i, " and ", j);
            Vector2 force = calculateForce(i, j);
            //NOTE: If I ever want to optimize this, I can do some "dynamic programming" and instead save this for the j-th object too
            
            //F = ma -> F/m = a
            //Debug_Print("Got Force: ", force.x, ", ", force.y);
            Vector2 accel = (force / obj.mass) * (timing);
            updatedVelocity = updatedVelocity + accel;
        }

        world[i].velocity = updatedVelocity;
        world[i].position = Vector2Add(world[i].position, Vector2Scale(world[i].velocity, timing));
    }

    //Collision Detection
    /*for (size_t i = 0; i < world.size(); i++){
        Object obj1 = world[i];
        //TODO
    }*/
}

void Simulator::DrawPredictions(int centerIndex) {
    if (timeConstant > 0.001f) return;

    const int PREDICT_ITER_LIM = 100000;

    if (predictionUpdate){
        predictionUpdate = false;

        int predictions_per_object = PREDICT_ITER_LIM / (int)world.size();
        //TODO
    }

    for (size_t i = 0; i < predictions.size(); i++){
        Dot dd = predictions[i];
        DrawCircle(dd.position.x, dd.position.y, dd.scale, dd.cc);
    }
}

void Simulator::DrawObjects() {
    for (size_t i = 0; i < world.size(); i++){
        Object obj = world[i];

        //Velocity Vector
        if (!obj.anchored){
            Vector2 vel = obj.velocity;
            vel = Vector2Normalize(vel);
            vel = vel * (Vector2Distance(obj.velocity, Vector2Zero()) + obj.radius);
            DrawLine(obj.position.x, obj.position.y, obj.position.x + vel.x, obj.position.y + vel.y, WHITE);
        }

        //Celestial Object
        DrawCircle(obj.position.x, obj.position.y, obj.radius, obj.clr);
        if (obj.clr.a < 50 || (obj.clr.b < 50 && obj.clr.g < 50 && obj.clr.r < 50)){
            DrawCircleLines(obj.position.x, obj.position.y, obj.radius+0.5f, WHITE);
        }
        
    }
}

bool Simulator::AddObject(Object obj) {
    if (world.size() >= (size_t)maxObjectCount) return false;

    world.push_back(obj);
    return true;
}

void Simulator::ClearAll(){
    world.clear();
}