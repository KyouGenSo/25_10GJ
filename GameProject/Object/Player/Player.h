#pragma once
#include <memory>

#include "Object3d.h"

class PlayerStateMachine;
class InputHandler;
class Camera;

class Player
{
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
  /// 移動
  /// </summary>
  /// <param name="speedMultiplier">速度倍率（デフォルト1.0）</param>
  void Move(float speedMultiplier = 1.0f);

  /// <summary>
  /// ImGuiの描画
  /// </summary>
  void DrawImGui();

  //-----------------------------Getters/Setters------------------------------//
  void SetSpeed(float speed) { speed_ = speed; }
  float GetSpeed() const { return speed_; }
  void SetCamera(Camera* camera) { camera_ = camera; }
  Camera* GetCamera() const { return camera_; }
  void SetMode(bool mode) { mode_ = mode; } // true: FirstPersonMode, false: TopDownMode
  bool GetMode() const { return mode_; }

  // 変形情報を取得・設定
  const Transform& GetTransform() const { return transform_; }
  void SetTransform(const Transform& transform) { transform_ = transform; }

  // モデルへのアクセス
  Object3d* GetModel() const { return model_.get(); }

  // システムへのアクセス
  PlayerStateMachine* GetStateMachine() const { return stateMachine_.get(); }
  InputHandler* GetInputHandler() const { return inputHandler_.get(); }

private: // メンバ変数

  std::unique_ptr<Object3d> model_; ///< モデル
  Camera* camera_ = nullptr; ///< カメラ

  Transform transform_{}; ///< 変形情報
  Vector3 velocity_{}; ///< 速度
  float speed_ = 0.5f; ///< 移動速度
  float targetAngle_ = 0.f; ///< 目標角度

  bool mode_ = false; // true: FirstPersonMode, false: TopDownMode

  // システム
  std::unique_ptr<PlayerStateMachine> stateMachine_;
  std::unique_ptr<InputHandler> inputHandler_;
};

