#pragma once
#include <memory>
#include "Transform.h"

class Player;
class Terrain;

class BossAttack
{
public:
    enum class AttackState
    {
        Idle,        // 待機状態
        Warning,     // 警告表示中
        Attack,      // 攻撃中
        Cooldown     // クールダウン中
    };

public:
    BossAttack() = default;
    virtual ~BossAttack() = default;

    virtual void Initialize(Player* player, Terrain* terrain) = 0;
    virtual void Finalize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void DrawImGui() {}  // デフォルトは何もしない
    
    virtual void Execute() = 0;
    virtual bool IsFinished() const = 0;
    virtual void Reset() = 0;

    AttackState GetState() const { return state_; }
    bool IsActive() const { return state_ != AttackState::Idle; }
    void SetActive(bool active) { isActive_ = active; }

protected:
    Player* player_ = nullptr;
    Terrain* terrain_ = nullptr;
    AttackState state_ = AttackState::Idle;
    float stateTimer_ = 0.0f;
    bool isActive_ = false;
};