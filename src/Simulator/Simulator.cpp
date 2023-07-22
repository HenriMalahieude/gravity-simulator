#include "raylib.h"
#include "raymath.h"

#include "../globals.hpp"
#include "Simulator.hpp"

using namespace std;

Vector2 calculateForce(Object base, Object other, float gConstant) {
    //F = (G * m1 * m2) / (dist^2)
    float distance = Vector2Distance(base.position, other.position);

    float force = (gConstant * base.mass * other.mass) / powf(distance, 2.f);
    Vector2 direction = Vector2Normalize(other.position - base.position);

    return direction * force;
}

void WorldTickRoutine(float frameTime, float timeConstant, float gConstant, vector<Object> &world) {
    //Calculate Forces and Update Velocities & Positions
    for (size_t i = 0; i < world.size(); i++){
        Object *obj = &(world[i]);
        float timing = frameTime * timeConstant;
        if (obj->anchored) continue;

        Vector2 updatedVelocity = obj->velocity;
        for (size_t j = 0; j < world.size(); j++){
            if (j == i) continue;
            //Debug_Print("Calling Calculate Force between ", i, " and ", j);
            Vector2 force = calculateForce(*obj, world[j], gConstant);
            //NOTE: If I ever want to optimize this, I can do some "dynamic programming" and instead save this for the j-th object too
            
            //F = ma -> F/m = a
            //Debug_Print("Got Force: ", force.x, ", ", force.y);
            Vector2 accel = (force / obj->mass) * (timing);
            updatedVelocity = updatedVelocity + accel;
        }

        obj->velocity = updatedVelocity;
        obj->position = Vector2Add(obj->position, Vector2Scale(obj->velocity, timing));
        //Debug_Print("New Vel: ", updatedVelocity.x, ", ", updatedVelocity.y);
    }
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
            //Debug_Print("Deleted Planet");
            advance(it, -1); 
        }
    }

    WorldTickRoutine(frameTime, timeConstant, gConstant, world);

    //Collision Detection
    //NOTE: There is a bug that causes radius collision not to change despite increased radius. Idk why, and I can't be bothered to fix it
    for (size_t i = 0; i < world.size(); i++){
        Object *obj1 = &world[i];
        for (size_t j = 0; j < world.size(); j++){
            if (i == j) continue;

            Object *obj2 = &world[j];
            float dist = Vector2Distance(obj1->position, obj2->position);
            if (dist <= obj1->radius || dist <= obj2->radius){ //They've collided, therefore...
                if (obj1->invincible){
                    world.erase(next(world.begin(), j));
                }else if (obj2->invincible){
                    world.erase(next(world.begin(), i));
                }else{
                    Vector2 momentumObj1 = obj1->velocity * obj1->mass;
                    Vector2 momentumObj2 = obj2->velocity * obj2->mass;
                    //Debug_Print("Collision Detected");
                    if (obj1->mass > obj2->mass){ //Conservation of Momentum
                        //Debug_Print("Object 1 with ", obj1->mass, "kg wins");
                        obj1->mass += obj2->mass;
                        obj1->velocity = (momentumObj1 + momentumObj2) / obj1->mass;
                        obj1->radius *= (1.f + (0.15f * (obj2->radius / obj2->radius)));

                        world.erase(next(world.begin(), j));
                    }else{
                        //Debug_Print("Object 2 with ", obj2->mass, "kg wins");
                        obj2->mass += obj1->mass;
                        obj2->velocity = (momentumObj1 + momentumObj2) / obj2->mass;
                        obj2->radius *= (1.f + (0.15f * (obj1->radius / obj2->radius)));

                        world.erase(next(world.begin(), i));
                    }
                }

                i--; j--; //Since we've just reduced the sizing of the worlds array
            }
        }
    }
}

void Simulator::DrawPredictions(int centerIndex) {
    if (timeConstant > 0.001f || world.size() <= 1) return;

    const int PREDICT_ITER_LIM = 100000;
    const float TIME_INTERVAL = 1.f/60.f;

    if (predictionUpdate){
        predictionUpdate = false;
        predictions.clear();

        vector<Object> predictWorld = world;

        int predictionsPerObject = PREDICT_ITER_LIM / (int)world.size();
        
        for (int i = 0; i < predictionsPerObject; i++){
            WorldTickRoutine(TIME_INTERVAL, 1.f, gConstant, predictWorld);
            for (size_t j = 0; j < predictWorld.size(); j++){
                predictions.push_back(Dot{predictWorld[j].clr, predictWorld[j].position});
            }
        }
    }

    for (size_t i = 0; i < predictions.size(); i++){
        Dot dd = predictions[i];
        DrawPixel(dd.position.x, dd.position.y, dd.cc);
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

    predictionUpdate = true;

    world.push_back(obj);
    return true;
}

void Simulator::ClearAll(){
    predictionUpdate = true;

    world.clear();
}

Object Simulator::SelectObject(int x, int y){
    Object details;

    for (size_t i = 0; i < world.size(); i++){
        Vector2 mousePos = Vector2{(float)x, (float)y};
        float dist = Vector2Distance(mousePos, world[i].position);
        if (dist < world[i].radius){
            return world[i];
        }
    }

    return details;
}