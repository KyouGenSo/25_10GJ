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

  //-----------------------------Getters/Setters------------------------------//
  void SetTransform(const Transform& transform) { transform_ = transform; }
  void SetHp(float hp) { hp_ = hp; }

  const Transform& GetTransform() const { return transform_; }
  Transform* GetTransformPtr() { return &transform_; }
  float GetHp() const { return hp_; }
  AABBCollider* GetCollider() const { return bodyCollider_.get(); }

private:
  std::unique_ptr<Object3d> model_; ///< モデル
  Transform transform_{};           ///< 変形情報

  float hp_ = 200.0f;               ///< 体力
  
  // Collider
  std::unique_ptr<AABBCollider> bodyCollider_;
};

