#pragma once
#include "BaseScene.h"
#include"Sprite.h"
#include"Object3d.h"
#include <vector>
#include <memory>
#include "AABB.h"
#include <Input.h>
#include <Terrain/Terrain.h>

class TitleScene : public BaseScene
{
public: // メンバ関数

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// 更新
    /// </summary>
    void Update() override;

    /// <summary>
    /// 描画
    /// </summary>
    void Draw() override;
    void DrawWithoutEffect() override;

    /// <summary>
    /// ImGuiの描画
    /// </summary>
    void DrawImGui() override;

private: // メンバ変数

    void SpritesInitialize();
    void SpritesUpdate();
    void TerrainUpdate();

    bool isDebug_ = false;

    // Sprites
    std::unique_ptr<Sprite> spriteToon_;
    std::unique_ptr<Sprite> spriteRaider_;

    std::unique_ptr<Terrain> terrain_;

    uint8_t currentTitleSpriteNumber_ = 0;

    Input* input_ = nullptr;
};
