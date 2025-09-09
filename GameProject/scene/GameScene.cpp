#include "GameScene.h"
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

void GameScene::Initialize()
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
    /// ================================== ///
    ///              初期化処理              ///
    /// ================================== ///
    pInput_ = Input::GetInstance();

    // SkyBoxの初期化
    skyBox_ = std::make_unique<SkyBox>();
    skyBox_->Initialize("my_skybox.dds");

    // CollisionManagerの初期化（最初に行う）
    collisionManager->Initialize();

    // 床モデルのUV変換設定
    groundUvTransform_.translate = Vector3(0.0f, 0.0f, 0.0f);
    groundUvTransform_.rotate = Vector3(0.0f, 0.0f, 0.0f);
    groundUvTransform_.scale = Vector3(100.0f, 100.0f, 100.0f);
    // 床モデルの初期化
    ground_ = std::make_unique<Object3d>();
    ground_->Initialize();
    ground_->SetModel("ground_black.gltf");
    ground_->SetUvTransform(groundUvTransform_);

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

    // 衝突マスクの設定（どのタイプ同士が衝突判定を行うか）
    //collisionManager->SetCollisionMask(
    //  static_cast<uint32_t>(CollisionTypeId::kPlayer),
    //  static_cast<uint32_t>(CollisionTypeId::kEnemyAttack),
    //  true
    //);

    cellFilter_ = std::make_unique<CellBasedFiltering>();
    cellFilter_->Initialize(
        static_cast<int>(Block::kScale * 4),
        static_cast<int>(Terrain::kSize * Block::kScale),
        static_cast<int>(Terrain::kSize * Block::kScale)
    );

    terrain_ = std::make_unique<Terrain>();
    terrain_->Initialize(cellFilter_.get());

    player_->SetTerrain(terrain_.get());

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    ShadowRenderer::GetInstance()->SetMaxShadowDistance(200.f);
}

void GameScene::Finalize()
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

    // CollisionManagerのリセット
    CollisionManager::GetInstance()->Reset();
}

void GameScene::Update()
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
    cellFilter_->UnregisterAll(CollisionManager::GetInstance());


    player_->SetMode(followCamera_->GetMode());

    skyBox_->Update();
    ground_->Update();
    player_->Update();
    boss_->Update();
    followCamera_->Update();

    terrain_->Update();

    cellFilter_->RegisterPotentials(player_->GetBodyCollider());
    cellFilter_->RegisterAll(CollisionManager::GetInstance());

    // 衝突判定の実行
    CollisionManager::GetInstance()->CheckAllCollisions();

    // シーン遷移
    if (Input::GetInstance()->TriggerKey(DIK_RETURN))
    {
        SceneManager::GetInstance()->ChangeScene("title");
    }
}

void GameScene::Draw()
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

    cellFilter_->Draw2d();

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
    player_->InstancedDraw();

    //------------------前景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();


    #ifdef _DEBUG
    // コライダーのデバッグ描画
    CollisionManager::GetInstance()->DrawColliders();
    #endif

}

void GameScene::DrawWithoutEffect()
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

void GameScene::DrawImGui()
{
    #ifdef _DEBUG
    player_->DrawImGui();
    boss_->DrawImGui();
    followCamera_->DrawImGui();
    terrain_->ImGui();
    cellFilter_->DrawImGui();
    ShadowRenderer::GetInstance()->DrawImGui();
    CollisionManager::GetInstance()->DrawImGui();
    #endif // DEBUG
}
