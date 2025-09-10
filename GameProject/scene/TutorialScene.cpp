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
    #ifdef _DEBUG
    DebugCamera::GetInstance()->Initialize();
    Object3dBasic::GetInstance()->SetDebug(false);
    Draw2D::GetInstance()->SetDebug(false);
    GPUParticle::GetInstance()->SetIsDebug(false);
    CollisionManager* collisionManager = CollisionManager::GetInstance();
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
        energyCoreManager_->Draw();
        ShadowRenderer::GetInstance()->EndShadowPass();
    }

    cellFilter_->Draw2dDebug();

    //------------------背景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();


    //-------------------Modelの描画-------------------//
    // 3Dモデル共通描画設定
    Object3dBasic::GetInstance()->SetCommonRenderSetting();

    player_->Draw();
    boss_->Draw();

    // エネルギーコアの描画
    energyCoreManager_->Draw();

    terrain_->Draw();
    player_->InstancedDraw();

    //------------------前景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();

    // HPバー描画
    boss_->Draw2d();
    player_->Draw2d();
    energyCoreManager_->Draw2d();
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
    auto collisionManager = CollisionManager::GetInstance();

    pInput_ = Input::GetInstance();
    skyBox_ = std::make_unique<SkyBox>();
    skyBox_->Initialize("my_skybox.dds");
    collisionManager->Initialize();
    boss_ = std::make_unique<Boss>();
    boss_->Initialize((*Object3dBasic::GetInstance()->GetCamera()));
    terrain_ = std::make_unique<Terrain>();
    terrain_->Initialize();
    energyCoreManager_ = std::make_unique<EnergyCoreManager>();
    energyCoreManager_->Initialize(boss_.get(), terrain_.get(), (*Object3dBasic::GetInstance()->GetCamera()));
    player_ = std::make_unique<Player>();
    player_->Initialize();
    player_->SetCamera((*Object3dBasic::GetInstance()->GetCamera()));
    followCamera_ = std::make_unique<FollowCamera>();
    followCamera_->Initialize((*Object3dBasic::GetInstance()->GetCamera()));
    followCamera_->SetTarget(&player_->GetTransform());
    followCamera_->SetTarget2(&boss_->GetTransform());
    collisionManager->SetCollisionMask(
        static_cast<uint32_t>(CollisionTypeId::kAttack),
        static_cast<uint32_t>(CollisionTypeId::kBossBody),
        true
    );
    collisionManager->SetCollisionMask(
        static_cast<uint32_t>(CollisionTypeId::kAttack),
        static_cast<uint32_t>(CollisionTypeId::kEnergyCore),
        true
    );
    collisionManager->SetCollisionMask(
        static_cast<uint32_t>(CollisionTypeId::kPlayer),
        static_cast<uint32_t>(CollisionTypeId::kBossAttack),
        true
    );
    cellFilter_ = std::make_unique<CellBasedFiltering>();
    cellFilter_->Initialize(
        static_cast<int>(Block::kScale * 2),
        static_cast<int>(Terrain::kSize * Block::kScale),
        static_cast<int>(Terrain::kSize * Block::kScale)
    );
    terrain_ = std::make_unique<Terrain>();
    terrain_->Initialize(cellFilter_.get());
    player_->SetTerrain(terrain_.get());
    boss_->SetPlayer(player_.get());
    boss_->SetTerrain(terrain_.get());
    colorManualSprite_ = std::make_unique<Sprite>();
    colorManualSprite_->Initialize("ColorManual.png");
    colorManualSprite_->SetPos(colorManualPos);
    colorManualSprite_->SetSize(colorManualSize);
    colorManualSprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    controlManualSprite_ = std::make_unique<Sprite>();
    controlManualSprite_->Initialize("ControlManual.png");
    controlManualSprite_->SetPos(controlManualPos);
    controlManualSprite_->SetSize(controlManualSize);
    controlManualSprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    ShadowRenderer::GetInstance()->SetMaxShadowDistance(50.f);
}

void TutorialScene::UpdateGameScene()
{
    cellFilter_->UnregisterAll(CollisionManager::GetInstance());
    colorManualSprite_->SetPos(colorManualPos);
    colorManualSprite_->SetSize(colorManualSize);
    colorManualSprite_->Update();
    controlManualSprite_->SetPos(controlManualPos);
    controlManualSprite_->SetSize(controlManualSize);
    controlManualSprite_->Update();
    player_->SetMode(followCamera_->GetMode());
    skyBox_->Update();
    player_->SetDebug(isDebug_);
    player_->Update();
    boss_->Update();
    energyCoreManager_->Update();
    followCamera_->Update();
    terrain_->Update();
    cellFilter_->RegisterPotentials(player_->GetBodyCollider());
    cellFilter_->RegisterAll(CollisionManager::GetInstance());
    CollisionManager::GetInstance()->CheckAllCollisions();
    if (player_->GetHp() <= 0)
    {
        SceneManager::GetInstance()->ChangeScene("over");
    }
    else if (boss_->GetHp() <= 0)
    {
        SceneManager::GetInstance()->ChangeScene("clear");
    }
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

    // エネルギーコアマネージャーの終了処理
    if (energyCoreManager_)
    {
        energyCoreManager_->Finalize();
    }

    // CollisionManagerのリセット
    CollisionManager::GetInstance()->Reset();
}