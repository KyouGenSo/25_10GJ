#include "Player.h"
#include "Object3d.h"
#include "Input.h"
#include "Camera.h"
#include "Input/InputHandler.h"
#include "AABBCollider.h"
#include "CollisionManager.h"
#include "../../Collision/CollisionTypeIdDef.h"
#include <cmath>

#include "GPUParticle.h"
#include "ImGui.h"
#include "FollowCamera/followCamera.h"
#include "Terrain/Block/Block.h"

Player::Player()
    :camera_(nullptr)
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

    transform_.translate = Vector3(2.0f, 7.f, 2.0f);
    transform_.rotate = Vector3(0.0f, 0.0f, 0.0f);
    transform_.scale = Vector3(1.0f, 1.0f, 1.0f);

    model_->SetTransform(transform_);

    // Input Handlerの初期化
    inputHandler_ = std::make_unique<InputHandler>();
    inputHandler_->Initialize();

    dispenser_ = std::make_unique<Dispenser>();
    dispenser_->Initialize().SetOwner(this);

    weapon_ = std::make_unique<Object3d>();
    weapon_->Initialize();
    weapon_->SetModel("weapon_stick.gltf");
    weapon_->SetTransform(transform_);
    weapon_->SetScale({ 10.f,10.f,10.f });
    weapon_->SetRotate({ 0.f, 0.f, 1.57f }); // Z軸を90度回転させて横向きに
    weapon_->SetMaterialColor({ 1.f, 1.f, 1.f, 1.f });

    // Colliderの設定
    SetupColliders();

    emitter_ = std::make_unique<EmitterManager>(GPUParticle::GetInstance());
    emitter_->CreateSphereEmitter(emitterName_, transform_.translate, 2.f, 30, 0.55f);
    emitter_->SetEmitterScaleRange(emitterName_, {0.5f, 0.5f}, {0.5f, 0.5f});
    emitter_->SetEmitterActive(emitterName_, false);
}

void Player::Finalize()
{

    if (attackCollider_)
    {
        CollisionManager::GetInstance()->RemoveCollider(attackCollider_.get());
    }

    // Colliderを削除
    if (bodyCollider_)
    {
        CollisionManager::GetInstance()->RemoveCollider(bodyCollider_.get());
    }
}

void Player::Update()
{

    // Inputの更新（StateのHandleInputより前に実行）
    if (inputHandler_)
    {
        inputHandler_->Update(this);
    }

    if (isAttacking_)
    {
        if (timer_ <= kMotionTime)
        { // InProgress
            timer_ += 1.f / 60.f;

            weapon_->Update();
        }
        else
        {
            CollisionManager::GetInstance()->RemoveCollider(attackCollider_.get());
            timer_ = 0.f;
            isAttacking_ = false;
        }
    }
    Action();

    dispenser_->Update();

    // モデルの更新
    model_->SetTransform(transform_);
    model_->Update();

    emitter_->SetEmitterPosition(emitterName_, transform_.translate);
    emitter_->Update();
}

void Player::Draw()
{
    if (isAttacking_)weapon_->Draw();
    model_->Draw();
}

void Player::InstancedDraw()
{
    dispenser_->Draw();
}

void Player::DrawHUD() const {
    dispenser_->DrawHUD();
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
    if (mode_ && camera_)
    {
        Matrix4x4 rotationMatrix = Mat4x4::MakeRotateY(camera_->GetRotateY());
        velocity_ = Mat4x4::TransformNormal(rotationMatrix, velocity_);
    }

    // 移動方向を向く
    if (velocity_.Length() > 0.01f)
    {
        targetAngle_ = std::atan2(velocity_.x, velocity_.z);
        transform_.rotate.y = Vec3::LerpShortAngle(transform_.rotate.y, targetAngle_, 0.2f);
    }
}

void Player::Action()
{
    if (inputHandler_) dispenser_->SelectColor(inputHandler_->GetColor());

    //空中は操作させない(例外アリ)
    if (onGround_)
    {
        velocity_.y = 0;

        canDash_ = false;
        canMove_ = false;
        canJump_ = false;
        canAttack_ = false;
        isBuffed_ = false;

        //床の色を判別する
        Block::Colors blockColor = Block::Colors::White;

        if (terrain_)
        {
            Vector3 feetPosition = transform_.translate;
            feetPosition.y -= kSize / 2.f + Block::kScale / 2.f; // 足元の位置を計算
            blockColor = terrain_->GetBlockColorAt(feetPosition);
        }

        if (blockColor == Block::Colors::Purple || blockColor == Block::Colors::Orange || blockColor == Block::Colors::Green){
            emitter_->SetEmitterActive(emitterName_, true);
            emitter_->SetEmitterColor(emitterName_, Block::ColorToVector4(blockColor));
        }else{
            emitter_->SetEmitterActive(emitterName_, false);
        }

        // Debug
        //blockColor = Block::Colors::Gray;

        if (!isDebug_)
        {
            if (blockColor == Block::Colors::Gray && !isAttacking_) canMove_ = true;
            if (blockColor == Block::Colors::Red || blockColor == Block::Colors::Purple) {
                isBuffed_ = (blockColor == Block::Colors::Purple);
                canAttack_ = true;
            }
            if (blockColor == Block::Colors::Blue || blockColor == Block::Colors::Green) canDash_ = true;
            if (blockColor == Block::Colors::Yellow || blockColor == Block::Colors::Orange || blockColor == Block::Colors::Green){
                isBuffed_ = (blockColor == Block::Colors::Orange);
                canJump_ = true;
            }
        }

        if (inputHandler_->IsDispense())
        {
            Dispense();
        }
    }

    if (canMove_) Move();
    if (canDash_ && inputHandler_->IsDashing()) Dash();
    if (canJump_ && inputHandler_->IsJumping()) Jump(isBuffed_);
    if (canAttack_ && inputHandler_->IsAttacking()) Attack(isBuffed_);

    Apply();
}

