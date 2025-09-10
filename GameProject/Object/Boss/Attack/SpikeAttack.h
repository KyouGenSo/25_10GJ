#pragma once
#include "BossAttack.h"
#include <vector>
#include <memory>
#include "Vector3.h"

class Object3d;
class BossAttackCollider;

class SpikeAttack : public BossAttack
{
public:
    SpikeAttack();
    ~SpikeAttack() override;

    void Initialize(Player* player, Terrain* terrain) override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
    void DrawImGui() override;
    
    void Execute() override;
    bool IsFinished() const override;
    void Reset() override;

private:
    struct Spike
    {
        std::unique_ptr<Object3d> model;           // 棘のモデル
        std::unique_ptr<Object3d> warningMark;     // 警告マーク
        std::unique_ptr<BossAttackCollider> collider;    // 当たり判定
        Transform transform;                        // 位置情報
        Vector3 targetPosition;                     // ターゲット位置
        float animationTimer = 0.0f;               // アニメーションタイマー
        bool isActive = false;                     // アクティブフラグ
    };

    void UpdateWarningState();
    void UpdateAttackState();
    void UpdateCooldownState();
    Vector3 GetPlayerBlockPosition();
    void ActivateSpike(const Vector3& position);

private:
    std::unique_ptr<Spike> spike_;                 // 棘オブジェクト
    
    const float kWarningDuration = 2.0f;           // 警告表示時間
    const float kAttackDuration = 1.0f;            // 攻撃継続時間
    const float kCooldownDuration = 0.5f;          // クールダウン時間
    const float kSpikeDamage = 20.0f;              // 棘のダメージ
    const float kSpikeHeight = 10.0f;              // 棘の高さ
    const float kSpikeAnimationSpeed = 3.0f;       // 棘のアニメーション速度
};