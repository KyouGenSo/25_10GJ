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
    
    // 乱数初期化
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
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
        // エリア内でランダムな座標を選択
        int xIndex = areas[i].xMin + std::rand() % (areas[i].xMax - areas[i].xMin + 1);
        int zIndex = areas[i].zMin + std::rand() % (areas[i].zMax - areas[i].zMin + 1);
        
        // ワールド座標に変換
        float worldX = xIndex * Block::kScale;
        float worldZ = zIndex * Block::kScale;
        
        // その位置の最上層のY座標を取得
        float worldY = pTerrain_->GetMaxYAt(worldX, worldZ);
        
        // エネルギーコアの位置を設定（ブロックの上に浮かせる）
        Transform coreTransform;
        coreTransform.translate = Vector3(worldX, worldY + Block::kScale * 0.5f, worldZ);
        coreTransform.rotate = Vector3(0.0f, 0.0f, 0.0f);
        coreTransform.scale = Vector3(1.5f, 1.5f, 1.5f);
        
        energyCores_[i]->SetTransform(coreTransform);
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