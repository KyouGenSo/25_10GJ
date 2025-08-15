#pragma once
#include "Input.h"
#include "Mat4x4Func.h"
#include "Vec3Func.h"
#include "Transform.h"
#include "Camera.h"
#include <math.h>

class FollowCamera {

public: // メンバ関数
  FollowCamera();
  ~FollowCamera();

  void Initialize(Camera* camera);

  void Finalize();

  void Update();

  void FirstPersonMode();
  void TopDownMode();

  void Reset();

  void ResetOffset();

  void DrawImGui();

  // offsetの計算関数
  Vector3 CalculateOffset() const;

  // 画面揺れ
  void ShakeScreen(float power);

  //-----------------------------Setters------------------------------//
  void SetTarget(const Transform* target);
  void SetTarget2(const Transform* target2);
  void SetOffset(const Vector3& offset)
  {
    offset_.x = offset.x;
    offset_.y = offset.y;
    offsetOrigin_.z = offset.z;
  }
  void SetRotateSpeed(const float speed) { rotateSpeed_ = speed; }

  /// <summary>
  /// Set the camera mode.
  /// </summary>
  /// <param name="mode">true: FirstPersonMode, false: TopDownMode</param>
  void SetMode(bool mode) { mode_ = mode; } // true: FirstPersonMode, false: TopDownMode

  //-----------------------------Getters------------------------------//
  Camera* GetCamera() { return camera_; }
  const Matrix4x4& GetViewProjection() const { return camera_->GetViewProjectionMatrix(); }
  const Vector3& GetOffset() const { return offset_; }
  const Transform* GetTarget2() const { return target2_; }
  bool GetMode() const { return mode_; } // true: FirstPersonMode, false: TopDownMode

private: // メンバ変数
  Camera* camera_;

  const Transform* target_ = nullptr;
  const Transform* target2_ = nullptr;

  Input* input_ = nullptr;

  bool mode_ = false; // true: FirstPersonMode, false: TopDownMode

  Vector3 interTargetPos_;

  Vector3 offset_ = { 0.0f, 3.0f, -18.0f };

  Vector3 offsetOrigin_ = { 0.0f, 3.0f, -18.0f };

  float t_ = 0.18f;

  float destinationAngleY_ = 0.0f;

  bool isRotating_ = false;

  float rotateSpeed_ = 0.05f;

  // TopDownMode専用パラメータ
  float topDownBaseHeight_ = 50.0f;
  float topDownHeightMultiplier_ = 1.5f;
  float topDownMinHeight_ = 20.0f;
  float topDownMaxHeight_ = 500.0f;

};

