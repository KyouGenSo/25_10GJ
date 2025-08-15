#include "followCamera.h"
#include "imgui.h"
#ifdef DEBUG
#include "ImGuiManager.h"
#endif // DEBUG

FollowCamera::FollowCamera() {}

FollowCamera::~FollowCamera() {}

void FollowCamera::Initialize(Camera* camera) {

  input_ = Input::GetInstance();

  camera_ = camera;
}

void FollowCamera::Finalize()
{

}

void FollowCamera::Update()
{
  if (mode_)
  {
    FirstPersonMode();
  } else
  {
    TopDownMode();
  }

}

void FollowCamera::FirstPersonMode()
{
  // ゲームパッドによる回転
  if (!Input::GetInstance()->RStickInDeadZone()) {
    isRotating_ = true;
  } else {
    isRotating_ = false;
  }

  if (isRotating_)
    destinationAngleY_ += Input::GetInstance()->GetRightStick().x * rotateSpeed_ /** 0.0001f*/;

  // 右スティック押し込みで角度をターゲットの後ろにリセット
  if (Input::GetInstance()->TriggerButton(XButtons.R_Thumbstick)) {
    destinationAngleY_ = target_->rotate.y;
  }

  // キーボードによる回転
  if (input_->PushKey(DIK_LEFT)) {
    destinationAngleY_ -= rotateSpeed_;
  }
  if (input_->PushKey(DIK_RIGHT)) {
    destinationAngleY_ += rotateSpeed_;
  }

  // offset
  offset_.z = Vec3::Lerp(offset_.z, offsetOrigin_.z, 0.08f);


  // カメラの角度を目標角度に向けて補間
  float angle = Vec3::LerpShortAngle(camera_->GetRotate().y, destinationAngleY_, 0.15f);
  camera_->SetRotate(Vector3(0.0f, angle, 0.0f));


  // playerの位置に補間して追従
  if (target_) {
    // ターゲットの位置に補間
    interTargetPos_ = Vec3::Lerp(interTargetPos_, target_->translate, t_);

    // ターゲットの位置にカメラを追従
    Vector3 offset = CalculateOffset();

    camera_->SetTranslate(interTargetPos_ + offset);
  }
}

void FollowCamera::TopDownMode()
{
  if (!target_) return;

  // 2つのターゲットがある場合は中心を計算、1つの場合はそのターゲットの位置
  Vector3 centerPos;
  if (target2_) {
    centerPos = Vec3::Add(target_->translate, target2_->translate);
    centerPos = Vec3::Multiply(centerPos, 0.5f);
  } else {
    centerPos = target_->translate;
  }

  // 中心位置に補間して追従
  interTargetPos_ = Vec3::Lerp(interTargetPos_, centerPos, t_);

  // カメラの高度を計算
  float cameraHeight = topDownBaseHeight_;
  if (target2_) {
    // 2つのターゲット間の距離を計算
    Vector3 distance = Vec3::Subtract(target_->translate, target2_->translate);
    float targetDistance = static_cast<float>(Vec3::Length(distance));
    
    // 距離に応じて高度を調整
    cameraHeight = topDownBaseHeight_ + targetDistance * topDownHeightMultiplier_;
    
    // 最小・最大高度で制限
    if (cameraHeight < topDownMinHeight_) cameraHeight = topDownMinHeight_;
    if (cameraHeight > topDownMaxHeight_) cameraHeight = topDownMaxHeight_;
  }

  // カメラの位置を設定（中心の真上）
  Vector3 cameraPos = interTargetPos_;
  cameraPos.y = cameraHeight;
  camera_->SetTranslate(cameraPos);

  // カメラの回転を固定（真下を向く）
  camera_->SetRotate(Vector3(1.5708f, 0.0f, 0.0f)); // 90度（π/2ラジアン）をX軸で回転
}

void FollowCamera::Reset() {
  if (target_) {
    interTargetPos_ = target_->translate;
    camera_->SetRotate(Vector3(0.0f, target_->rotate.y, 0.0f));
  }

  destinationAngleY_ = camera_->GetRotate().y;

  offset_ = { offsetOrigin_.x, offsetOrigin_.y, offsetOrigin_.z };

  Vector3 offset = CalculateOffset();

  camera_->SetTranslate(interTargetPos_ + offset);
}

void FollowCamera::ResetOffset() {
  offset_.x = offsetOrigin_.x;
  offset_.y = offsetOrigin_.y;
}

Vector3 FollowCamera::CalculateOffset() const {
  Vector3 offset = offset_;

  // カメラの角度から回転行列を算出
  Matrix4x4 rotationMatrix = Mat4x4::MakeRotateXYZ(camera_->GetRotate());

  offset = Mat4x4::TransFormNormal(rotationMatrix, offset);

  return offset;
}

void FollowCamera::SetTarget(const Transform* target) {
  target_ = target;
  Reset();
}

void FollowCamera::SetTarget2(const Transform* target2) {
  target2_ = target2;
}

void FollowCamera::DrawImGui() {
#ifdef _DEBUG
  ImGui::Begin("FollowCamera");

  // モード選択
  bool modeChanged = ImGui::Checkbox("FirstPersonMode", &mode_);
  if (modeChanged) {
    Reset();
  }

  if (mode_) {
    // FirstPersonMode用の設定
    ImGui::Separator();
    ImGui::Text("FirstPerson Settings");
    ImGui::DragFloat("Offset.x", &offset_.x, 0.1f);
    ImGui::DragFloat("Offset.y", &offset_.y, 0.1f);
    ImGui::DragFloat("Offset.z", &offsetOrigin_.z, 0.1f);
    ImGui::DragFloat("Rotate Speed", &rotateSpeed_, 0.01f);
  } else {
    // TopDownMode用の設定
    ImGui::Separator();
    ImGui::Text("TopDown Settings");
    ImGui::DragFloat("Base Height", &topDownBaseHeight_, 0.5f, 5.0f, 100.0f);
    ImGui::DragFloat("Height Multiplier", &topDownHeightMultiplier_, 0.01f, 0.0f, 2.0f);
    ImGui::DragFloat("Min Height", &topDownMinHeight_, 0.5f, 1.0f, 50.0f);
    ImGui::DragFloat("Max Height", &topDownMaxHeight_, 0.5f, 10.0f, 200.0f);
    
    ImGui::Separator();
    ImGui::Text("Target Info");
    if (target_) {
      ImGui::Text("Target1: Set");
    } else {
      ImGui::Text("Target1: Not Set");
    }
    if (target2_) {
      ImGui::Text("Target2: Set");
    } else {
      ImGui::Text("Target2: Not Set");
    }
  }

  ImGui::End();
#endif _DEBUG
}