#include "Player.h"
#include "Object3d.h"
#include "Input.h"
#include "Camera.h"
#include "Input/InputHandler.h"
#include "AABBCollider.h"
#include "CollisionManager.h"
#include "../../Collision/CollisionTypeIdDef.h"
#include <cmath>

#include "Audio.h"
#include "GPUParticle.h"
#include "ImGui.h"
#include "FollowCamera/followCamera.h"
#include "Terrain/Block/Block.h"
#include "Bar3d/Bar3d.h"

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
    
    // weaponTransform_の初期化
    weaponTransform_.translate = transform_.translate;
    weaponTransform_.rotate = Vector3(1.57f, 0.f, 0.f); // X軸を90度回転させて横向きに（縦モデルを横に倒す）
    weaponTransform_.scale = Vector3(2.f, 1.5f, 2.f);
    
    weapon_->SetTransform(weaponTransform_);
    weapon_->SetMaterialColor({ 1.f, 1.f, 1.f, 1.f });

    // Colliderの設定
    SetupColliders();

    emitter_ = std::make_unique<EmitterManager>(GPUParticle::GetInstance());
    emitter_->CreateSphereEmitter(emitterName_, transform_.translate, 2.f, 30, 0.55f);
    emitter_->SetEmitterScaleRange(emitterName_, {0.5f, 0.5f}, {0.5f, 0.5f});
    emitter_->SetEmitterActive(emitterName_, false);
    
    // SEの読み込み
    hitVH_ = Audio::GetInstance()->LoadWaveFile("playerhit.wav");
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
            
            // 攻撃モーション中のweaponTransform_の更新
            UpdateAttackMotion();
            
            weapon_->Update();
        }
        else
        {
            CollisionManager::GetInstance()->RemoveCollider(attackCollider_.get());
            timer_ = 0.f;
            isAttacking_ = false;
            
            // 攻撃終了時に武器の位置をリセット
            ResetWeaponTransform();
        }
    }
    Action();

    dispenser_->Update();

    // モデルの更新
    model_->SetTransform(transform_);
    model_->Update();
    
    // 武器のトランスフォーム更新（攻撃中でない場合はプレイヤーに追従）
    if (!isAttacking_)
    {
        weaponTransform_.translate = transform_.translate;
        weaponTransform_.rotate.x = 1.57f; // 縦モデルを横に倒す
        weaponTransform_.rotate.y = transform_.rotate.y; // プレイヤーと同じ向き
        weaponTransform_.rotate.z = 0.0f;
        weapon_->SetTransform(weaponTransform_);
    }

    emitter_->SetEmitterPosition(emitterName_, transform_.translate);
    emitter_->Update();

    // HPバーの更新
    if (hpBar_ && camera_)
    {
        // HPバーの位置を更新（プレイヤーの頭上に表示）
        Vector2 screenPos = Bar3d::GetHeadUpPositionOnScreen(
            transform_.translate,
            hpBarSize_,
            *camera_,
            kSize + hpBarOffsetY_  // Y軸オフセット（プレイヤーのサイズ分上に）
        );
        hpBar_->SetPosition(screenPos);
        hpBar_->SetCurrentValue(hp_);
        hpBar_->Display(1000.0f);
        hpBar_->Update();
    }
}

void Player::Draw()
{
    if (isAttacking_)weapon_->Draw();
    model_->Draw();
}

