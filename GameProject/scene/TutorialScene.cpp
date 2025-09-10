#include "TutorialScene.h"
#include "ModelManager.h"
#include "Object3dBasic.h"
#include "SpriteBasic.h"
#include "Input.h"
#include "DebugCamera.h"
#include "Draw2D.h"
#include "GlobalVariables.h"
#include "FrameTimer.h"
#include "GPUParticle.h"
#include "SceneManager.h"
#include "EmitterManager.h"
#include "Object3d.h"
#include "Model.h"
#include "ShadowRenderer.h"
#include "CollisionManager.h"
#include "../Collision/CollisionTypeIdDef.h"
#include <cstdlib>

#include <numbers>

#ifdef _DEBUG
#include"ImGui.h"
#include "DebugCamera.h"
#endif
#include <ctime>

void TutorialScene::Initialize()
{
    CollisionManager* collisionManager = CollisionManager::GetInstance();
    #ifdef _DEBUG
    DebugCamera::GetInstance()->Initialize();
    Object3dBasic::GetInstance()->SetDebug(false);
    Draw2D::GetInstance()->SetDebug(false);
    GPUParticle::GetInstance()->SetIsDebug(false);
    // デバッグビルドではコライダー表示をデフォルトでON
    collisionManager->SetDebugDrawEnabled(true);
    #endif

    // CollisionManagerの初期化（最初に行う）
    collisionManager->Initialize();

    /// ================================== ///
    ///              初期化処理              ///
    /// ================================== ///
    this->InitializeGameScene();
    tutorial_ = std::make_unique<Tutorial>();
    tutorial_->Initialize();
}

void TutorialScene::Finalize()
{
    this->FinalizeGameScene();

    // CollisionManagerのリセット
    CollisionManager::GetInstance()->Reset();
}

void TutorialScene::Update()
{
    #ifdef _DEBUG
    if (Input::GetInstance()->TriggerKey(DIK_F1))
    {
        Object3dBasic::GetInstance()->SetDebug(!Object3dBasic::GetInstance()->GetDebug());
        Draw2D::GetInstance()->SetDebug(!Draw2D::GetInstance()->GetDebug());
        GPUParticle::GetInstance()->SetIsDebug(!GPUParticle::GetInstance()->GetIsDebug());
        isDebug_ = !isDebug_;
    }

    if (isDebug_)
    {
        DebugCamera::GetInstance()->Update();
    }
    #endif

    /// ================================== ///
    ///              更新処理               ///
    /// ================================== ///
    this->UpdateGameScene();
    tutorial_->Update();

    // シーン遷移
    if (Input::GetInstance()->TriggerKey(DIK_RETURN))
    {
        SceneManager::GetInstance()->ChangeScene("title");
    }
}

void TutorialScene::Draw()
{
    /// ================================== ///
    ///              描画処理               ///
    /// ================================== ///

    skyBox_->Draw();

    if (ShadowRenderer::GetInstance()->IsEnabled())
    {
        ShadowRenderer::GetInstance()->BeginShadowPass();
        player_->Draw();
        boss_->Draw();
        ShadowRenderer::GetInstance()->EndShadowPass();
    }

    //------------------背景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();



    //-------------------Modelの描画-------------------//
    // 3Dモデル共通描画設定
    Object3dBasic::GetInstance()->SetCommonRenderSetting();

    //ground_->Draw();
    player_->Draw();
    boss_->Draw();
    terrain_->Draw();

    //------------------前景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();
    tutorial_->Draw2d();


    #ifdef _DEBUG
    // コライダーのデバッグ描画
    CollisionManager::GetInstance()->DrawColliders();
    #endif

}

void TutorialScene::DrawWithoutEffect()
{
    /// ================================== ///
    ///              描画処理               ///
    /// ================================== ///

    //------------------背景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();



    //-------------------Modelの描画-------------------//
    // 3Dモデル共通描画設定
    Object3dBasic::GetInstance()->SetCommonRenderSetting();



    //------------------前景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();

}

void TutorialScene::DrawImGui()
{
    #ifdef _DEBUG
    player_->DrawImGui();
    boss_->DrawImGui();
    followCamera_->DrawImGui();
    terrain_->ImGui();
    tutorial_->ImGui();
    ShadowRenderer::GetInstance()->DrawImGui();
    CollisionManager::GetInstance()->DrawImGui();
    #endif // DEBUG
}

void TutorialScene::InitializeGameScene()
{
    pInput_ = Input::GetInstance();

    // SkyBoxの初期化
    skyBox_ = std::make_unique<SkyBox>();
    skyBox_->Initialize("my_skybox.dds");



    // 敵モデルの初期化
    boss_ = std::make_unique<Boss>();
    boss_->Initialize();

    // Playerの初期化
    player_ = std::make_unique<Player>();
    player_->Initialize();
    player_->SetCamera((*Object3dBasic::GetInstance()->GetCamera()));

    followCamera_ = std::make_unique<FollowCamera>();
    followCamera_->Initialize((*Object3dBasic::GetInstance()->GetCamera()));
    followCamera_->SetTarget(&player_->GetTransform());
    followCamera_->SetTarget2(&boss_->GetTransform());

    terrain_ = std::make_unique<Terrain>();
    terrain_->Initialize();

    player_->SetTerrain(terrain_.get());

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    ShadowRenderer::GetInstance()->SetMaxShadowDistance(50.0f);
}

void TutorialScene::UpdateGameScene()
{
    player_->SetMode(followCamera_->GetMode());

    skyBox_->Update();
    player_->Update();
    boss_->Update();
    followCamera_->Update();

    terrain_->Update();

    // 衝突判定の実行
    CollisionManager::GetInstance()->CheckAllCollisions();
}

void TutorialScene::FinalizeGameScene()
{
    // オブジェクトの終了処理
    if (player_)
    {
        player_->Finalize();
    }
    if (boss_)
    {
        boss_->Finalize();
    }
}