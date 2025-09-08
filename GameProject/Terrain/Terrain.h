#pragma once
#include <vector>

#include "./Block/Block.h"
#include <InstancedObject3d.h>
#include <memory>

class Terrain
{
public:
    static const uint32_t kWidth = 10;
    static const uint32_t kHeight = 2;
    static const uint32_t kSize = kWidth * 2;
    static const uint32_t kNumBlocks = kSize * kHeight * kSize;

    Terrain() = default;
    ~Terrain() = default;

    void Initialize();
    void Finalize();
    void Update();
    void Draw();
    void ImGui();

    static Vector3 ToLocalVector3(uint32_t index);

    /// Getters

    // 座標に対応するブロックの色を取得 (範囲外は例外)
    Block::Colors GetBlockColorAt(const Vector3& position);

    // positionにあるBlockを取得。範囲外は例外
    Block* GetBlockAt(const Vector3& position);

    // positionにBlockがあるか
    bool HasBlockAt(const Vector3& position);

    /// Setters

    // 座標に対応するブロックの色を変更 (範囲外は例外)
    void SetBlockColorAt(const Vector3& position, Block::Colors color);

    // XZ-座標からY座標の最大値を取得。範囲外は例外
    float GetMaxYAt(float x, float z);

private:
    std::unique_ptr<InstancedObject3d> instancedObjectBlock_;

    template <class type, int sx, int sy, int sz>
    using Array3D = std::array<std::array<std::array<type, sz>, sy>, sx>;

    Array3D<std::unique_ptr<Block>, kSize, kHeight, kSize> blocks_;

    #ifdef _DEBUG
    Vector3 selectPosition_ = { 0.0f, 0.0f, 0.0f };
    #endif // _DEBUG
};