void Player::Apply()
{
    // 位置を更新
    transform_.translate += velocity_;

    velocity_ *= friction_; // 摩擦
    velocity_.y -= gravity_; // 重力
}

void Player::Jump(bool _isBuffed)
{
    velocity_.y = jumpForce_;

    if (_isBuffed) velocity_.y *= 2.f;

    canJump_ = false;
    onGround_ = false;
}

void Player::Dash()
{
    canDash_ = false;
    velocity_.x = std::sin(transform_.rotate.y) * dashForce_;
    velocity_.z = std::cos(transform_.rotate.y) * dashForce_;
    velocity_.y = 0.f;
}

void Player::Attack(bool _isBuffed)
{
    if (canAttack_ && !isAttacking_)
    {
        //攻撃開始フレーム
        canAttack_ = false;
        isAttacking_ = true;
        damage_ = kDamage;
        attackRange_ = defaultAttackRange;

        if (_isBuffed){
            damage_ *= 3.f;
            attackRange_ *= 1.5f;
        }
        attackCollider_->SetSize(attackRange_);

        attackCollider_->SetOffset({ sinf(transform_.rotate.y) * offset_, 0.f, cosf(transform_.rotate.y) * offset_ });
        CollisionManager::GetInstance()->AddCollider(attackCollider_.get());
    }
}

void Player::Dispense()
{
    Vector3 direction = model_->GetTransform().rotate;
    dispenser_->Dispense(direction);
}

void Player::DrawImGui()
{
    #ifdef _DEBUG
    ImGui::Begin("Player");

    // Transform調整
    if (ImGui::CollapsingHeader("Transform"))
    {
        ImGui::DragFloat3("Position", &transform_.translate.x, 0.1f);
        ImGui::DragFloat3("Rotation", &transform_.rotate.x, 0.01f);
        ImGui::DragFloat3("Scale", &transform_.scale.x, 0.01f);
    }

    // Movement調整
    if (ImGui::CollapsingHeader("Movement"))
    {
        ImGui::DragFloat("Speed", &speed_, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat3("Velocity", &velocity_.x, 0.01f);
        ImGui::Checkbox("On Ground", &onGround_);
        ImGui::DragFloat("Target Angle", &targetAngle_, 0.01f);
        ImGui::Checkbox("Camera Mode", &mode_);
        ImGui::Checkbox("Debug Mode", &isDebug_);
    }

    // Attack調整
    if (ImGui::CollapsingHeader("Attack"))
    {
        ImGui::Checkbox("Is Attacking", &isAttacking_);
        ImGui::DragFloat("Attack Timer", &timer_, 0.01f, 0.0f, 2.0f);
        ImGui::DragFloat("Damage", &damage_, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat3("Attack Size", &defaultAttackRange.x, 0.1f);
        ImGui::DragFloat("offset", &offset_, 0.1f);

        static float motionTime = kMotionTime;
        if (ImGui::DragFloat("Motion Time", &motionTime, 0.01f, 0.0f, 2.0f))
        {
            const_cast<float&>(kMotionTime) = motionTime;
        }
    }

    // Physics調整
    if (ImGui::CollapsingHeader("Physics"))
    {
        ImGui::DragFloat("Gravity", &gravity_, 0.001f, 0.0f, 1.0f);
        ImGui::DragFloat("Friction", &friction_, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Jump Force", &jumpForce_, 0.01f, 0.0f, 5.0f);
        ImGui::DragFloat("Dash Force", &dashForce_, 0.01f, 0.0f, 10.0f);
    }

    dispenser_->DrawImGui();

    ImGui::End();
    #endif
}

void Player::SetupColliders()
{
    CollisionManager* collisionManager = CollisionManager::GetInstance();

    // 本体のCollider
    bodyCollider_ = std::make_unique<PlayerCollider>(this);
    bodyCollider_->SetTransform(&transform_);
    bodyCollider_->SetSize(Vector3(kSize, kSize, kSize));
    bodyCollider_->SetOffset(Vector3(0.0f, 0.0f, 0.0f));
    bodyCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeId::kPlayer));

    // 攻撃
    attackCollider_ = std::make_unique<AABBCollider>();
    attackCollider_->SetTransform(&transform_);
    attackCollider_->SetSize({ 3.f, 0.5f, 3.f });
    attackCollider_->SetOffset({ 0.f,0.f,0.f });
    attackCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeId::kAttack));
    attackCollider_->SetOwner(this);

    // CollisionManagerに登録
    collisionManager->AddCollider(bodyCollider_.get());
    collisionManager->SetCollisionMask(static_cast<uint32_t>(CollisionTypeId::kPlayer), static_cast<uint32_t>(CollisionTypeId::kTerrain), true);
    collisionManager->SetCollisionMask(static_cast<uint32_t>(CollisionTypeId::kAttack), static_cast<uint32_t>(CollisionTypeId::kEnemy), true);
}

void Player::OnGround()
{
    onGround_ = true;
}

void Player::OffGround()
{
    onGround_ = false;
}