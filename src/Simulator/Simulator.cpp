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
    //Zero Accelerations
    for (size_t i = 0; i < world.size(); i++) {
        Object *obj = &(world[i]);
        obj->acceleration = Vector2Zero();
    }

    //Calculate Forces O(n log(n))
    for (size_t i = 0; i < world.size(); i++){ //n
        Object *obj = &(world[i]);

        //Vector2 updatedVelocity = obj->velocity;
        for (size_t j = i+1; j < world.size(); j++){ //log(n)
            Object *oth = &(world[j]);
            if (oth->anchored && obj->anchored) continue;
            //Debug_Print("Calling Calculate Force between ", i, " and ", j);
            Vector2 force = calculateForce(*obj, *oth, gConstant);
            
            //F = ma -> F/m = a
            //Debug_Print("Got Force: ", force.x, ", ", force.y);
            if (!obj->anchored) obj->acceleration += (force / obj->mass);
            if (!oth->anchored) oth->acceleration += (force / oth->mass) * -1.f; //Newton's Third Law
        }

        //obj->velocity = updatedVelocity;
        //obj->position = Vector2Add(obj->position, Vector2Scale(obj->velocity, step));
        //Debug_Print("New Vel: ", updatedVelocity.x, ", ", updatedVelocity.y);
    }

    //Apply Forces
    float step = frameTime * timeConstant;
    for (size_t i = 0; i < world.size(); i++){
        Object *obj = &(world[i]);
        if (obj->anchored) continue;

        obj->velocity += obj->acceleration * step;
        obj->position += obj->velocity * step;
    }
}

void WorldCollisionUpdate(float frameTime, vector<Object> &world) {
    //Out of Screen Check
    Vector2 extraBounds = Vector2{window_length / 2, window_height / 2};
    Vector2 minBounds = Vector2{-1 * extraBounds.x, -1 * extraBounds.y};
    Vector2 maxBounds = Vector2{window_length + extraBounds.x, window_height + extraBounds.y};

    for (vector<Object>::iterator it = world.begin(); it != world.end(); advance(it, 1)){
        Object obj = *it;
        bool horizontal_check = obj.position.x < minBounds.x || obj.position.x > maxBounds.x;
        bool vertical_check = obj.position.y < minBounds.y || obj.position.y > maxBounds.y;

        if (horizontal_check || vertical_check 
            || isnan(it->position.x) || isnan(it->position.y)){
            world.erase(it);
            //Debug_Print("Deleted Planet");
            advance(it, -1); 
        }

        if (it->mass >= Simulator::maxMass*0.75f && it->radius >= Simulator::maxRadius*0.75f){
            it->radius = 15.f;
            it->mass = Simulator::maxMass;
            it->invincible = true;
            it->clr = BLACK;
        }
    }

    //Collision Detection
    //NOTE: There is a bug that causes radius collision not to change despite increased radius. Idk why, and I can't be bothered to fix it
    for (size_t i = 0; i < world.size(); i++){
        Object *obj1 = &world[i];
        for (size_t j = i+1; j < world.size(); j++){

            Object *obj2 = &world[j];
            float dist = Vector2Distance(obj1->position, obj2->position);
            if (dist <= obj1->radius || dist <= obj2->radius){ //They've collided, therefore...
                if (obj1->invincible){
                    world.erase(next(world.begin(), j));
                    j--;
                    continue;
                }else if (obj2->invincible){
                    world.erase(next(world.begin(), i));
                    i--;
                    break;
                }else{
                    Vector2 momentumObj1 = obj1->velocity * obj1->mass;
                    Vector2 momentumObj2 = obj2->velocity * obj2->mass;
                    //Debug_Print("Collision Detected");
                    if (obj1->mass > obj2->mass){ //Conservation of Momentum
                        //Debug_Print("Object 1 with ", obj1->mass, "kg wins");
                        obj1->mass += obj2->mass;
                        obj1->mass = fminf(obj1->mass, Simulator::maxMass);
                        obj1->velocity = (momentumObj1 + momentumObj2) / obj1->mass;
                        obj1->radius *= (1.f + (0.5f * (obj2->mass / obj1->mass)));
                        obj1->radius = fminf(obj1->radius, Simulator::maxRadius);
                        obj1->clr.r = (obj1->clr.r + obj2->clr.r) / 2;
                        obj1->clr.g = (obj1->clr.g + obj2->clr.g) / 2;
                        obj1->clr.b = (obj1->clr.b + obj2->clr.b) / 2;

                        world.erase(next(world.begin(), j));
                        j--;
                    }else{
                        //Debug_Print("Object 2 with ", obj2->mass, "kg wins");
                        obj2->mass += obj1->mass;
                        obj2->mass = fminf(obj2->mass, Simulator::maxMass);
                        obj2->velocity = (momentumObj1 + momentumObj2) / obj2->mass;
                        obj2->radius *= (1.f + (0.5f * (obj1->mass / obj2->mass)));
                        obj2->radius = fminf(obj2->radius, Simulator::maxRadius);
                        obj2->clr.r = (obj1->clr.r + obj2->clr.r) / 2;
                        obj2->clr.g = (obj1->clr.g + obj2->clr.g) / 2;
                        obj2->clr.b = (obj1->clr.b + obj2->clr.b) / 2;

                        world.erase(next(world.begin(), i));
                        i--;
                        break;
                    }
                }
            }
        }
    }
}

