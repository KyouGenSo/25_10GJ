#pragma once
#include <vector>

#include "./Block/Block.h"

class Terrain
{
public:
    Terrain() = default;
    ~Terrain() = default;

    void Initialize();
    void Finalize();
    void Update();
    void Draw();

    static Vector3 ToVector3(uint32_t index);

private:
    std::vector <std::unique_ptr<Block>> blocks_;
    static const uint32_t kSize = 20;
    static const uint32_t kHeight = 2;
    static const uint32_t kNumBlocks = kSize * kHeight * kSize;
};