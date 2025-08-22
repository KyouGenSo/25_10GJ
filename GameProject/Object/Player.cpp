#include "Player.h"
#include "Input.h"
#include "Camera.h"

void Player::Initialize()
{
  model_ = std::make_unique<Object3d>();
  model_->Initialize();
  model_->SetModel("Player.gltf");

  transform_.translate = Vector3(0.0f, 2.5f, 0.0f);
  transform_.rotate = Vector3(0.0f, 0.0f, 0.0f);
  transform_.scale = Vector3(1.0f, 1.0f, 1.0f);

  model_->SetTransform(transform_);
}

void Player::Finalize()
{
}

void Player::Update()
{
  Move();
  transform_.rotate.y = Vec3::LerpShortAngle(transform_.rotate.y, targetAngle_, 0.2f);
  model_->SetTransform(transform_);
  model_->Update();
}

void Player::Draw()
{
  model_->Draw();
}

void Player::Move()
{
  bool isMoving = false;
  Matrix4x4 rotationMatrix;

  Input* input = Input::GetInstance();
  velocity_ = { input->GetLeftStick().x, 0.0f, input->GetLeftStick().y };

  if (!input->LStickInDeadZone())
  {
    isMoving = true;
  }

  if (isMoving)
  {
    velocity_ = velocity_.Normalize() * speed_;

    if (mode_)
    {
      rotationMatrix = Mat4x4::MakeRotateY(camera_->GetRotateY());

      velocity_ = Mat4x4::TransformNormal(rotationMatrix, velocity_);
    }

    transform_.translate += velocity_;

    targetAngle_ = std::atan2(velocity_.x, velocity_.z);
  }

}

void Player::DrawImGui()
{
}