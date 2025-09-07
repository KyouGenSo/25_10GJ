#include "Player.h"
#include "Object3d.h"
#include "Input.h"
#include "Camera.h"
#include "Input/InputHandler.h"
#include "AABBCollider.h"
#include "CollisionManager.h"
#include "../../Collision/CollisionTypeIdDef.h"
#include <cmath>
#include "ImGui.h"
#include "Terrain/Block/Block.h"

Player::Player()
    :camera_(nullptr)
    ,speed_(1.0f)
    ,targetAngle_(0.0f)
    ,mode_(false)
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
    
    // Input Handlerの初期化
    inputHandler_ = std::make_unique<InputHandler>();
    inputHandler_->Initialize();
    
    // Colliderの設定
    SetupColliders();
}

void Player::Finalize()
{
    // Colliderを削除
    if (bodyCollider_) {
      CollisionManager::GetInstance()->RemoveCollider(bodyCollider_.get());
    }
}

void Player::Update()
{
    float deltaTime = 1.0f / 60.0f; // 60FPSを仮定
    
    // Inputの更新（StateのHandleInputより前に実行）
    if (inputHandler_)
    {
      inputHandler_->Update(this);
    }
    
    Action();
    
    // モデルの更新
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
    velocity_ = { moveDir.x, velocity_.y, moveDir.y };
    velocity_ = velocity_.Normalize() * speed_ * speedMultiplier;

    // カメラモードに応じて移動方向を調整
    if (mode_ && camera_){
        Matrix4x4 rotationMatrix = Mat4x4::MakeRotateY(camera_->GetRotateY());
        velocity_ = Mat4x4::TransformNormal(rotationMatrix, velocity_);
    }

    // 移動方向を向く
    if (velocity_.Length() > 0.01f){
        targetAngle_ = std::atan2(velocity_.x, velocity_.z);
        transform_.rotate.y = Vec3::LerpShortAngle(transform_.rotate.y, targetAngle_, 0.2f);
    }
}

void Player::Action() {
    //床の色を判別する
    Block::Colors blockColor = Block::Colors::White;

    if (terrain_) {
        blockColor = terrain_->GetBlockColorAt(transform_.translate);
    }

    if (inputHandler_) Move();
    if (blockColor == Block::Colors::Red && inputHandler_->IsAttacking()) Attack();
    if (blockColor == Block::Colors::Blue && inputHandler_->IsDashing()) Dash();
    if (blockColor == Block::Colors::Yellow && inputHandler_->IsJumping()) Jump();

    Apply();
}

void Player::Apply() {
    // 位置を更新
    transform_.translate += velocity_;

    velocity_ *= 0.8f; // 摩擦
    velocity_.y -= 0.1f; // 重力

    if (transform_.translate.y <= 1.5f){
        transform_.translate.y = 1.5f;
        velocity_.y = 0.f;
    }
}

void Player::Jump() {
    velocity_.y = 1.f;
}

void Player::Dash() {
    float dashForce = 3.0f;
    float currentY = velocity_.y;
    
    velocity_.x = std::sin(transform_.rotate.y) * dashForce;
    velocity_.z = std::cos(transform_.rotate.y) * dashForce;
    velocity_.y = currentY;
}

void Player::Attack() {

}

void Player::DrawImGui()
{
#ifdef _DEBUG
    ImGui::Begin("Player");
    
    const char* colorNames[] = { "White", "Gray", "Blue", "Green", "Red", "Yellow", "Purple", "Orange" };
    int currentColor = static_cast<int>(color_);
    
    if (ImGui::Combo("Color", &currentColor, colorNames, IM_ARRAYSIZE(colorNames))) {
        color_ = static_cast<Block::Colors>(currentColor);
    }
    
    if (ImGui::Button("SetColor")){
        terrain_->SetBlockColorAt(transform_.translate, color_);
    }
    
    ImGui::End();
#endif
}

void Player::SetupColliders()
{
    // 本体のCollider
    bodyCollider_ = std::make_unique<AABBCollider>();
    bodyCollider_->SetTransform(&transform_);
    bodyCollider_->SetSize(Vector3(1.5f, 1.5f, 1.5f));
    bodyCollider_->SetOffset(Vector3(0.0f, 0.0f, 0.0f));
    bodyCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeId::kPlayer));
    bodyCollider_->SetOwner(this);
    
    // CollisionManagerに登録
    CollisionManager* collisionManager = CollisionManager::GetInstance();
    collisionManager->AddCollider(bodyCollider_.get());
}