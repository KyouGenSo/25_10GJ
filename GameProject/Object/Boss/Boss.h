#pragma once
#include <memory>

#include "Transform.h"

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
  void OnHit(float damage);

  //-----------------------------Getters/Setters------------------------------//
  void SetTransform(const Transform& transform) { transform_ = transform; }
  void SetHp(float hp) { hp_ = hp; }
  void SetPhase(uint32_t phase) { phase_ = phase; }

  const Transform& GetTransform() const { return transform_; }
  Transform* GetTransformPtr() { return &transform_; }
  float GetHp() const { return hp_; }
  uint32_t GetPhase() const { return phase_; }
  uint32_t GetID() const { return id_; }
  AABBCollider* GetCollider() const { return bodyCollider_.get(); }

private:
  std::unique_ptr<Object3d> model_; ///< モデル
  Transform transform_{};           ///< 変形情報

  float hp_ = 200.0f;               ///< 体力
  uint32_t phase_ = 1;              ///< フェーズ
  uint32_t maxPhase_ = 5;           ///< 最大フェーズ
  
  // Collider
  std::unique_ptr<AABBCollider> bodyCollider_;
  
  // ID管理
  static uint32_t nextID_;
  uint32_t id_;

};

