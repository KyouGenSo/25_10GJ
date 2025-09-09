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
    ImGui::Begin("Energy Core Manager");
    
    // ===== 統計情報 =====
    int activeCount = 0;
    int destroyedCount = 0;
    float totalHp = 0.0f;
    float totalMaxHp = 0.0f;
    
    for (const auto& core : energyCores_)
    {
        if (core->GetIsDestroyed())
        {
            destroyedCount++;
        }
        else
        {
            activeCount++;
            totalHp += core->GetHp();
        }
        totalMaxHp += core->GetMaxHp();
    }
    
    // 統計情報表示
    ImGui::Text("Status Overview");
    ImGui::Text("Active Cores: %d / %d", activeCount, static_cast<int>(energyCores_.size()));
    ImGui::Text("Destroyed: %d", destroyedCount);
    
    // 総HPバー
    float hpRatio = totalMaxHp > 0 ? totalHp / totalMaxHp : 0.0f;
    ImVec4 hpColor = hpRatio > 0.5f ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) :
                     hpRatio > 0.25f ? ImVec4(0.8f, 0.8f, 0.2f, 1.0f) :
                     ImVec4(0.8f, 0.2f, 0.2f, 1.0f);
    
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, hpColor);
    ImGui::ProgressBar(hpRatio, ImVec2(-1, 0), 
                      (std::string("Total HP: ") + std::to_string(static_cast<int>(totalHp)) + 
                       " / " + std::to_string(static_cast<int>(totalMaxHp))).c_str());
    ImGui::PopStyleColor();
    
    ImGui::Separator();
    
    // ===== 一括操作ボタン =====
    ImGui::Text("Batch Operations");
    
    if (ImGui::Button("Destroy All"))
    {
        for (auto& core : energyCores_)
        {
            core->Destroy();
        }
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Respawn All"))
    {
        RespawnAll();
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Damage All (20)"))
    {
        for (auto& core : energyCores_)
        {
            core->Damage(20.0f);
        }
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Randomize Positions"))
    {
        PlaceRandomly();
    }
    
    ImGui::Separator();
    
    // ===== コアの2x2グリッド表示 =====
    ImGui::Text("Energy Cores Grid");
    
    // グリッドレイアウト（2列）
    if (ImGui::BeginTable("CoreGrid", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit))
    {
        for (size_t i = 0; i < energyCores_.size(); i++)
        {
            ImGui::TableNextColumn();
            
            auto& core = energyCores_[i];
            ImGui::PushID(static_cast<int>(i));
            
            // コアのヘッダー
            bool isDestroyed = core->GetIsDestroyed();
            ImVec4 textColor = isDestroyed ? ImVec4(0.5f, 0.5f, 0.5f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, textColor);
            
            std::string header = "Core " + std::to_string(i);
            if (isDestroyed)
            {
                header += " [DESTROYED]";
            }
            ImGui::Text("%s", header.c_str());
            ImGui::PopStyleColor();
            
            // HPバー
            float hp = core->GetHp();
            float maxHp = core->GetMaxHp();
            float ratio = maxHp > 0 ? hp / maxHp : 0.0f;
            
            ImVec4 barColor = ratio > 0.5f ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) :
                             ratio > 0.25f ? ImVec4(0.8f, 0.8f, 0.2f, 1.0f) :
                             ImVec4(0.8f, 0.2f, 0.2f, 1.0f);
            
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, barColor);
            std::string hpText = std::to_string(static_cast<int>(hp)) + "/" + std::to_string(static_cast<int>(maxHp));
            ImGui::ProgressBar(ratio, ImVec2(150, 0), hpText.c_str());
            ImGui::PopStyleColor();
            
            // クイックアクション
            if (isDestroyed)
            {
                if (ImGui::SmallButton("Respawn"))
                {
                    core->Respawn();
                    // 位置を再設定
                    core->StartSpawnAnimation(core->GetTransform().translate);
                }
            }
            else
            {
                if (ImGui::SmallButton("Destroy"))
                {
                    core->Destroy();
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("Damage"))
                {
                    core->Damage(20.0f);
                }
            }
            
            // 詳細表示
            if (ImGui::TreeNode(("Details##" + std::to_string(i)).c_str()))
            {
                core->DrawImGui();
                ImGui::TreePop();
            }
            
            ImGui::PopID();
        }
        
        ImGui::EndTable();
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
        
        // ワールド座標に変換（ブロックの中心に配置）
        float worldX = xIndex * Block::kScale + Block::kScale * 0.5f;
        float worldZ = zIndex * Block::kScale + Block::kScale * 0.5f;

        // その位置の最上層のY座標を取得
        float worldY = pTerrain_->GetMaxYAt(xIndex * Block::kScale, zIndex * Block::kScale);
        
        // エネルギーコアの位置を設定（ブロックの上に少し浮かせる）
        Vector3 targetPosition = Vector3(worldX, worldY, worldZ);
        
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