#pragma once

#include <BaseScene.h>
#include <memory>
#include <Tutorial/Tutorial.h>
#include "SkyBox.h"
#include "FollowCamera/followCamera.h"
#include "Object/Player/Player.h"
#include "Object/Boss/Boss.h"
#include <Terrain/Terrain.h>
#include <Object/EnergyCore/EnergyCoreManager.h>

class TutorialScene : public BaseScene
{
    public:
    TutorialScene() = default;
    ~TutorialScene() = default;

    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
    void DrawImGui() override;
    void DrawWithoutEffect() override;

private:
    void InitializeGameScene();
    void UpdateGameScene();
    void FinalizeGameScene();

    std::unique_ptr<Tutorial> tutorial_;

    /// GameSceneのメンバ変数
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
    std::unique_ptr<Sprite> controlManualSprite_;
    Vector2 controlManualPos{ 0, 204 };
    Vector2 controlManualSize{ 455, 280 };

    bool isDebug_ = false;


};