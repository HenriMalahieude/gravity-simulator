#include <vector>

#ifndef __MAIN_INSTANCE__
#define __MAIN_INSTANCE__

#include "../Objects/Objects.hpp"

struct Dot{
    Color cc = WHITE;
    Vector2 position = Vector2{0, 0};
    float scale = 1.f;
};

class Simulator{
    private:
        std::vector<Object> world = {};
        int maxObjectCount = 100;
        float gConstant = 1.f;

        bool predictionUpdate = true;
        std::vector<Dot> predictions = {};

        Vector2 calculateForce(int base, int other);
    public:
        float timeConstant = 1.f;
        Simulator() {}

        void Update(float frameTime);

        bool AddObject(Object);
        void ClearAll();

        void DrawPredictions(int centerIndex = -1);
        void DrawObjects();
};

#endif