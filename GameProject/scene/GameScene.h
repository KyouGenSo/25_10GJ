#pragma once
#include "BaseScene.h"
#include"Object3d.h"
#include "EmitterManager.h"
#include "Sprite.h"
#include "SkyBox.h"
#include "BoneTracker.h"
#include "CharacterEffectPresets.h"
#include "FollowCamera/followCamera.h"
#include "Object/Player/Player.h"
#include "Object/Boss/Boss.h"
#include <Terrain/Terrain.h>

class GameScene : public BaseScene
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
    Input* pInput_ = nullptr;

    // SkyBox
    std::unique_ptr<SkyBox> skyBox_;
    std::unique_ptr<Object3d> ground_;

    std::unique_ptr<Player> player_;
    std::unique_ptr<Boss> boss_;

    std::unique_ptr<FollowCamera> followCamera_;
    std::unique_ptr<Terrain> terrain_;

    Transform groundUvTransform_{};

    bool isDebug_ = false;

};
