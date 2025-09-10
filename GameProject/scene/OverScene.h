#pragma once
#include "BaseScene.h"
#include "Sprite.h"
#include <memory>

class OverScene : public BaseScene
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

    bool isDebug_ = false;

    std::unique_ptr<Sprite> overSprite_ = nullptr;
    std::unique_ptr<Sprite> buttonSprite_ = nullptr;
    std::unique_ptr<Sprite> bgSprite_ = nullptr;

    Vector2 overPos_ = { 530, 200 };
    Vector2 overSize_ = { 512, 128 };
    Vector2 buttonPos_ = { 530, 600 };
    Vector2 buttonSize_ = { 256, 64 };
    Vector2 bgPos_ = { 0, 0 };

};