void Player::Draw2d()
{
    // HPバーの2D描画
    if (hpBar_)
    {
        hpBar_->Draw2d();
    }
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
    if (!onGround_)return;

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
            if ((blockColor == Block::Colors::Gray || blockColor == Block::Colors::DarkBlue || blockColor == Block::Colors::DarkRed || blockColor == Block::Colors::DarkYellow) && !isAttacking_) canMove_ = true;
            if (blockColor == Block::Colors::Red || blockColor == Block::Colors::Purple || blockColor == Block::Colors::Orange || blockColor == Block::Colors::DarkRed) {
                isBuffed_ = (blockColor == Block::Colors::Purple);
                canAttack_ = true;
            }
            if (blockColor == Block::Colors::Blue || blockColor == Block::Colors::Green || blockColor == Block::Colors::DarkBlue) canDash_ = true;
            if (blockColor == Block::Colors::Yellow || blockColor == Block::Colors::Orange || blockColor == Block::Colors::Green || blockColor == Block::Colors::DarkYellow){
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

    if (transform_.translate.x <= kSize/2.f){
        transform_.translate.x = kSize/2.f;
    }
    if (transform_.translate.z <= kSize/2.f){
        transform_.translate.z = kSize/2.f;
    }

    float _max = Terrain::kSize * Terrain::kSize/2.f - kSize/2.f;
    if (_max <= transform_.translate.x){
        transform_.translate.x = _max;
    }
    if (_max <= transform_.translate.z){
        transform_.translate.z = _max;
    }

    velocity_ *= friction_; // 摩擦
    velocity_.y -= gravity_; // 重力

    onGround_ = false;
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

    // HPバーデバッグ情報
    if (ImGui::CollapsingHeader("HP Bar Debug"))
    {
        ImGui::Text("HP Bar Status:");
        ImGui::Text("  - Exists: %s", hpBar_ ? "Yes" : "No");
        
        if (hpBar_) {
            ImGui::Text("  - Camera: %s", camera_ ? "Set" : "NULL");
            ImGui::Text("  - Current HP: %.1f / %.1f", hp_, kMaxHp);
            
            ImGui::Separator();
            ImGui::Text("HP Bar Settings:");
            
            // サイズ調整
            if (ImGui::DragFloat2("HP Bar Size", &hpBarSize_.x, 1.0f, 1.0f, 200.0f)) {
                hpBar_->SetSize(hpBarSize_);
            }
            
            // Y軸オフセット調整
            if (ImGui::DragFloat("Y Offset", &hpBarOffsetY_, 0.1f, 0.0f, 20.0f)) {
                // オフセットの変更は次のUpdateで反映される
            }
            
            // 現在のHP値を調整（テスト用）
            if (ImGui::DragFloat("Test HP Value", &hp_, 1.0f, 0.0f, kMaxHp)) {
                hpBar_->SetCurrentValue(hp_);
            }
            
            // 表示状態の切り替え
            bool isDisplay = true;
            if (ImGui::Checkbox("Display HP Bar", &isDisplay)) {
                hpBar_->Display(isDisplay);
            }
        }
        
        ImGui::Separator();
        
        // HPバー再初期化ボタン
        if (ImGui::Button("Reinitialize HP Bar") && camera_) {
            hpBar_ = std::make_unique<Bar3d>();
            Color hpColor = 0x00ff00ff;  // 緑色
            Color bgColor = 0x000000ff;  // 黒
            hpBar_->Initialize(hpColor, bgColor);
            hpBar_->SetSize(hpBarSize_);
            hpBar_->SetMaxValue(kMaxHp);
            hpBar_->SetCurrentValue(hp_);
            hpBar_->Display(true);
        }
    }

    ImGui::End();
    #endif
}

void Player::SetCamera(Camera* camera)
{
    camera_ = camera;
    
    // カメラが設定されたらHPバーを初期化（条件を修正）
    if (camera_)  // !hpBar_の条件を削除
    {
        hpBar_ = std::make_unique<Bar3d>();
        Color hpColor = 0x00ff00ff;  // 緑色
        Color bgColor = 0x000000ff;  // 黒
        hpBar_->Initialize(hpColor, bgColor);
        hpBar_->SetSize(hpBarSize_);
        hpBar_->SetMaxValue(kMaxHp);
        hpBar_->SetCurrentValue(hp_);
        hpBar_->Display(true);  // 常に表示
    }
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
}

void Player::PlayHitSE()
{
    Audio::GetInstance()->Play(hitVH_);
}

void Player::OnGround()
{
    onGround_ = true;
}

void Player::OffGround()
{
    onGround_ = false;
}

void Player::SetCellFilter(CellBasedFiltering* _cellFiltering) {
    cellFilter_ = _cellFiltering;
}

void Player::UpdateAttackMotion()
{
    // 攻撃時間の正規化（0.0～1.0）
    float normalizedTime = timer_ / kMotionTime;
    
    // 武器の振り回し半径
    float swingRadius = 2.5f;
    
    // 攻撃開始角度（左側: -π/2）から終了角度（右側: π/2）までの半回転
    float startAngle = -3.14159f / 2.0f; // -90度
    float endAngle = 3.14159f / 2.0f;    // +90度
    float currentSwingAngle = startAngle + (endAngle - startAngle) * normalizedTime;
    
    // プレイヤーの向いている方向を基準角度とする
    float baseAngle = transform_.rotate.y;
    float totalAngle = baseAngle + currentSwingAngle;
    
    // プレイヤーの座標を中心として武器の位置を計算（sin/cosを修正）
    weaponTransform_.translate.x = transform_.translate.x + std::sin(totalAngle) * swingRadius;
    weaponTransform_.translate.y = transform_.translate.y + 0.5f; // 少し上に
    weaponTransform_.translate.z = transform_.translate.z + std::cos(totalAngle) * swingRadius;
    
    // 武器の向きを攻撃の軌道に合わせて調整
    // まず縦モデルを横に倒す回転（X軸90度）+ 攻撃の向き（Y軸回転）
    weaponTransform_.rotate.x = 1.57f; // 縦モデルを横に倒す
    weaponTransform_.rotate.y = totalAngle; // 攻撃の向きに合わせて回転
    weaponTransform_.rotate.z = 0.0f;
    
    // 武器のトランスフォームを適用
    weapon_->SetTransform(weaponTransform_);
}

void Player::ResetWeaponTransform()
{
    // 武器の位置をプレイヤーの位置にリセット
    weaponTransform_.translate = transform_.translate;
    weaponTransform_.rotate.x = 1.57f; // 縦モデルを横に倒す
    weaponTransform_.rotate.y = transform_.rotate.y; // プレイヤーと同じ向き
    weaponTransform_.rotate.z = 0.0f;
    
    weapon_->SetTransform(weaponTransform_);
}