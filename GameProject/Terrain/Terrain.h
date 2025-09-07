#pragma once
#include <vector>

#include "./Block/Block.h"
#include <InstancedObject3d.h>
#include <memory>

class Terrain
{
public:
    static const uint32_t kSize = 20;
    static const uint32_t kHeight = 2;
    static const uint32_t kNumBlocks = kSize * kHeight * kSize;

    Terrain() = default;
    ~Terrain() = default;

    void Initialize();
    void Finalize();
    void Update();
    void Draw();

    static Vector3 ToVector3(uint32_t index);
    static uint32_t ToIndex(const Vector3& position);

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

    // インデックスに対応するブロックの色を変更 (範囲外は例外)
    void SetBlockColorAt(uint32_t index, Block::Colors color);

    // XZ-座標からY座標の最大値を取得。範囲外は例外
    float GetMaxYAt(float x, float z);

private:
    std::unique_ptr<InstancedObject3d> instancedObjectBlock_;
    std::vector <std::unique_ptr<Block>> blocks_;
};