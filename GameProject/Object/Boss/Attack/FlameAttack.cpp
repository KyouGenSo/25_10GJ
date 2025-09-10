#include "FlameAttack.h"
#include "EmitterManager.h"
#include "GPUParticle.h"
#include "../../../Collision/BossAttackCollider.h"
#include "CollisionManager.h"
#include "FrameTimer.h"
#include "../../Player/Player.h"
#include "../../../Terrain/Terrain.h"
#include "../../../Collision/CollisionTypeIdDef.h"

#ifdef _DEBUG
#include "ImGui.h"
#endif

FlameAttack::FlameAttack()
{
}

FlameAttack::~FlameAttack()
{
}

void FlameAttack::Initialize(Player* player, Terrain* terrain)
{
    player_ = player;
    terrain_ = terrain;
    
    // エリアの初期化
    InitializeFlameAreas();
    
    // パーティクルエフェクトの初期化
    InitializeFlameParticles();
    
    Reset();
}

void FlameAttack::Finalize()
{
    // コライダーを削除
    for (auto& area : flameAreas_)
    {
        if (area.collider)
        {
            CollisionManager::GetInstance()->RemoveCollider(area.collider.get());
        }
    }
}

void FlameAttack::Update()
{
    if (!isActive_) return;
    
    stateTimer_ += FrameTimer::GetInstance()->GetDeltaTime();
    
    switch (state_)
    {
    case AttackState::Warning:
        UpdateWarningState();
        break;
    case AttackState::Attack:
        UpdateAttackState();
        break;
    default:
        break;
    }
    
    // パーティクルマネージャーの更新
    if (emitterManager_)
    {
        emitterManager_->Update();
    }
}

void FlameAttack::Draw()
{
    // パーティクルの描画はGPUParticleが自動的に行う
}

