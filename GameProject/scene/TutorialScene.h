#pragma once

#include <BaseScene.h>
#include <memory>
#include <Tutorial/TutorialSection.h>
#include "SkyBox.h"
#include "FollowCamera/followCamera.h"
#include "Object/Player/Player.h"
#include "Object/Boss/Boss.h"
#include <Terrain/Terrain.h>

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

    std::unique_ptr<TutorialSection> section_;
    Input* pInput_ = nullptr;

    // SkyBox
    std::unique_ptr<SkyBox> skyBox_;

    std::unique_ptr<Player> player_;
    std::unique_ptr<Boss> boss_;

    std::unique_ptr<FollowCamera> followCamera_;
    std::unique_ptr<Terrain> terrain_;

    bool isDebug_ = false;
};