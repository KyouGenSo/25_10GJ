#pragma once
#include <memory>
#include <string>
#include "Transform.h"
#include "Vector4.h"
#include "EmitterManager.h"

class Boss;
class Object3d;
class EnergyCoreCollider;
class Bar3d;
class Camera;

class EnergyCore
{
public:
    EnergyCore();
    ~EnergyCore();

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize(Boss* boss, Camera* camera);

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
    /// 2D描画（HPバー）
    /// </summary>
    void Draw2d();

    /// <summary>
    /// ImGuiの描画
    /// </summary>
    void DrawImGui();

    /// <summary>
    /// ダメージ処理
    /// </summary>
    /// <param name="damage">ダメージ量</param>
    void Damage(float damage);

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

    /// <summary>
    /// ダメージフラッシュ開始
    /// </summary>
    /// <param name="duration">フラッシュ時間（秒）</param>
    void StartDamageFlash(float duration = 0.05f);

    /// <summary>
    /// ダメージシェーク開始
    /// </summary>
    /// <param name="duration">シェーク時間（秒）</param>
    void StartDamageShake(float duration = 0.3f);

    //-----------------------------Getters/Setters------------------------------//
    void SetTransform(const Transform& transform) { transform_ = transform; }
    void SetIsDestroyed(bool destroyed) { isDestroyed_ = destroyed; }
    void SetFlashDuration(float duration) { flashDuration_ = duration; }

    const Transform& GetTransform() const { return transform_; }
    Transform* GetTransformPtr() { return &transform_; }
    bool GetIsDestroyed() const { return isDestroyed_; }
    float GetHp() const { return hp_; }
    float GetMaxHp() const { return maxHp_; }
    float GetFlashDuration() const { return flashDuration_; }

private:
    /// <summary>
    /// パーティクルの初期化
    /// </summary>
    void InitParticleEmitter();

    /// <summary>
    /// 出現アニメーション更新
    /// </summary>
    void UpdateSpawnAnimation();

    /// <summary>
    /// ダメージフラッシュ更新
    /// </summary>
    void UpdateDamageFlash();

    /// <summary>
    /// ダメージシェーク更新
    /// </summary>
    void UpdateDamageShake();

private:
    std::unique_ptr<Object3d> model_;           ///< モデル
    Transform transform_{};                     ///< 変形情報
    Boss* pBoss_ = nullptr;                     ///< ボスへの参照
    bool isDestroyed_ = false;                  ///< 破壊状態
    
    // HP関連
    float maxHp_ = 100.0f;                      ///< 最大HP
    float hp_ = 100.0f;                         ///< 現在のHP

    // Collider
    std::unique_ptr<EnergyCoreCollider> collider_;

    // 出現アニメーション関連
    bool isSpawning_ = false;                   ///< 出現中フラグ
    float spawnTimer_ = 0.0f;                   ///< 出現タイマー
    float spawnDuration_ = 2.0f;                ///< 出現時間
    Vector3 spawnStartPos_{};                   ///< 開始位置
    Vector3 spawnTargetPos_{};                  ///< 目標位置

    // シェーク関連
    float shakeTimer_ = 0.0f;                              ///< シェークタイマー
    Vector3 shakeAmplitude_{ 0.5f, 0.2f, 0.5f }; ///< 振動幅（X,Y,Z）
    float shakeDuration_ = 2.0f;                          ///< シェーク時間
    
    // ダメージフラッシュ関連
    bool isFlashing_ = false;                                      ///< フラッシュ中フラグ
    float flashTimer_ = 0.0f;                                      ///< フラッシュタイマー
    float flashDuration_ = 0.1f;                                   ///< フラッシュ時間（秒）
    Vector4 originalColor_{ 1.0f, 1.0f, 1.0f, 1.0f }; ///< 元の色
    Vector4 flashColor_{ 1.0f, 1.0f, 1.0f, 1.0f };    ///< フラッシュ色（白）
    
    // ダメージシェーク関連
    bool isDamageShaking_ = false;                                 ///< ダメージシェーク中フラグ
    float damageShakeTimer_ = 0.0f;                                ///< ダメージシェークタイマー
    float damageShakeDuration_ = 0.3f;                             ///< ダメージシェーク時間
    Vector3 damageShakeAmplitude_{ 0.3f, 0.1f, 0.3f };    ///< ダメージシェーク振動幅
    Vector3 damageShakeOffset_{};                                  ///< ダメージシェークオフセット
    
    // パーティクルエフェクト関連
    std::unique_ptr<EmitterManager> emitterManager_;       ///< エミッターマネージャー
    std::string emitterName_;                              ///< このコアのエミッター名

    // HPバー関連
    std::unique_ptr<Bar3d> hpBar_;                         ///< HPバー
    Camera* pCamera_ = nullptr;                            ///< カメラへの参照
};