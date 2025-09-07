#pragma once
#include "BaseScene.h"
#include"Sprite.h"
#include"Object3d.h"
#include <vector>
#include <memory>
#include "AABB.h"
#include <Input.h>
#include <Terrain/Terrain.h>
#include <Animation/TitleAnimation/TitleAnimation.h>

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
    void AnimationsInitialize();
    void AnimationsUpdate();

    bool isDebug_ = false;

    // Sprites
    std::unique_ptr<Sprite> pSpriteToonRaider1_;
    std::unique_ptr<Sprite> pSpriteToonRaider2_;
    std::unique_ptr<Sprite> pSpriteBackground_;

    // Direction resources
    std::unique_ptr<Terrain> terrain_;
    std::unique_ptr<TitleAnimation> pTitleAnimation_;

    Input* input_ = nullptr;
};