void Simulator::Update(float frameTime){
    if (timeConstant <= 0.001f) return;
    predictionUpdate = true;

    //Debug_Print("Simulator Update Called"); Debug_Scope ds;

    WorldTickRoutine(frameTime, timeConstant, gConstant, world);
    WorldCollisionUpdate(frameTime, world);
}

void Simulator::DrawPredictions(int centerIndex) {
    if (timeConstant > 0.001f || world.size() <= 1) return;

    const int PREDICT_ITER_LIM = 10000;
    const float TIME_INTERVAL = 1.f/target_fps;

    if (predictionUpdate){
        predictionUpdate = false;
        predictions.clear();

        vector<Object> predictWorld = world;
        int timeComplexity = (int)world.size() * log10f((float)world.size());
        timeComplexity = timeComplexity == 0 ? 1 : timeComplexity;
        int totalPredictions = PREDICT_ITER_LIM / timeComplexity;
        
        for (int i = 0; i < totalPredictions; i++){
            WorldTickRoutine(TIME_INTERVAL, 1.f, gConstant, predictWorld);
            WorldCollisionUpdate(TIME_INTERVAL, predictWorld);
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
        if (obj.clr.a < 150 || (obj.clr.b < 10 && obj.clr.g < 10 && obj.clr.r < 10)){
            DrawCircleLines(obj.position.x, obj.position.y, obj.radius+0.1f, WHITE);
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

Object *Simulator::SelectObject(int x, int y){
    for (size_t i = 0; i < world.size(); i++){
        Vector2 mousePos = Vector2{(float)x, (float)y};
        float dist = Vector2Distance(mousePos, world[i].position);
        if (dist < (world[i].radius + 1.f)) {
            return &world[i];
        }
    }

    return nullptr;
}

void Simulator::ChaosWorld() {
    predictionUpdate = true;
    world.clear();

    Vector2 center = Vector2{window_length/2, window_height/2};
    Object sun = Object{
        .position = center,
        .velocity = Vector2{0, 0},
        .mass = 10000.f,
        .radius = 20.f,
        .anchored = true,
        .invincible = true,
        .clr = YELLOW
    };
    world.push_back(sun);

    float maxSpeed = 30.f;
    float minSpeed = 11.f;
    float precision = 100.f;
    int square = 20;

    for (int i = 0; i < square*square; i++) {
        float x = (float)(i % (square));
        float y = (float)(i / (square));

        Vector2 pos = Vector2{(x/square) * window_length-10, (y/square) * window_height-10};

        float dist = Vector2Distance(pos, center);
        if (dist <= sun.radius*2) continue; //avoid nan
        if (dist > (window_length/2-50)) continue;
        
        float scale = 1 - (dist / (window_length/2));
        Vector2 dir = Vector2Normalize(Vector2Subtract(pos, center));
        float t = dir.x;
        dir.x = -1.f * dir.y;
        dir.y = t; //rotate counter clockwise
        dir = Vector2Scale(dir, (scale * (maxSpeed-minSpeed) + minSpeed));

        Object ast = Object{
            .position = pos,
            .velocity = dir,
            .mass = 10.f,
            .radius = 3.f,
            .clr = GRAY
        };
        ast.position += Vector2{5*(((rand() % (int)precision) / precision)-0.5f), 5*(((rand() % (int)precision) / precision)-0.5f)};
        ast.velocity += Vector2{minSpeed*(((rand() % (int)precision) / precision)-0.5f), minSpeed*(((rand() % (int)precision) / precision)-0.5f)};
        ast.mass += ((rand() % (int)precision) / precision) - 0.5f;
        ast.radius += ((rand() % (int)precision) / precision) - 0.5f;
        ast.clr.r += (((rand() % (int)precision) / precision) * 100) - 50;
        ast.clr.g += (((rand() % (int)precision) / precision) * 100) - 50;  
        ast.clr.b += (((rand() % (int)precision) / precision) * 50) - 25;

        world.push_back(ast);
    }
}

void Simulator::ResetWorld(){
    predictionUpdate = true;
    
    world.clear();
    Object sun = Object{
        .position = Vector2{window_length/2, window_height/2},
        .velocity = Vector2{0, 0},
        .mass = 10000.f,
        .radius = 20.f,
        .anchored = true,
        .invincible = true,
        .clr = YELLOW
    };
    Object planet = Object{
        .position = Vector2{window_length/2 + 150, window_height/2},
        .velocity = Vector2{0, -18.5f},
        .mass = 1000.f,
        .radius = 5.f,
        .anchored = false,
        .invincible = false,
        .clr = MAROON
    };
    Object moon = Object{
        .position = Vector2{planet.position.x + 15, planet.position.y},
        .velocity = Vector2{0, 2.f},
        .mass = 10.f,
        .radius = 3.f,
        .anchored = false,
        .invincible = false,
        .clr = GRAY
    };
    world.push_back(sun);
    world.push_back(planet);
    world.push_back(moon);
}