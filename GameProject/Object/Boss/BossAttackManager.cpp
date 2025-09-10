#include "BossAttackManager.h"
#include "Attack/BossAttack.h"
#include "Attack/SpikeAttack.h"
#include "Attack/FlameAttack.h"
#include "FrameTimer.h"
#include <random>

#ifdef _DEBUG
#include "ImGui.h"
#endif

BossAttackManager::BossAttackManager()
{
}

BossAttackManager::~BossAttackManager()
{
}

void BossAttackManager::Initialize(Player* player, Terrain* terrain)
{
    player_ = player;
    terrain_ = terrain;

    autoAttackEnabled_ = true;
    
    // 攻撃パターンを登録
    auto spikeAttack = std::make_unique<SpikeAttack>();
    spikeAttack->Initialize(player, terrain);
    RegisterAttack(AttackType::Spike, std::move(spikeAttack));
    
    auto flameAttack = std::make_unique<FlameAttack>();
    flameAttack->Initialize(player, terrain);
    RegisterAttack(AttackType::Flame, std::move(flameAttack));
    
    // 今後、他の攻撃パターンもここで登録

}

void BossAttackManager::Finalize()
{
    // 全ての攻撃を終了処理
    for (auto& [type, attack] : attacks_)
    {
        if (attack)
        {
            attack->Finalize();
        }
    }
    attacks_.clear();
}

void BossAttackManager::Update()
{
    // 自動攻撃タイマーの更新
    if (autoAttackEnabled_)
    {
        UpdateAttackTimer();
    }
    
    // 現在の攻撃を更新
    if (currentAttack_)
    {
        currentAttack_->Update();
        
        // 攻撃が終了したらリセット
        if (currentAttack_->IsFinished())
        {
            currentAttack_ = nullptr;
        }
    }
    
    // 全ての攻撃を更新（非アクティブなものも状態管理のため）
    for (auto& [type, attack] : attacks_)
    {
        if (attack.get() != currentAttack_ && attack)
        {
            attack->Update();
        }
    }
}

void BossAttackManager::Draw()
{
    // 全ての攻撃を描画
    for (auto& [type, attack] : attacks_)
    {
        if (attack)
        {
            attack->Draw();
        }
    }
}

void BossAttackManager::DrawImGui()
{
#ifdef _DEBUG
    ImGui::Begin("Boss Attack Manager");
    
    ImGui::Text("Attack System Control");
    ImGui::Separator();
    
    // 現在の攻撃状態
    ImGui::Text("Current Attack: %s", currentAttack_ ? "Active" : "None");
    if (currentAttack_)
    {
        const char* stateName[] = { "Idle", "Warning", "Attack", "Cooldown" };
        ImGui::Text("  State: %s", stateName[static_cast<int>(currentAttack_->GetState())]);
    }
    
    ImGui::Separator();
    
    // 自動攻撃設定
    ImGui::Checkbox("Auto Attack", &autoAttackEnabled_);
    if (autoAttackEnabled_)
    {
        ImGui::DragFloat("Attack Interval", &attackInterval_, 0.1f, 0.5f, 10.0f);
        ImGui::ProgressBar(attackTimer_ / attackInterval_, ImVec2(0.0f, 0.0f));
    }
    
    ImGui::Separator();
    
    // 手動攻撃実行
    ImGui::Text("Manual Attack Execution:");
    
    const char* attackTypes[] = { "Spike", "Flame", "Sweep", "Projectile", "Area" };
    int currentType = static_cast<int>(selectedAttackType_);
    if (ImGui::Combo("Attack Type", &currentType, attackTypes, IM_ARRAYSIZE(attackTypes)))
    {
        selectedAttackType_ = static_cast<AttackType>(currentType);
    }
    
    if (ImGui::Button("Execute Selected Attack"))
    {
        ExecuteAttack(selectedAttackType_);
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Execute Random Attack"))
    {
        ExecuteRandomAttack();
    }
    
    if (ImGui::Button("Stop All Attacks"))
    {
        StopAllAttacks();
    }
    
    ImGui::Separator();
    
    // 攻撃パターン情報
    ImGui::Text("Registered Attacks:");
    for (const auto& [type, attack] : attacks_)
    {
        ImGui::BulletText("%s: %s", 
            attackTypes[static_cast<int>(type)],
            attack ? "Loaded" : "Not Loaded");
    }
    
    ImGui::End();
    
    // 各攻撃のImGuiを描画
    for (auto& [type, attack] : attacks_)
    {
        if (attack)
        {
            attack->DrawImGui();
        }
    }
#endif
}

void BossAttackManager::ExecuteAttack(AttackType type)
{
    // 既に攻撃中の場合は実行しない
    if (currentAttack_ && !currentAttack_->IsFinished())
    {
        return;
    }
    
    // 指定された攻撃を取得
    auto it = attacks_.find(type);
    if (it != attacks_.end() && it->second)
    {
        currentAttack_ = it->second.get();
        currentAttack_->Execute();
        attackTimer_ = 0.0f;  // タイマーリセット
    }
}

void BossAttackManager::ExecuteRandomAttack()
{
    // 利用可能な攻撃のリストを作成
    std::vector<AttackType> availableAttacks;
    for (const auto& [type, attack] : attacks_)
    {
        if (attack)
        {
            availableAttacks.push_back(type);
        }
    }
    
    if (availableAttacks.empty()) return;
    
    // ランダムに選択
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, static_cast<int>(availableAttacks.size() - 1));
    
    AttackType randomType = availableAttacks[dis(gen)];
    ExecuteAttack(randomType);
}

bool BossAttackManager::IsAttacking() const
{
    return currentAttack_ != nullptr && !currentAttack_->IsFinished();
}

void BossAttackManager::StopAllAttacks()
{
    // 全ての攻撃をリセット
    for (auto& [type, attack] : attacks_)
    {
        if (attack)
        {
            attack->Reset();
        }
    }
    currentAttack_ = nullptr;
    attackTimer_ = 0.0f;
}

void BossAttackManager::RegisterAttack(AttackType type, std::unique_ptr<BossAttack> attack)
{
    attacks_[type] = std::move(attack);
}

void BossAttackManager::UpdateAttackTimer()
{
    if (!autoAttackEnabled_) return;
    
    // 攻撃中でない場合のみタイマーを更新
    if (!IsAttacking())
    {
        attackTimer_ += FrameTimer::GetInstance()->GetDeltaTime();
        
        // インターバルを超えたらランダム攻撃を実行
        if (attackTimer_ >= attackInterval_)
        {
            ExecuteRandomAttack();
        }
    }
}