#pragma once
#include <memory>
#include <vector>

#include "Transform.h"
#include "Vector4.h"

class BossBodyCollider;
class OBBCollider;
class Object3d;
class AABBCollider;

class Boss
{
public:
    Boss();
    ~Boss();

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
    /// ImGuiの描画
    /// </summary>
    void DrawImGui();

    /// <summary>
    /// ダメージ処理
    /// </summary>
    void Damage(float damage);

    /// <summary>
    /// ダメージフラッシュ開始
    /// </summary>
    /// <param name="duration">フラッシュ時間（秒）</param>
    void StartDamageFlash(float duration = 0.1f);

    //-----------------------------Getters/Setters------------------------------//
    void SetTransform(const Transform& transform) { transform_ = transform; }
    void SetHp(float hp) { hp_ = hp; }
    void SetIsCollapse(bool collapse) { isCollapse = collapse; }
    void SetCollapseDuration(float collapseDuration) { collapseDuration_ = collapseDuration; }
    void SetFlashDuration(float duration) { flashDuration_ = duration; }

    const Transform& GetTransform() const { return transform_; }
    Transform* GetTransformPtr() { return &transform_; }
    float GetMaxHp() const { return kMaxHp; }
    float GetHp() const { return hp_; }
    bool GetIsCollapse() const { return isCollapse; }
    float GetCollapseDuration() const { return collapseDuration_; }
    float GetFlashDuration() const { return flashDuration_; }

private: // プライベート関数
    /// <summary>
    /// コライダーの初期化
    /// </summary>
    void InitializeColliders();

    /// <summary>
    /// コライダーの更新
    /// </summary>
    void UpdateColliders();

    /// <summary>
    /// 倒れる処理
    /// </summary>
    void Collapse();

    /// <summary>
    /// ダメージフラッシュ更新
    /// </summary>
    void UpdateDamageFlash();
private:
    std::unique_ptr<Object3d> model_; ///< モデル
    Transform transform_{};           ///< 変形情報

    const float kMaxHp = 200.0f;      ///< 最大体力
    float hp_ = 200.0f;               ///< 体力
    bool isColliderActive = false;    ///< コライダーの有効フラグ
    bool isCollapse = false;          ///< 崩壊フラグ
    float collapseTimer = 0;          ///< 崩壊タイマー
    float collapseDuration_ = 5.0f;   ///< 崩壊時間

    // Collider
    std::vector<std::unique_ptr<BossBodyCollider>> bodyColliders_;
    std::vector<Transform> bodyColliderTransforms_;
    
    // ダメージフラッシュ関連
    bool isFlashing_ = false;                              ///< フラッシュ中フラグ
    float flashTimer_ = 0.0f;                              ///< フラッシュタイマー
    float flashDuration_ = 0.1f;                           ///< フラッシュ時間（秒）
    Vector4 originalColor_{ 1.0f, 1.0f, 1.0f, 1.0f };     ///< 元の色
    Vector4 flashColor_{ 1.0f, 1.0f, 1.0f, 1.0f };        ///< フラッシュ色（白）
};