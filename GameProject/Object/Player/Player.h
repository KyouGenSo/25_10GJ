#pragma once
#include <memory>

#include "EmitterManager.h"
#include "Transform.h"
#include "Collision/Collider/Player/PlayerCollider.hpp"
#include "Object/Dispenser/Dispenser.hpp"
#include "Terrain/Terrain.h"

class Object3d;
class InputHandler;
class Camera;
class AABBCollider;
class Boss;

class Player
{
public:
    const float kSize = 3.0f;
private:
    std::unique_ptr<Object3d> model_; ///< モデル
    Camera* camera_ = nullptr;        ///< カメラ
    Terrain* terrain_ = nullptr;      ///< 床
    Transform transform_{};           ///< 変形情報
    Vector3 velocity_{};              ///< 速度
    float speed_ = 0.2f;              ///< 移動速度
    float targetAngle_ = 0.f;         ///< 目標角度
    float hp_ = 100.f;                ///< 体力
    
    bool mode_ = true;               ///< true: ThirdPersonMode, false: TopDownMode
    
    // システム
    std::unique_ptr<InputHandler> inputHandler_;
    
    // Colliders
    std::unique_ptr<PlayerCollider> bodyCollider_;
    
    // 攻撃関連
    bool isAttacking_ = false;
    std::unique_ptr<Object3d> weapon_;
    float timer_ = 0.f;
    const float kMotionTime = 0.55f;
    std::unique_ptr<AABBCollider> attackCollider_;
    float damage_ = 10.f;
    const float kDamage = 10.f;

    std::unique_ptr<Dispenser> dispenser_;

    bool onGround_ = true;
    uint16_t jumpCount_ = 0;

    std::unique_ptr<EmitterManager> emitter_;
    std::string emitterName_ = "PlayerOnBuffedFloor";

public: // メンバ関数
    Player();
    ~Player();
    
    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();
    
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize();
    
    /// <summary>
    /// 更新
    /// </summary>
    void Update();
    
    /// <summary>
    /// 描画
    /// </summary>
    void Draw();

    /// <summary>
    /// InstancedModel用描画
    /// </summary>
    void InstancedDraw();

    /// <summary>
    /// ImGuiの描画
    /// </summary>
    void DrawImGui();
    
    /// <summary>
    /// 攻撃関連
    /// </summary>
    void SetupColliders();
    
    //-----------------------------Getters/Setters------------------------------//
    void SetSpeed(float speed) { speed_ = speed; }
    void SetCamera(Camera* camera) { camera_ = camera; }
    void SetMode(bool mode) { mode_ = mode; } // true: ThirdPersonMode, false: TopDownMode
    void SetTransform(const Transform& transform) { transform_ = transform; }
    void SetTerrain(Terrain* terrain) {terrain_ = terrain;}
    void SetHp(float hp) { hp_ = hp; if (hp_ < 0.f) hp_ = 0.f; }
    void OnGround();

    float GetSpeed() const { return speed_; }
    Camera* GetCamera() const { return camera_; }
    bool GetMode() const { return mode_; }
    float GetHp() const { return hp_; }
    const Transform& GetTransform() const { return transform_; }
    Object3d* GetModel() const { return model_.get(); }
    InputHandler* GetInputHandler() const { return inputHandler_.get(); }
    Transform* GetTransformPtr() { return &transform_; }
    AABBCollider* GetBodyCollider() const { return bodyCollider_.get(); }

private:
    /// <summary>
    /// 移動
    /// </summary>
    /// <param name="speedMultiplier">速度倍率（デフォルト1.0）</param>
    void Move(float speedMultiplier = 1.0f);

    void Jump(bool _isBuffed);

    void Dash(bool _isBuffed);

    void Attack(bool _isBuffed);

    void Dispense();

    void Action();

    void Apply();
};

