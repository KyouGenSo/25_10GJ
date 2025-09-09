#include "EnergyCore.h"
#include "Object3d.h"
#include "CollisionManager.h"
#include "Model.h"
#include "FrameTimer.h"
#include "../Boss/Boss.h"
#include "../../Collision/EnergyCoreCollider.h"
#include "../../Collision/CollisionTypeIdDef.h"
#include <cmath>
#include <algorithm>

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
    hp_ = maxHp_;  // HPを最大値に初期化
    
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
        
        // ダメージフラッシュの更新
        UpdateDamageFlash();
        
        // ダメージシェークの更新
        UpdateDamageShake();
        
        // シェークオフセットを適用
        Transform shakeTransform = transform_;
        shakeTransform.translate = transform_.translate + damageShakeOffset_;
        
        model_->SetTransform(shakeTransform);
        model_->Update();
        
        // コライダーの更新（元の位置で更新）
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
    
    // HP表示
    ImGui::Text("HP: %.1f / %.1f", hp_, maxHp_);
    ImGui::ProgressBar(hp_ / maxHp_, ImVec2(0, 0), "");
    
    // HP調整
    if (ImGui::DragFloat("HP", &hp_, 1.0f, 0.0f, maxHp_))
    {
        // HPが0になったら破壊
        if (hp_ <= 0.0f && !isDestroyed_)
        {
            Destroy();
        }
    }
    if (ImGui::DragFloat("Max HP", &maxHp_, 1.0f, 1.0f, 1000.0f))
    {
        // 最大HPが変更されたら、現在のHPも調整
        hp_ = std::min(hp_, maxHp_);
    }
    
    ImGui::Separator();
    
    // フラッシュ設定
    ImGui::Text("Flash Settings");
    if (ImGui::DragFloat("Flash Duration", &flashDuration_, 0.01f, 0.01f, 1.0f))
    {
        // フラッシュ時間の調整
    }
    if (ImGui::Button("Test Flash"))
    {
        StartDamageFlash(flashDuration_);
    }
    
    ImGui::Separator();
    
    // ダメージシェーク設定
    ImGui::Text("Damage Shake Settings");
    ImGui::DragFloat3("Shake Amplitude", &damageShakeAmplitude_.x, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Shake Duration", &damageShakeDuration_, 0.01f, 0.01f, 2.0f);
    if (ImGui::Button("Test Damage Shake"))
    {
        StartDamageShake(damageShakeDuration_);
    }
    
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
    ImGui::SameLine();
    if (ImGui::Button("Damage (20)"))
    {
        Damage(20.0f);
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

void EnergyCore::Damage(float damage)
{
    if (isDestroyed_) return;
    
    // ダメージ処理
    hp_ -= damage;
    hp_ = std::max<float>(hp_, 0.0f);
    
    // ダメージフラッシュを開始
    StartDamageFlash();
    
    // ダメージシェークを開始
    StartDamageShake();
    
    // HPが0になったら破壊
    if (hp_ <= 0.0f)
    {
        Destroy();
    }
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
    hp_ = maxHp_;  // HPを最大値にリセット
    
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

void EnergyCore::StartDamageFlash(float duration)
{
    if (isDestroyed_) return;
    
    // 現在の色を保存（まだフラッシュ中でない場合のみ）
    if (model_ && !isFlashing_)
    {
        originalColor_ = model_->GetMaterialColor();
    }
    
    // フラッシュを開始
    isFlashing_ = true;
    flashTimer_ = 0.0f;
    flashDuration_ = duration;
    
    // モデルを白色に変更
    if (model_)
    {
        model_->SetMaterialColor(flashColor_);
    }
}

void EnergyCore::UpdateDamageFlash()
{
    if (!isFlashing_) return;
    
    // タイマー更新
    flashTimer_ += FrameTimer::GetInstance()->GetDeltaTime();
    
    // フラッシュ時間が経過したら元の色に戻す
    if (flashTimer_ >= flashDuration_)
    {
        isFlashing_ = false;
        flashTimer_ = 0.0f;
        
        // 元の色に戻す
        if (model_)
        {
            model_->SetMaterialColor(originalColor_);
        }
    }
}

void EnergyCore::StartDamageShake(float duration)
{
    if (isDestroyed_) return;
    
    isDamageShaking_ = true;
    damageShakeTimer_ = 0.0f;
    damageShakeDuration_ = duration;
}

void EnergyCore::UpdateDamageShake()
{
    if (!isDamageShaking_) 
    {
        damageShakeOffset_ = Vector3(0.0f, 0.0f, 0.0f);
        return;
    }
    
    float deltaTime = FrameTimer::GetInstance()->GetDeltaTime();
    damageShakeTimer_ += deltaTime;
    
    // シェークが終了したら
    if (damageShakeTimer_ >= damageShakeDuration_)
    {
        isDamageShaking_ = false;
        damageShakeTimer_ = 0.0f;
        damageShakeOffset_ = Vector3(0.0f, 0.0f, 0.0f);
        return;
    }
    
    // シェークの減衰
    float decay = 1.0f - (damageShakeTimer_ / damageShakeDuration_);
    
    // 各軸で異なる周波数でsin波を生成（ApplyShakeと同じロジック）
    damageShakeOffset_.x = std::sin(damageShakeTimer_ * 150.0f) * damageShakeAmplitude_.x * decay;
    damageShakeOffset_.y = std::sin(damageShakeTimer_ * 45.0f) * damageShakeAmplitude_.y * decay;
    damageShakeOffset_.z = std::sin(damageShakeTimer_ * 120.0f) * damageShakeAmplitude_.z * decay;
}