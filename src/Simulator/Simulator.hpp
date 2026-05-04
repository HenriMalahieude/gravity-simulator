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
        Image predictionImage;
        Texture2D predictionTexture;
        bool predictionUpdate = true;
        int nextID = 1;
        //std::vector<Dot> predictions = {};
    public:
        static constexpr float maxRadius = 30.f;
        static constexpr float maxMass = 300000.f;
        static constexpr int maxObjectCount = 1000;
        static constexpr float gConstant = 5.f;
        
        std::vector<Object> world = {};
        float timeConstant = 1.f;
        Simulator() {
            predictionImage = GenImageColor(window_length, window_height, Color{0, 0, 0, 0});
            predictionTexture = LoadTextureFromImage(predictionImage);
        }

        void Update(float frameTime);

        int SelectObject(int x, int y);
        Object ObjectInfo(int id){
            for (size_t i = 0; i < world.size(); i++){
                if (world[i].id == id) return world[i];
            }

            return Object{};
        }
        bool AddObject(Object);
        void ResetWorld();
        void ChaosWorld();
        void ClearAll();

        void DrawPredictions(int centerIndex = -1);
        void DrawObjects();
};

#endif