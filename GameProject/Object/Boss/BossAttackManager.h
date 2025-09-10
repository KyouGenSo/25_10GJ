#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

class BossAttack;
class Player;
class Terrain;

class BossAttackManager
{
public:
    enum class AttackType
    {
        Spike,      // 棘攻撃
        Flame,      // 炎攻撃
    };

public:
    BossAttackManager();
    ~BossAttackManager();

    void Initialize(Player* player, Terrain* terrain);
    void Finalize();
    void Update();
    void Draw();
    void DrawImGui();

    void ExecuteAttack(AttackType type);
    void ExecuteRandomAttack();
    bool IsAttacking() const;
    void StopAllAttacks();

    void SetAttackInterval(float interval) { attackInterval_ = interval; }
    void SetAutoAttack(bool enabled) { autoAttackEnabled_ = enabled; }
    float GetAttackInterval() const { return attackInterval_; }
    bool IsAutoAttackEnabled() const { return autoAttackEnabled_; }

private:
    void RegisterAttack(AttackType type, std::unique_ptr<BossAttack> attack);
    void UpdateAttackTimer();

private:
    std::unordered_map<AttackType, std::unique_ptr<BossAttack>> attacks_;
    BossAttack* currentAttack_ = nullptr;
    
    Player* player_ = nullptr;
    Terrain* terrain_ = nullptr;
    
    float attackTimer_ = 0.0f;
    float attackInterval_ = 5.0f;  // 攻撃間隔（秒）
    bool autoAttackEnabled_ = true;  // 自動攻撃の有効/無効
    
    // デバッグ用
    bool debugMode_ = false;
    AttackType selectedAttackType_ = AttackType::Spike;
};