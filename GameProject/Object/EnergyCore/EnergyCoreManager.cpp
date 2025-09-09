#include "EnergyCoreManager.h"
#include "EnergyCore.h"
#include "../Boss/Boss.h"
#include "../../Terrain/Terrain.h"
#include "../../Terrain/Block/Block.h"
#include <string>

#ifdef _DEBUG
#include "ImGui.h"
#endif

EnergyCoreManager::EnergyCoreManager()
{
}

EnergyCoreManager::~EnergyCoreManager()
{
}

void EnergyCoreManager::Initialize(Boss* boss, Terrain* terrain)
{
    pBoss_ = boss;
    pTerrain_ = terrain;
    
    // 乱数生成器の初期化（現在時刻をシードに使用）
    std::random_device rd;
    randomEngine_ = std::mt19937(rd());
    
    // エネルギーコアの初期化（4つ生成）
    energyCores_.resize(kCoreCount);
    for (int i = 0; i < kCoreCount; i++)
    {
        energyCores_[i] = std::make_unique<EnergyCore>();
        energyCores_[i]->Initialize(pBoss_);
    }
    
    // Terrain上にランダム配置
    PlaceRandomly();
}

void EnergyCoreManager::Finalize()
{
    // 各エネルギーコアの終了処理
    for (auto& core : energyCores_)
    {
        if (core)
        {
            core->Finalize();
        }
    }
    energyCores_.clear();
}

void EnergyCoreManager::Update()
{
    // エネルギーコアの更新
    for (auto& core : energyCores_)
    {
        core->Update();
    }
    
    // 全て破壊されたかチェック
    if (CheckAllDestroyed())
    {
        RespawnAll();
    }
}

void EnergyCoreManager::Draw()
{
    // エネルギーコアの描画
    for (auto& core : energyCores_)
    {
        core->Draw();
    }
}

void EnergyCoreManager::DrawImGui()
{
#ifdef _DEBUG
    ImGui::Begin("Energy Cores");
    
    // Destroy Allボタン
    if (ImGui::Button("Destroy All"))
    {
        for (auto& core : energyCores_)
        {
            core->Destroy();
        }
    }
    
    ImGui::Separator();
    
    // 各エネルギーコアの詳細
    for (size_t i = 0; i < energyCores_.size(); i++)
    {
        ImGui::PushID(static_cast<int>(i));
        if (ImGui::TreeNode(("Energy Core " + std::to_string(i)).c_str()))
        {
            energyCores_[i]->DrawImGui();
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
    ImGui::End();
#endif
}

EnergyCore* EnergyCoreManager::GetCore(size_t index)
{
    if (index < energyCores_.size())
    {
        return energyCores_[index].get();
    }
    return nullptr;
}

void EnergyCoreManager::PlaceRandomly()
{
    // 各エリアの範囲を定義
    struct Area {
        int xMin, xMax, zMin, zMax;
    };
    
    Area areas[kCoreCount] = {
        {0, 9, 0, 9},      // エリア1: 左上
        {10, 19, 0, 9},    // エリア2: 右上
        {0, 9, 10, 19},    // エリア3: 左下
        {10, 19, 10, 19}   // エリア4: 右下
    };
    
    // 各エリアでランダムにブロックを選んでエネルギーコアを配置
    for (int i = 0; i < kCoreCount; i++)
    {
        // エリア内でランダムな座標を選択（uniform_int_distribution使用）
        std::uniform_int_distribution<int> distX(areas[i].xMin, areas[i].xMax);
        std::uniform_int_distribution<int> distZ(areas[i].zMin, areas[i].zMax);
        
        int xIndex = distX(randomEngine_);
        int zIndex = distZ(randomEngine_);
        
        // ワールド座標に変換
        float worldX = xIndex * Block::kScale;
        float worldZ = zIndex * Block::kScale;
        
        // その位置の最上層のY座標を取得
        float worldY = pTerrain_->GetMaxYAt(worldX, worldZ);
        
        // エネルギーコアの位置を設定
        Vector3 targetPosition = Vector3(worldX, worldY + Block::kScale * 0.5f, worldZ);
        
        Transform coreTransform;
        coreTransform.translate = targetPosition;  // 一時的な位置（StartSpawnAnimationで上書きされる）
        coreTransform.rotate = Vector3(0.0f, 0.0f, 0.0f);
        coreTransform.scale = Vector3(1.5f, 1.5f, 1.5f);
        
        energyCores_[i]->SetTransform(coreTransform);
        
        // 出現アニメーションを開始
        energyCores_[i]->StartSpawnAnimation(targetPosition);
    }
}

bool EnergyCoreManager::CheckAllDestroyed() const
{
    for (const auto& core : energyCores_)
    {
        if (!core->GetIsDestroyed())
        {
            return false;
        }
    }
    return true;
}

void EnergyCoreManager::RespawnAll()
{
    // 全エネルギーコアを再生成
    for (auto& core : energyCores_)
    {
        core->Respawn();
    }
    
    // 再配置
    PlaceRandomly();
}