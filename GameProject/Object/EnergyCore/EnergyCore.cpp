#include "EnergyCore.h"
#include "Object3d.h"
#include "CollisionManager.h"
#include "Model.h"
#include "FrameTimer.h"
#include "GPUParticle.h"
#include "../Boss/Boss.h"
#include "../../Collision/EnergyCoreCollider.h"
#include "../../Collision/CollisionTypeIdDef.h"
#include "../../Bar3d/Bar3d.h"
#include "Camera.h"
#include <cmath>
#include <algorithm>

#include "Audio.h"

#ifdef _DEBUG
#include "ImGui.h"
#endif

EnergyCore::EnergyCore()
{
}

EnergyCore::~EnergyCore()
{
}

void EnergyCore::Initialize(Boss* boss, Camera* camera)
{
    pBoss_ = boss;
    pCamera_ = camera;
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

    // HPバーの初期化
    hpBar_ = std::make_unique<Bar3d>();
    Color hpColor = 0xff0000ff;  // 赤色
    Color bgColor = 0x000000cc;  // 半透明の黒
    hpBar_->Initialize(hpColor, bgColor);
    hpBar_->SetMaxValue(maxHp_);
    hpBar_->SetCurrentValue(hp_);
    hpBar_->SetSize(Vector2(80.0f, 12.0f));  // HPバーのサイズ
    hpBar_->Display(false);  // 初期状態では非表示

    // SEの読み込み
    hitVH_ = Audio::GetInstance()->LoadWaveFile("enemyhit.wav");

    // パーティクルエミッターの初期化
    InitParticleEmitter();
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

        // パーティクルエミッターの位置を更新（シェークも考慮）
        if (emitterManager_)
        {
            // エミッターの位置をコライダーと同じ位置に更新
            Vector3 emitterPos = shakeTransform.translate + collider_->GetOffset();
            emitterManager_->SetEmitterPosition(emitterName_, emitterPos);

            // エミッターマネージャーの更新
            emitterManager_->Update();
        }

        // HPバーの更新
        if (hpBar_ && pCamera_)
        {
            // HPバーの位置を更新（エネルギーコアの上に表示）
            Vector2 screenPos = Bar3d::GetHeadUpPositionOnScreen(
                shakeTransform.translate,
                Vector2(80.0f, 8.0f),
                *pCamera_,
                8.0f  // Y軸オフセット
            );
            hpBar_->SetPosition(screenPos);
            hpBar_->SetCurrentValue(hp_);
            hpBar_->Update();
        }
    } else
    {
        // 破壊されたらエミッターを無効化
        if (emitterManager_)
        {
            emitterManager_->SetEmitterActive(emitterName_, false);
        }
        
        // 破壊されたらHPバーも非表示
        if (hpBar_)
        {
            hpBar_->Display(false);
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

void EnergyCore::Draw2d()
{
    if (hpBar_)
    {
        hpBar_->Draw2d();
    }
}

void EnergyCore::DrawImGui()
{
#ifdef _DEBUG
    // タブバーを使用して情報を整理
    if (ImGui::BeginTabBar("EnergyCoreTabs"))
    {
        // ===== Overviewタブ =====
        if (ImGui::BeginTabItem("Overview"))
        {
            // 状態表示
            ImVec4 statusColor = isDestroyed_ ? ImVec4(0.8f, 0.2f, 0.2f, 1.0f) : ImVec4(0.2f, 0.8f, 0.2f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, statusColor);
            ImGui::Text("Status: %s", isDestroyed_ ? "DESTROYED" : "ACTIVE");
            ImGui::PopStyleColor();

            ImGui::Spacing();

            // HP表示（色付きバー）
            float hpRatio = maxHp_ > 0 ? hp_ / maxHp_ : 0.0f;
            ImVec4 hpBarColor = hpRatio > 0.5f ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) :
                hpRatio > 0.25f ? ImVec4(0.8f, 0.8f, 0.2f, 1.0f) :
                ImVec4(0.8f, 0.2f, 0.2f, 1.0f);

            ImGui::Text("Health Points");
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, hpBarColor);
            ImGui::ProgressBar(hpRatio, ImVec2(-1, 20), "");
            ImGui::PopStyleColor();

            ImGui::Text("HP: %.0f / %.0f (%.1f%%)", hp_, maxHp_, hpRatio * 100.0f);

            // HP調整
            ImGui::SetNextItemWidth(150);
            if (ImGui::DragFloat("Current HP", &hp_, 1.0f, 0.0f, maxHp_))
            {
                if (hp_ <= 0.0f && !isDestroyed_)
                {
                    Destroy();
                }
            }

            ImGui::SetNextItemWidth(150);
            if (ImGui::DragFloat("Max HP", &maxHp_, 1.0f, 1.0f, 1000.0f))
            {
                hp_ = std::min(hp_, maxHp_);
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // クイックアクション
            ImGui::Text("Quick Actions");

            if (isDestroyed_)
            {
                if (ImGui::Button("Respawn", ImVec2(100, 0)))
                {
                    Respawn();
                }
            } else
            {
                if (ImGui::Button("Destroy", ImVec2(100, 0)))
                {
                    Destroy();
                }

                ImGui::SameLine();
                if (ImGui::Button("Damage (20)", ImVec2(100, 0)))
                {
                    Damage(20.0f);
                }

                ImGui::SameLine();
                if (ImGui::Button("Heal Full", ImVec2(100, 0)))
                {
                    hp_ = maxHp_;
                }
            }

            ImGui::EndTabItem();
        }

        // ===== Transformタブ =====
        if (ImGui::BeginTabItem("Transform"))
        {
            ImGui::Text("Position, Rotation, and Scale");
            ImGui::Spacing();

            // Position
            ImGui::Text("Position");
            ImGui::SetNextItemWidth(200);
            ImGui::DragFloat3("##Position", &transform_.translate.x, 0.1f);

            // Rotation
            ImGui::Text("Rotation");
            ImGui::SetNextItemWidth(200);
            ImGui::DragFloat3("##Rotation", &transform_.rotate.x, 0.1f);

            // Scale
            ImGui::Text("Scale");
            ImGui::SetNextItemWidth(200);
            ImGui::DragFloat3("##Scale", &transform_.scale.x, 0.1f, 0.1f, 10.0f);

            ImGui::Spacing();

            // Reset buttons
            if (ImGui::Button("Reset Position"))
            {
                transform_.translate = Vector3(0.0f, 0.0f, 0.0f);
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset Rotation"))
            {
                transform_.rotate = Vector3(0.0f, 0.0f, 0.0f);
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset Scale"))
            {
                transform_.scale = Vector3(1.0f, 1.0f, 1.0f);
            }

            ImGui::EndTabItem();
        }

        // ===== Effectsタブ =====
        if (ImGui::BeginTabItem("Effects"))
        {
            // ダメージフラッシュ
            ImGui::Text("Damage Flash Effect");
            ImGui::Spacing();

            ImGui::SetNextItemWidth(150);
            ImGui::DragFloat("Flash Duration", &flashDuration_, 0.01f, 0.01f, 1.0f);

            if (ImGui::Button("Test Flash", ImVec2(120, 0)))
            {
                StartDamageFlash(flashDuration_);
            }

            if (isFlashing_)
            {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[Flashing]");
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // ダメージシェーク
            ImGui::Text("Damage Shake Effect");
            ImGui::Spacing();

            ImGui::SetNextItemWidth(200);
            ImGui::DragFloat3("Shake Amplitude", &damageShakeAmplitude_.x, 0.01f, 0.0f, 1.0f);

            ImGui::SetNextItemWidth(150);
            ImGui::DragFloat("Shake Duration", &damageShakeDuration_, 0.01f, 0.01f, 2.0f);

            if (ImGui::Button("Test Shake", ImVec2(120, 0)))
            {
                StartDamageShake(damageShakeDuration_);
            }

            if (isDamageShaking_)
            {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[Shaking]");
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // パーティクルエフェクト情報
            ImGui::Text("Particle Effects");
            ImGui::Spacing();

            if (emitterManager_)
            {
                ImGui::Text("Emitter: %s", emitterName_.c_str());
                ImGui::Text("Status: %s",
                    (isDestroyed_ || isSpawning_) ? "Inactive" : "Active");
            } else
            {
                ImGui::Text("No particle emitter");
            }

            ImGui::EndTabItem();
        }

        // ===== Debugタブ =====
        if (ImGui::BeginTabItem("Debug"))
        {
            // Collider設定
            if (collider_)
            {
                ImGui::Text("Collider Settings");
                ImGui::Spacing();

                Vector3 size = collider_->GetSize();
                ImGui::SetNextItemWidth(200);
                if (ImGui::DragFloat3("Collider Size", &size.x, 0.1f, 0.1f, 100.0f))
                {
                    collider_->SetSize(size);
                }

                Vector3 offset = collider_->GetOffset();
                ImGui::SetNextItemWidth(200);
                if (ImGui::DragFloat3("Collider Offset", &offset.x, 0.1f))
                {
                    collider_->SetOffset(offset);
                }

                bool isActive = collider_->IsActive();
                if (ImGui::Checkbox("Collider Active", &isActive))
                {
                    collider_->SetActive(isActive);
                }
            } else
            {
                ImGui::Text("No collider attached");
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // 内部状態
            ImGui::Text("Internal States");
            ImGui::Spacing();

            ImGui::Text("Flags:");
            ImGui::BulletText("isDestroyed: %s", isDestroyed_ ? "true" : "false");
            ImGui::BulletText("isSpawning: %s", isSpawning_ ? "true" : "false");
            ImGui::BulletText("isFlashing: %s", isFlashing_ ? "true" : "false");
            ImGui::BulletText("isDamageShaking: %s", isDamageShaking_ ? "true" : "false");

            ImGui::Spacing();

            ImGui::Text("Timers:");
            ImGui::BulletText("spawnTimer: %.2f / %.2f", spawnTimer_, spawnDuration_);
            ImGui::BulletText("flashTimer: %.2f / %.2f", flashTimer_, flashDuration_);
            ImGui::BulletText("damageShakeTimer: %.2f / %.2f", damageShakeTimer_, damageShakeDuration_);

            ImGui::Spacing();

            ImGui::Text("Spawn Animation:");
            ImGui::BulletText("Start Pos: (%.1f, %.1f, %.1f)",
                spawnStartPos_.x, spawnStartPos_.y, spawnStartPos_.z);
            ImGui::BulletText("Target Pos: (%.1f, %.1f, %.1f)",
                spawnTargetPos_.x, spawnTargetPos_.y, spawnTargetPos_.z);

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
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

    Audio::GetInstance()->Play(hitVH_);

    // HPバーを表示（3秒間）
    if (hpBar_)
    {
        hpBar_->Display(3.0f);
    }

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

        // パーティクルエミッターを無効化
        if (emitterManager_)
        {
            emitterManager_->SetEmitterActive(emitterName_, false);
        }

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

    // パーティクルエミッターを有効化
    if (emitterManager_)
    {
        emitterManager_->SetEmitterActive(emitterName_, true);
    }

    // HPバーをリセット
    if (hpBar_)
    {
        hpBar_->SetCurrentValue(hp_);
        hpBar_->Display(false);  // 再生成時は非表示
    }

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
    if (targetPosition.y > -5.0f)  // 高い地形（0付近）
    {
        spawnStartPos_.y = -5.6f;
    } else  // 低い地形（-10付近
    {
        spawnStartPos_.y = -15.6f;
    }


    // 初期位置に配置
    transform_.translate = spawnStartPos_;

    // コライダーを無効化（アニメーション完了後に有効化）
    if (collider_)
    {
        collider_->SetActive(false);
    }

    // アニメーション中はパーティクルを無効化
    if (emitterManager_)
    {
        emitterManager_->SetEmitterActive(emitterName_, false);
    }
}

void EnergyCore::InitParticleEmitter()
{
    // パーティクルエフェクトの初期化
    emitterManager_ = std::make_unique<EmitterManager>(GPUParticle::GetInstance());

    // ユニークなエミッター名を生成（ポインタのアドレスを使用）
    emitterName_ = "EnergyCore_" + std::to_string(reinterpret_cast<uintptr_t>(this));

    // BoxEmitterを作成（コライダーと同じサイズ）
    Vector3 emitterSize = collider_->GetSize();
    emitterManager_->CreateBoxEmitter(
        emitterName_,
        transform_.translate + collider_->GetOffset(),  // コライダーと同じ位置
        emitterSize,                                     // コライダーと同じサイズ  
        Vector3(0.0f, 0.0f, 0.0f),                      // 回転なし
        30,                                              // パーティクル数
        0.2f                                             // 生成頻度（0.2秒ごと）
    );

    // パーティクルの色を赤系に設定
    Vector4 startColor(1.0f, 0.2f, 0.0f, 1.0f);  // 明るい赤
    Vector4 endColor(0.8f, 0.0f, 0.0f, 0.0f);    // 暗い赤（フェードアウト）
    emitterManager_->SetEmitterColors(emitterName_, startColor, endColor);

    // パーティクルの速度を設定（ゆっくり上昇）
    emitterManager_->SetEmitterVelocityRange(
        emitterName_,
        Vector2(-0.05f, 0.05f),   // X軸の速度範囲
        Vector2(0.05f, 0.3f),    // Y軸の速度範囲（上昇）
        Vector2(-0.05f, 0.05f)    // Z軸の速度範囲
    );

    // パーティクルのサイズを設定（小さめ）
    emitterManager_->SetEmitterScaleRange(
        emitterName_,
        Vector2(0.3f, 0.3f),    // X軸のスケール範囲
        Vector2(0.3f, 0.3f)     // Y軸のスケール範囲
    );

    // パーティクルの寿命を設定
    emitterManager_->SetEmitterLifeTimeRange(
        emitterName_,
        Vector2(1.0f, 2.0f)     // 1〜2秒の寿命
    );
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

        // パーティクルエミッターを有効化
        if (emitterManager_)
        {
            emitterManager_->SetEmitterActive(emitterName_, true);
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