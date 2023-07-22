#include <vector>

#ifndef __MAIN_INSTANCE__
#define __MAIN_INSTANCE__

#include "../Objects/Objects.hpp"

struct Dot{
    Color cc = WHITE;
    Vector2 position = Vector2{0, 0};
};

class Simulator{
    private:
        std::vector<Object> world = {};
        int maxObjectCount = 100;
        float gConstant = 5.f;

        bool predictionUpdate = true;
        std::vector<Dot> predictions = {};
    public:
        float timeConstant = 1.f;
        Simulator() {}

        void Update(float frameTime);

        Object SelectObject(int x, int y);
        bool AddObject(Object);
        void ClearAll();

        void DrawPredictions(int centerIndex = -1);
        void DrawObjects();
};

#endif