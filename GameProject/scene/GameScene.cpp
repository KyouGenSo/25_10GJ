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

    // Terrainの初期化（エネルギーコア配置の前に必要）
    terrain_ = std::make_unique<Terrain>();
    terrain_->Initialize();

    // エネルギーコアの初期化（4つ生成）
    energyCores_.resize(4);
    for (int i = 0; i < 4; i++)
    {
        energyCores_[i] = std::make_unique<EnergyCore>();
        energyCores_[i]->Initialize(boss_.get());
    }
    
    // Terrainを４等分してエネルギーコアをランダム配置
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // 各エリアの範囲を定義
    struct Area {
        int xMin, xMax, zMin, zMax;
    };
    
    Area areas[4] = {
        {0, 9, 0, 9},      // エリア1: 左上
        {10, 19, 0, 9},    // エリア2: 右上
        {0, 9, 10, 19},    // エリア3: 左下
        {10, 19, 10, 19}   // エリア4: 右下
    };
    
    // 各エリアでランダムにブロックを選んでエネルギーコアを配置
    for (int i = 0; i < 4; i++)
    {
        // エリア内でランダムな座標を選択
        int xIndex = areas[i].xMin + std::rand() % (areas[i].xMax - areas[i].xMin + 1);
        int zIndex = areas[i].zMin + std::rand() % (areas[i].zMax - areas[i].zMin + 1);
        
        // ワールド座標に変換
        float worldX = xIndex * Block::kScale;
        float worldZ = zIndex * Block::kScale;
        
        // その位置の最上層のY座標を取得
        float worldY = terrain_->GetMaxYAt(worldX, worldZ);
        
        // エネルギーコアの位置を設定（ブロックの上に浮かせる）
        Transform coreTransform;
        coreTransform.translate = Vector3(worldX, worldY + Block::kScale * 0.5f + 5.0f, worldZ);
        coreTransform.rotate = Vector3(0.0f, 0.0f, 0.0f);
        coreTransform.scale = Vector3(1.5f, 1.5f, 1.5f);
        
        energyCores_[i]->SetTransform(coreTransform);
    }

    // Playerの初期化
    player_ = std::make_unique<Player>();
    player_->Initialize();
    player_->SetCamera((*Object3dBasic::GetInstance()->GetCamera()));

    followCamera_ = std::make_unique<FollowCamera>();
    followCamera_->Initialize((*Object3dBasic::GetInstance()->GetCamera()));
    followCamera_->SetTarget(&player_->GetTransform());
    followCamera_->SetTarget2(&boss_->GetTransform());

     //衝突マスクの設定（どのタイプ同士が衝突判定を行うか）
    collisionManager->SetCollisionMask(
      static_cast<uint32_t>(CollisionTypeId::kPlayer),
      static_cast<uint32_t>(CollisionTypeId::kBossBody),
      true
    );
    
    // プレイヤーとエネルギーコアの衝突判定を有効化
    collisionManager->SetCollisionMask(
      static_cast<uint32_t>(CollisionTypeId::kPlayer),
      static_cast<uint32_t>(CollisionTypeId::kEnergyCore),
      true
    );

    player_->SetTerrain(terrain_.get());

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
    
    // エネルギーコアの終了処理
    for (auto& core : energyCores_)
    {
        if (core)
        {
            core->Finalize();
        }
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

    player_->SetMode(followCamera_->GetMode());

    skyBox_->Update();
    ground_->Update();
    player_->Update();
    boss_->Update();
    
    // エネルギーコアの更新
    for (auto& core : energyCores_)
    {
        core->Update();
    }
    
    // 全エネルギーコアが破壊されたかチェック
    bool allDestroyed = true;
    for (const auto& core : energyCores_)
    {
        if (!core->GetIsDestroyed())
        {
            allDestroyed = false;
            break;
        }
    }
    
    // 全て破壊されたら再生成
    if (allDestroyed)
    {
        for (auto& core : energyCores_)
        {
            core->Respawn();
        }
    }
    
    followCamera_->Update();

    terrain_->Update();

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
        for (auto& core : energyCores_)
        {
            core->Draw();
        }
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
    
    // エネルギーコアの描画
    for (auto& core : energyCores_)
    {
        core->Draw();
    }
    
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
    
    // エネルギーコアのImGui描画
    ImGui::Begin("Energy Cores");
    for (int i = 0; i < energyCores_.size(); i++)
    {
        ImGui::PushID(i);
        if (ImGui::TreeNode(("Energy Core " + std::to_string(i)).c_str()))
        {
            energyCores_[i]->DrawImGui();
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
    ImGui::End();
    
    followCamera_->DrawImGui();
    terrain_->ImGui();
    ShadowRenderer::GetInstance()->DrawImGui();
    CollisionManager::GetInstance()->DrawImGui();
    #endif // DEBUG
}
