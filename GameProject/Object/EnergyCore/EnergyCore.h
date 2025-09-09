#pragma once
#include <memory>
#include "Transform.h"

class Boss;
class Object3d;
class EnergyCoreCollider;

class EnergyCore
{
public:
    EnergyCore();
    ~EnergyCore();

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize(Boss* boss);

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
    /// ImGuiの描画
    /// </summary>
    void DrawImGui();

    /// <summary>
    /// 破壊処理
    /// </summary>
    void Destroy();

    /// <summary>
    /// 再生成処理
    /// </summary>
    void Respawn();

    /// <summary>
    /// 出現アニメーション開始
    /// </summary>
    /// <param name="targetPosition">目標位置</param>
    void StartSpawnAnimation(const Vector3& targetPosition);

    /// <summary>
    /// シェーク処理
    /// </summary>
    /// <param name="amplitude">振動幅</param>
    /// <param name="duration">シェーク時間</param>
    /// <returns>シェークオフセット</returns>
    Vector3 ApplyShake(const Vector3& amplitude, float duration);

    //-----------------------------Getters/Setters------------------------------//
    void SetTransform(const Transform& transform) { transform_ = transform; }
    void SetIsDestroyed(bool destroyed) { isDestroyed_ = destroyed; }

    const Transform& GetTransform() const { return transform_; }
    Transform* GetTransformPtr() { return &transform_; }
    bool GetIsDestroyed() const { return isDestroyed_; }

private:
    /// <summary>
    /// 出現アニメーション更新
    /// </summary>
    void UpdateSpawnAnimation();

private:
    std::unique_ptr<Object3d> model_;           ///< モデル
    Transform transform_{};                     ///< 変形情報
    Boss* pBoss_ = nullptr;                     ///< ボスへの参照
    bool isDestroyed_ = false;                  ///< 破壊状態

    // Collider
    std::unique_ptr<EnergyCoreCollider> collider_;

    // 出現アニメーション関連
    bool isSpawning_ = false;                   ///< 出現中フラグ
    float spawnTimer_ = 0.0f;                   ///< 出現タイマー
    float spawnDuration_ = 2.0f;                ///< 出現時間
    Vector3 spawnStartPos_{};                   ///< 開始位置
    Vector3 spawnTargetPos_{};                  ///< 目標位置

    // シェーク関連
    float shakeTimer_ = 0.0f;                   ///< シェークタイマー
    Vector3 shakeAmplitude_{ 0.5f, 0.2f, 0.5f }; ///< 振動幅（X,Y,Z）
    float shakeDuration_ = 2.0f;                ///< シェーク時間
};