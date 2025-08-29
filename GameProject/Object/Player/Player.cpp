#include "Player.h"
#include "Object3d.h"
#include "Input.h"
#include "Camera.h"
#include "State/PlayerStateMachine.h"
#include "State/IdleState.h"
#include "State/MoveState.h"
#include "State/DashState.h"
#include "State/AttackState.h"
#include "State/ShootState.h"
#include "State/ParryState.h"
#include "Input/InputHandler.h"

Player::Player()
  : camera_(nullptr)
  , speed_(1.0f)
  , targetAngle_(0.0f)
  , mode_(false)
{
}

Player::~Player()
{
}

void Player::Initialize()
{
  model_ = std::make_unique<Object3d>();
  model_->Initialize();
  model_->SetModel("Player.gltf");

  transform_.translate = Vector3(0.0f, 2.5f, 0.0f);
  transform_.rotate = Vector3(0.0f, 0.0f, 0.0f);
  transform_.scale = Vector3(1.0f, 1.0f, 1.0f);

  model_->SetTransform(transform_);

  // State Machineの初期化
  stateMachine_ = std::make_unique<PlayerStateMachine>(this);
  stateMachine_->RegisterState("Idle", std::make_unique<IdleState>());
  stateMachine_->RegisterState("Walk", std::make_unique<MoveState>());
  stateMachine_->RegisterState("Dash", std::make_unique<DashState>());
  stateMachine_->RegisterState("Attack", std::make_unique<AttackState>());
  stateMachine_->RegisterState("Shoot", std::make_unique<ShootState>());
  stateMachine_->RegisterState("Parry", std::make_unique<ParryState>());
  stateMachine_->ChangeState("Idle");
  stateMachine_->Initialize();

  // Input Handlerの初期化
  inputHandler_ = std::make_unique<InputHandler>();
  inputHandler_->Initialize();
}

void Player::Finalize()
{
}

void Player::Update()
{
  float deltaTime = 1.0f / 60.0f; // 60FPSを仮定

  // Inputの更新（StateのHandleInputより前に実行）
  if (inputHandler_)
  {
    inputHandler_->Update(this);
  }

  // State Machineの更新
  if (stateMachine_)
  {
    stateMachine_->HandleInput();
    stateMachine_->Update(deltaTime);
  }

  // 旧実装
  model_->SetTransform(transform_);
  model_->Update();
}

void Player::Draw()
{
  model_->Draw();
}

void Player::Move(float speedMultiplier)
{
  if (!inputHandler_) return;
  
  Vector2 moveDir = inputHandler_->GetMoveDirection();
  if (moveDir.Length() < 0.1f) return;
  
  // 3Dベクトルに変換
  velocity_ = { moveDir.x, 0.0f, moveDir.y };
  velocity_ = velocity_.Normalize() * speed_ * speedMultiplier;
  
  // カメラモードに応じて移動方向を調整
  if (mode_ && camera_)
  {
    Matrix4x4 rotationMatrix = Mat4x4::MakeRotateY(camera_->GetRotateY());
    velocity_ = Mat4x4::TransformNormal(rotationMatrix, velocity_);
  }
  
  // 位置を更新
  transform_.translate += velocity_;
  
  // 移動方向を向く
  if (velocity_.Length() > 0.01f)
  {
    targetAngle_ = std::atan2(velocity_.x, velocity_.z);
    transform_.rotate.y = Vec3::LerpShortAngle(transform_.rotate.y, targetAngle_, 0.2f);
  }
}

void Player::DrawImGui()
{
}