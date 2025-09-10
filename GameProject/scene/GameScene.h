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
#include "Object/EnergyCore/EnergyCoreManager.h"
#include <Terrain/Terrain.h>
#include <Features/CellBasedFiltering/CellBasedFiltering.h>
#include <Tutorial/HelpMenu.h>

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

    std::unique_ptr<Player> player_;
    std::unique_ptr<Boss> boss_;
    std::unique_ptr<EnergyCoreManager> energyCoreManager_;

    std::unique_ptr<FollowCamera> followCamera_;
    std::unique_ptr<Terrain> terrain_;

    std::unique_ptr<CellBasedFiltering> cellFilter_;

    std::unique_ptr<Sprite> colorManualSprite_;
    Vector2 colorManualPos{ 0, 4 };
    Vector2 colorManualSize{ 450, 200 };

    std::unique_ptr<Sprite> howToPlaySprite_;
    Vector2 howToPlayPos{ 0, 210 };

    std::unique_ptr<HelpMenu> helpMenu_;

    uint32_t bgmVH_ = 0;

    bool isDebug_ = false;

};