void FlameAttack::DrawImGui()
{
#ifdef _DEBUG
    if (ImGui::TreeNode("FlameAttack"))
    {
        // 攻撃状態表示
        const char* stateStr = "Unknown";
        switch (state_)
        {
        case AttackState::Idle: stateStr = "Idle"; break;
        case AttackState::Warning: stateStr = "Warning"; break;
        case AttackState::Attack: stateStr = "Attack"; break;
        case AttackState::Cooldown: stateStr = "Cooldown"; break;
        }
        ImGui::Text("State: %s", stateStr);
        ImGui::Text("State Timer: %.2f", stateTimer_);
        ImGui::Text("Active: %s", isActive_ ? "Yes" : "No");
        
        ImGui::Separator();
        
        // タイミングパラメータ
        if (ImGui::TreeNode("Timing Parameters"))
        {
            ImGui::DragFloat("Warning Duration", &warningDuration_, 0.01f, 0.1f, 5.0f);
            ImGui::DragFloat("Warning Fade Time", &warningFadeTime_, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Flame Duration", &flameDuration_, 0.01f, 0.1f, 5.0f);
            ImGui::TreePop();
        }
        
        // ダメージとサイズ
        if (ImGui::TreeNode("Damage & Size"))
        {
            ImGui::DragFloat("Flame Damage", &flameDamage_, 1.0f, 0.0f, 100.0f);
            ImGui::DragFloat("Flame Height", &flameHeight_, 0.1f, 1.0f, 20.0f);
            ImGui::TreePop();
        }
        
        // パーティクル設定
        if (ImGui::TreeNode("Particle Settings"))
        {
            ImGui::DragInt("Particle Count", &particleCount_, 1, 10, 500);
            ImGui::DragFloat("Particle Frequency", &particleFrequency_, 0.001f, 0.001f, 1.0f);
            ImGui::TreePop();
        }
        
        // 警告時パーティクル設定
        if (ImGui::TreeNode("Warning Particle"))
        {
            ImGui::ColorEdit4("Warning Color", &warningColor_.x);
            ImGui::DragFloat2("Warning Vel X Range", &warningVelX_.x, 0.01f);
            ImGui::DragFloat2("Warning Vel Y Range", &warningVelY_.x, 0.01f);
            ImGui::DragFloat2("Warning Vel Z Range", &warningVelZ_.x, 0.01f);
            ImGui::DragFloat2("Warning Scale", &warningScale_.x, 0.1f, 0.1f, 10.0f);
            ImGui::DragFloat2("Warning LifeTime", &warningLifeTime_.x, 0.01f, 0.01f, 5.0f);
            ImGui::TreePop();
        }
        
        // 攻撃時パーティクル設定
        if (ImGui::TreeNode("Attack Particle"))
        {
            ImGui::ColorEdit4("Flame Start Color", &flameStartColor_.x);
            ImGui::ColorEdit4("Flame End Color", &flameEndColor_.x);
            ImGui::DragFloat2("Flame Vel X Range", &flameVelX_.x, 0.01f);
            ImGui::DragFloat2("Flame Vel Y Range", &flameVelY_.x, 0.01f);
            ImGui::DragFloat2("Flame Vel Z Range", &flameVelZ_.x, 0.01f);
            ImGui::DragFloat2("Flame Scale", &flameScale_.x, 0.1f, 0.1f, 20.0f);
            ImGui::DragFloat2("Flame LifeTime", &flameLifeTime_.x, 0.01f, 0.01f, 5.0f);
            ImGui::TreePop();
        }
        
        // エリア情報
        if (ImGui::TreeNode("Flame Areas"))
        {
            for (int i = 0; i < 4; i++)
            {
                if (ImGui::TreeNode(("Area " + std::to_string(i)).c_str()))
                {
                    // 基本情報
                    ImGui::Text("Base Position: (%.1f, %.1f, %.1f)", 
                        flameAreas_[i].position.x, 
                        flameAreas_[i].position.y, 
                        flameAreas_[i].position.z);
                    ImGui::Text("Size: (%.1f, %.1f, %.1f)", 
                        flameAreas_[i].size.x, 
                        flameAreas_[i].size.y, 
                        flameAreas_[i].size.z);
                    ImGui::Text("Height: %.1f", flameAreas_[i].height);
                    
                    // コライダー情報
                    if (ImGui::TreeNode("Collider Transform"))
                    {
                        ImGui::Text("Active: %s", 
                            (flameAreas_[i].collider && flameAreas_[i].collider->IsActive()) ? "Yes" : "No");
                        if (flameAreas_[i].collider)
                        {
                            ImGui::Text("Damage: %.1f", flameAreas_[i].collider->GetDamage());
                            const Vector3& colliderSize = flameAreas_[i].collider->GetSize();
                            ImGui::Text("Collider Size: (%.1f, %.1f, %.1f)",
                                colliderSize.x, colliderSize.y, colliderSize.z);
                        }
                        ImGui::Text("Translate: (%.1f, %.1f, %.1f)",
                            flameAreas_[i].transform.translate.x,
                            flameAreas_[i].transform.translate.y,
                            flameAreas_[i].transform.translate.z);
                        ImGui::Text("Rotate: (%.1f, %.1f, %.1f)",
                            flameAreas_[i].transform.rotate.x,
                            flameAreas_[i].transform.rotate.y,
                            flameAreas_[i].transform.rotate.z);
                        ImGui::Text("Scale: (%.1f, %.1f, %.1f)",
                            flameAreas_[i].transform.scale.x,
                            flameAreas_[i].transform.scale.y,
                            flameAreas_[i].transform.scale.z);
                        ImGui::TreePop();
                    }
                    
                    // パーティクルエミッター情報
                    if (ImGui::TreeNode("Particle Emitter"))
                    {
                        ImGui::Text("Emitter Name: %s", flameAreas_[i].emitterName.c_str());
                        Vector3 emitterPos = flameAreas_[i].position;
                        emitterPos.y = flameAreas_[i].height - 8.0f;
                        ImGui::Text("Emitter Position: (%.1f, %.1f, %.1f)",
                            emitterPos.x,
                            emitterPos.y,
                            emitterPos.z);
                        ImGui::Text("Emitter Box Size: (%.1f, %.1f, %.1f)",
                            flameAreas_[i].size.x,
                            1.0f,
                            flameAreas_[i].size.z);
                        ImGui::TreePop();
                    }
                    
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
        
        // テスト実行ボタン
        if (ImGui::Button("Test Execute"))
        {
            if (state_ == AttackState::Idle)
            {
                Execute();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
        {
            Reset();
        }
        
        ImGui::TreePop();
    }
#endif
}

void FlameAttack::Execute()
{
    if (state_ != AttackState::Idle) return;
    
    // 警告状態へ遷移
    state_ = AttackState::Warning;
    stateTimer_ = 0.0f;
    isActive_ = true;
    
    // パーティクル設定を更新（ImGuiで変更された値を反映）
    for (int i = 0; i < 4; i++)
    {
        auto& area = flameAreas_[i];
        Vector3 emitterPos = area.position;
        emitterPos.y = area.height;
        
        // エミッターのパラメータを更新
        emitterManager_->UpdateBoxEmitter(
            area.emitterName,
            emitterPos,
            Vector3(area.size.x, 1.0f, area.size.z),
            Vector3(0.0f, 0.0f, 0.0f),
            particleCount_,
            particleFrequency_
        );
        
        // コライダーのダメージを更新
        if (area.collider)
        {
            area.collider->SetDamage(flameDamage_);
            area.collider->SetSize(Vector3(area.size.x, flameHeight_, area.size.z));
        }
    }
}

bool FlameAttack::IsFinished() const
{
    return state_ == AttackState::Idle;
}

void FlameAttack::Reset()
{
    state_ = AttackState::Idle;
    stateTimer_ = 0.0f;
    isActive_ = false;
    
    // 全てのパーティクルとコライダーを無効化
    DeactivateFlames();
}

void FlameAttack::InitializeFlameAreas()
{
    // 左上エリア（高い）
    flameAreas_[0].position = Vector3(50.0f, 0.0f, 50.0f);
    flameAreas_[0].size = Vector3(100.0f, flameHeight_, 100.0f);
    flameAreas_[0].height = 0.0f;
    
    // 右下エリア（高い）
    flameAreas_[1].position = Vector3(150.0f, 0.0f, 150.0f);
    flameAreas_[1].size = Vector3(100.0f, flameHeight_, 100.0f);
    flameAreas_[1].height = 0.0f;
    
    // 左下エリア（低い）
    flameAreas_[2].position = Vector3(50.0f, -10.0f, 150.0f);
    flameAreas_[2].size = Vector3(100.0f, flameHeight_, 100.0f);
    flameAreas_[2].height = -10.0f;
    
    // 右上エリア（低い）
    flameAreas_[3].position = Vector3(150.0f, -10.0f, 50.0f);
    flameAreas_[3].size = Vector3(100.0f, flameHeight_, 100.0f);
    flameAreas_[3].height = -10.0f;
    
    // 各エリアのコライダーを初期化
    for (int i = 0; i < 4; i++)
    {
        auto& area = flameAreas_[i];
        
        // Transformの設定
        area.transform.translate = area.position;
        //area.transform.translate.y = area.height;
        area.transform.rotate = Vector3(0.0f, 0.0f, 0.0f);
        area.transform.scale = Vector3(1.0f, 1.0f, 1.0f);
        
        // コライダーの初期化
        area.collider = std::make_unique<BossAttackCollider>(player_);
        area.collider->SetTransform(&area.transform);
        area.collider->SetSize(area.size);
        area.collider->SetDamage(flameDamage_);
        area.collider->SetActive(false);
        CollisionManager::GetInstance()->AddCollider(area.collider.get());
    }
}

void FlameAttack::InitializeFlameParticles()
{
    emitterManager_ = std::make_unique<EmitterManager>(GPUParticle::GetInstance());
    
    for (int i = 0; i < 4; i++)
    {
        auto& area = flameAreas_[i];
        area.emitterName = "FlameArea_" + std::to_string(i);
        
        // BoxEmitterで炎を生成
        Vector3 emitterPos = area.position;
        emitterPos.y = area.height + 5.0f;  // 地面の高さに配置
        
        emitterManager_->CreateBoxEmitter(
            area.emitterName,
            emitterPos,
            Vector3(area.size.x, 1.0f, area.size.z),  // 地面に薄く配置
            Vector3(0.0f, 0.0f, 0.0f),
            particleCount_,    // パーティクル数
            particleFrequency_   // 生成頻度
        );
        
        // 初期は非アクティブ
        emitterManager_->SetEmitterActive(area.emitterName, false);
    }
}

void FlameAttack::UpdateWarningState()
{
    // 警告パーティクル（地面が赤く光る演出）
    if (stateTimer_ < 0.1f)  // 最初だけ実行
    {
        for (int i = 0; i < 4; i++)
        {
            auto& area = flameAreas_[i];
            
            // 警告用の薄い赤色パーティクル
            emitterManager_->SetEmitterColor(
                area.emitterName,
                warningColor_
            );
            
            // ゆっくり上昇
            emitterManager_->SetEmitterVelocityRange(
                area.emitterName,
                warningVelX_,    // X方向
                warningVelY_,    // Y方向（ゆっくり上昇）
                warningVelZ_     // Z方向
            );

            // スケール設定
            emitterManager_->SetEmitterScaleRange(
                area.emitterName,
                warningScale_,    // X方向のスケール範囲
                warningScale_     // Y方向のスケール範囲
            );

            // ライフタイム設定
            emitterManager_->SetEmitterLifeTimeRange(
                area.emitterName,
                warningLifeTime_     // パーティクルの寿命
            );
            
            emitterManager_->SetEmitterActive(area.emitterName, true);
        }
    }
    
    // 警告終了の warningFadeTime_ 秒前にパーティクルを停止
    float fadeStartTime = warningDuration_ - warningFadeTime_;
    if (stateTimer_ >= fadeStartTime && stateTimer_ < fadeStartTime + 0.05f)
    {
        for (int i = 0; i < 4; i++)
        {
            emitterManager_->SetEmitterActive(flameAreas_[i].emitterName, false);
        }
    }
    
    // 警告時間が経過したら攻撃状態へ
    if (stateTimer_ >= warningDuration_)
    {
        state_ = AttackState::Attack;
        stateTimer_ = 0.0f;
        ActivateFlames();
    }
}

void FlameAttack::UpdateAttackState()
{
    for (int i = 0; i < 4; i++)
    {
        emitterManager_->SetEmitterActive(flameAreas_[i].emitterName, true);
    }

    // 攻撃時間が経過したら終了
    if (stateTimer_ >= flameDuration_)
    {
        Reset();
    }
}

void FlameAttack::ActivateFlames()
{
    for (int i = 0; i < 4; i++)
    {
        auto& area = flameAreas_[i];
        
        // 炎の色を本来の色に戻す
        emitterManager_->SetEmitterColors(
            area.emitterName,
            flameStartColor_,  // 開始色
            flameEndColor_     // 終了色
        );
        
        // 炎の勢いを強くする
        emitterManager_->SetEmitterVelocityRange(
            area.emitterName,
            flameVelX_,   // X方向
            flameVelY_,   // Y方向（勢いよく上昇）
            flameVelZ_    // Z方向
        );

        // スケール設定
        emitterManager_->SetEmitterScaleRange(
            area.emitterName,
            flameScale_,    // X方向のスケール範囲
            flameScale_     // Y方向のスケール範囲
        );

        // ライフタイム設定
        emitterManager_->SetEmitterLifeTimeRange(
            area.emitterName,
            flameLifeTime_     // パーティクルの寿命
        );
        
        // コライダーを有効化
        if (area.collider)
        {
            area.collider->SetActive(true);
        }
    }
}

void FlameAttack::DeactivateFlames()
{
    for (int i = 0; i < 4; i++)
    {
        auto& area = flameAreas_[i];
        
        // パーティクルを無効化
        emitterManager_->SetEmitterActive(area.emitterName, false);
        
        // コライダーを無効化
        if (area.collider)
        {
            area.collider->SetActive(false);
        }
    }
}