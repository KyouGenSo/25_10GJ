#include "EnergyCore.h"
#include "Object3d.h"
#include "CollisionManager.h"
#include "Model.h"
#include "FrameTimer.h"
#include "../Boss/Boss.h"
#include "../../Collision/EnergyCoreCollider.h"
#include "../../Collision/CollisionTypeIdDef.h"
#include <cmath>

#ifdef _DEBUG
#include "ImGui.h"
#endif

EnergyCore::EnergyCore()
{
}

EnergyCore::~EnergyCore()
{
}

void EnergyCore::Initialize(Boss* boss)
{
    pBoss_ = boss;
    isDestroyed_ = false;
    
    // モデルの初期化
    model_ = std::make_unique<Object3d>();
    model_->Initialize();
    model_->SetModel("EnergyCore.gltf");
    
    // デフォルトのトランスフォーム設定
    transform_.translate = Vector3(0.0f, 0.0f, 0.0f);
    transform_.rotate = Vector3(0.0f, 0.0f, 0.0f);
    transform_.scale = Vector3(1.0f, 1.0f, 1.0f);
    model_->SetTransform(transform_);
    
    // コライダーの初期化
    collider_ = std::make_unique<EnergyCoreCollider>(this, pBoss_);
    collider_->SetTransform(&transform_);
    collider_->SetSize(Vector3(3.0f, 6.0f, 3.0f));
    collider_->SetOffset(Vector3(0.0f, 2.7f, 0.0f));
    collider_->SetActive(true);
    
    // CollisionManagerに登録
    CollisionManager::GetInstance()->AddCollider(collider_.get());
}

void EnergyCore::Finalize()
{
    // Colliderを削除
    if (collider_)
    {
        CollisionManager::GetInstance()->RemoveCollider(collider_.get());
    }
}

void EnergyCore::Update()
{
    if (!isDestroyed_)
    {
        // 出現アニメーションの更新
        if (isSpawning_)
        {
            UpdateSpawnAnimation();
        }
        
        model_->SetTransform(transform_);
        model_->Update();
        
        // コライダーの更新
        if (collider_)
        {
            collider_->SetTransform(&transform_);
        }
    }
}

void EnergyCore::Draw()
{
    if (!isDestroyed_)
    {
        model_->Draw();
    }
}

void EnergyCore::DrawImGui()
{
#ifdef _DEBUG
    // Transform設定
    ImGui::Text("Transform");
    ImGui::DragFloat3("Position", &transform_.translate.x, 0.1f);
    ImGui::DragFloat3("Rotation", &transform_.rotate.x, 0.1f);
    ImGui::DragFloat3("Scale", &transform_.scale.x, 0.1f, 0.1f, 10.0f);
    
    ImGui::Separator();
    
    // 状態表示
    ImGui::Text("Destroyed: %s", isDestroyed_ ? "True" : "False");
    
    if (ImGui::Button("Destroy"))
    {
        Destroy();
    }
    ImGui::SameLine();
    if (ImGui::Button("Respawn"))
    {
        Respawn();
    }
    
    // Collider設定
    if (collider_)
    {
        ImGui::Separator();
        ImGui::Text("Collider Settings");
        
        // Size
        Vector3 size = collider_->GetSize();
        if (ImGui::DragFloat3("Size", &size.x, 0.1f, 0.1f, 100.0f))
        {
            collider_->SetSize(size);
        }
        
        // Offset
        Vector3 offset = collider_->GetOffset();
        if (ImGui::DragFloat3("Offset", &offset.x, 0.1f))
        {
            collider_->SetOffset(offset);
        }
        
        // Active状態
        bool isActive = collider_->IsActive();
        if (ImGui::Checkbox("Active", &isActive))
        {
            collider_->SetActive(isActive);
        }
    }
#endif
}

void EnergyCore::Destroy()
{
    if (!isDestroyed_)
    {
        isDestroyed_ = true;
        
        // コライダーを無効化
        if (collider_)
        {
            collider_->SetActive(false);
        }
        
        // ボスを崩壊状態にする
        if (pBoss_)
        {
            pBoss_->SetIsCollapse(true);
        }
    }
}

void EnergyCore::Respawn()
{
    isDestroyed_ = false;
    
    // 注意：コライダーの有効化はStartSpawnAnimationで行うため、ここでは行わない
}

void EnergyCore::StartSpawnAnimation(const Vector3& targetPosition)
{
    // 出現アニメーションの初期化
    isSpawning_ = true;
    spawnTimer_ = 0.0f;
    shakeTimer_ = 0.0f;
    
    // 目標位置を設定
    spawnTargetPos_ = targetPosition;
    
    // 開始位置を地面の下に設定（Y軸-10.0f下）
    spawnStartPos_ = targetPosition;
    if (targetPosition.y >= 5.0f) spawnStartPos_.y = -1.0f;
    else if (targetPosition.y <= -5.0f) spawnStartPos_.y = -10.6f;
    
    
    // 初期位置に配置
    transform_.translate = spawnStartPos_;
    
    // コライダーを無効化（アニメーション完了後に有効化）
    if (collider_)
    {
        collider_->SetActive(false);
    }
}

void EnergyCore::UpdateSpawnAnimation()
{
    float deltaTime = FrameTimer::GetInstance()->GetDeltaTime();
    
    // タイマー更新
    spawnTimer_ += deltaTime;
    shakeTimer_ += deltaTime;
    
    // アニメーションの進行度（0.0〜1.0）
    float t = spawnTimer_ / spawnDuration_;
    t = std::min(t, 1.0f);
    
    // イージング（緩やかな加速と減速）
    float easedT = t * t * (3.0f - 2.0f * t);
    
    // 基本位置の線形補間
    Vector3 basePos;
    basePos.x = spawnStartPos_.x + (spawnTargetPos_.x - spawnStartPos_.x) * easedT;
    basePos.y = spawnStartPos_.y + (spawnTargetPos_.y - spawnStartPos_.y) * easedT;
    basePos.z = spawnStartPos_.z + (spawnTargetPos_.z - spawnStartPos_.z) * easedT;
    
    // シェーク効果を追加
    Vector3 shakeOffset = ApplyShake(shakeAmplitude_, shakeDuration_);
    
    // 最終位置を設定
    transform_.translate = basePos + shakeOffset;
    
    // アニメーション完了チェック
    if (t >= 1.0f)
    {
        isSpawning_ = false;
        transform_.translate = spawnTargetPos_;  // 最終位置に固定
        
        // コライダーを有効化
        if (collider_)
        {
            collider_->SetActive(true);
        }
    }
}

Vector3 EnergyCore::ApplyShake(const Vector3& amplitude, float duration)
{
    // シェークが終了している場合
    if (shakeTimer_ >= duration)
    {
        return Vector3(0.0f, 0.0f, 0.0f);
    }
    
    // シェークの減衰（時間経過とともに減少）
    float decay = 1.0f - (shakeTimer_ / duration);
    
    // 各軸で異なる周波数でsin波を生成
    float shakeX = std::sin(shakeTimer_ * 100.0f) * amplitude.x * decay;
    float shakeY = std::sin(shakeTimer_ * 35.0f) * amplitude.y * decay;
    float shakeZ = std::sin(shakeTimer_ * 80.0f) * amplitude.z * decay;
    
    return Vector3(shakeX, shakeY, shakeZ);
}