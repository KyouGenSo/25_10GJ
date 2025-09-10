#pragma once
#include "BossAttack.h"
#include <array>
#include <memory>
#include <string>

#include "vector2.h"
#include "Vector3.h"
#include "Vector4.h"

class EmitterManager;
class BossAttackCollider;

class FlameAttack : public BossAttack
{
public:
    FlameAttack();
    ~FlameAttack() override;

    void Initialize(Player* player, Terrain* terrain) override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
    void DrawImGui();
    
    void Execute() override;
    bool IsFinished() const override;
    void Reset() override;

private:
    struct FlameArea
    {
        Vector3 position;                           // エリアの中心位置
        Vector3 size;                               // エリアサイズ
        float height;                               // 高さ（高い/低い）
        std::unique_ptr<BossAttackCollider> collider;  // 当たり判定
        std::string emitterName;                    // パーティクルエミッター名
        Transform transform;                         // 変形情報
    };

    void InitializeFlameAreas();
    void InitializeFlameParticles();
    void UpdateWarningState();
    void UpdateAttackState();
    void ActivateFlames();
    void DeactivateFlames();

private:
    std::array<FlameArea, 4> flameAreas_;          // 4つのエリア
    std::unique_ptr<EmitterManager> emitterManager_;  // パーティクルマネージャー
    
    // 調整可能なパラメータ
    float warningDuration_ = 4.0f;                 // 警告時間
    float warningFadeTime_ = 1.0f;                 // 警告パーティクルが消えるタイミング（警告終了の何秒前か）
    float flameDuration_ = 0.3f;                   // 炎継続時間
    float flameDamage_ = 30.0f;                    // ダメージ
    float flameHeight_ = 3.0f;                     // 炎の判定高さ
    
    // パーティクル設定（警告時）
    Vector4 warningColor_ = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    Vector2 warningVelX_ = Vector2(-0.01f, 0.01f);
    Vector2 warningVelY_ = Vector2(0.05f, 0.1f);
    Vector2 warningVelZ_ = Vector2(-0.01f, 0.01f);
    Vector2 warningScale_ = Vector2(0.5f, 0.5f);
    Vector2 warningLifeTime_ = Vector2(0.5f, 1.0f);
    
    // パーティクル設定（攻撃時）
    Vector4 flameStartColor_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    Vector4 flameEndColor_ = Vector4(1.0f, 0.6f, 0.0f, 1.0f);
    Vector2 flameVelX_ = Vector2(0.0f, 0.0f);
    Vector2 flameVelY_ = Vector2(3.0f, 3.0f);
    Vector2 flameVelZ_ = Vector2(0.0f, 0.0f);
    Vector2 flameScale_ = Vector2(8.0f, 8.0f);
    Vector2 flameLifeTime_ = Vector2(0.03f, 0.03f);
    
    int particleCount_ = 100;                      // パーティクル数
    float particleFrequency_ = 0.05f;              // 生成頻度
};