#include "SpikeAttack.h"
#include "Object3d.h"
#include "../../../Collision/BossAttackCollider.h"
#include "CollisionManager.h"
#include "FrameTimer.h"
#include "../../Player/Player.h"
#include "../../../Terrain/Terrain.h"
#include "../../../Terrain/Block/Block.h"
#include "../../../Collision/CollisionTypeIdDef.h"
#include <algorithm>

#ifdef _DEBUG
#include "ImGui.h"
#endif

SpikeAttack::SpikeAttack()
{
}

SpikeAttack::~SpikeAttack()
{
}

void SpikeAttack::Initialize(Player* player, Terrain* terrain)
{
    player_ = player;
    terrain_ = terrain;
    
    spike_ = std::make_unique<Spike>();
    
    // 棘モデルの初期化
    spike_->model = std::make_unique<Object3d>();
    spike_->model->Initialize();
    spike_->model->SetModel("toge.gltf");
    spike_->transform.translate = Vector3(0.0f, 0.0f, 0.0f);
    spike_->transform.rotate = Vector3(0.0f, 0.0f, 0.0f);
    spike_->transform.scale = Vector3(3.0f, 3.0f, 3.0f);
    spike_->model->SetTransform(spike_->transform);
    
    // 警告マークの初期化
    spike_->warningMark = std::make_unique<Object3d>();
    spike_->warningMark->Initialize();
    spike_->warningMark->SetModel("plane.obj");
    
    // 警告マークの色を赤に設定
    spike_->warningMark->SetMaterialColor(Vector4(1.0f, 0.0f, 0.0f, 0.8f));
    
    // コライダーの初期化
    spike_->collider = std::make_unique<BossAttackCollider>(player_);
    spike_->collider->SetTransform(&spike_->transform);
    spike_->collider->SetSize(Vector3(5.0f, kSpikeHeight, 5.0f));
    spike_->collider->SetDamage(kSpikeDamage);
    spike_->collider->SetActive(false);
    CollisionManager::GetInstance()->AddCollider(spike_->collider.get());
    
    Reset();
}

void SpikeAttack::Finalize()
{
    if (spike_ && spike_->collider)
    {
        CollisionManager::GetInstance()->RemoveCollider(spike_->collider.get());
    }
}

void SpikeAttack::Update()
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
    case AttackState::Cooldown:
        UpdateCooldownState();
        break;
    default:
        break;
    }

    spike_->model->Update();
    spike_->warningMark->Update();
}

void SpikeAttack::Draw()
{
    if (!isActive_ || !spike_) return;
    
    if (state_ == AttackState::Warning && spike_->warningMark)
    {
        // 警告マークの描画
        spike_->warningMark->Draw();
    }
    else if ((state_ == AttackState::Attack || state_ == AttackState::Cooldown) && spike_->model)
    {
        // 棘の描画
        spike_->model->Draw();
    }
}

void SpikeAttack::DrawImGui()
{
#ifdef _DEBUG
    if (ImGui::TreeNode("SpikeAttack"))
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
        
        // パラメータ表示
        ImGui::Text("Parameters:");
        ImGui::Text("  Warning Duration: %.1f", kWarningDuration);
        ImGui::Text("  Attack Duration: %.1f", kAttackDuration);
        ImGui::Text("  Cooldown Duration: %.1f", kCooldownDuration);
        ImGui::Text("  Spike Damage: %.1f", kSpikeDamage);
        ImGui::Text("  Spike Height: %.1f", kSpikeHeight);
        ImGui::Text("  Animation Speed: %.1f", kSpikeAnimationSpeed);
        
        ImGui::Separator();
        
        // 棘情報
        if (spike_)
        {
            ImGui::Text("Spike Info:");
            ImGui::Text("  Target Position: (%.1f, %.1f, %.1f)", 
                spike_->targetPosition.x, 
                spike_->targetPosition.y, 
                spike_->targetPosition.z);
            ImGui::Text("  Animation Timer: %.2f", spike_->animationTimer);
            ImGui::Text("  Is Active: %s", spike_->isActive ? "Yes" : "No");
            
            // トランスフォーム詳細
            if (ImGui::TreeNode("Transform Details"))
            {
                ImGui::Text("Translate: (%.1f, %.1f, %.1f)", 
                    spike_->transform.translate.x, 
                    spike_->transform.translate.y, 
                    spike_->transform.translate.z);
                ImGui::Text("Rotate: (%.1f, %.1f, %.1f)", 
                    spike_->transform.rotate.x, 
                    spike_->transform.rotate.y, 
                    spike_->transform.rotate.z);
                ImGui::Text("Scale: (%.1f, %.1f, %.1f)", 
                    spike_->transform.scale.x, 
                    spike_->transform.scale.y, 
                    spike_->transform.scale.z);
                ImGui::TreePop();
            }
            
            // コライダー情報
            if (ImGui::TreeNode("Collider Info"))
            {
                ImGui::Text("Active: %s", 
                    (spike_->collider && spike_->collider->IsActive()) ? "Yes" : "No");
                if (spike_->collider)
                {
                    ImGui::Text("Damage: %.1f", spike_->collider->GetDamage());
                    const Vector3& colliderSize = spike_->collider->GetSize();
                    ImGui::Text("Size: (%.1f, %.1f, %.1f)",
                        colliderSize.x, colliderSize.y, colliderSize.z);
                }
                ImGui::TreePop();
            }
        }
        
        ImGui::Separator();
        
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

void SpikeAttack::Execute()
{
    if (state_ != AttackState::Idle) return;
    
    // プレイヤーの足元のブロック位置を取得
    Vector3 blockPos = GetPlayerBlockPosition();
    
    // 棘を配置
    ActivateSpike(blockPos);
    
    // 警告状態へ遷移
    state_ = AttackState::Warning;
    stateTimer_ = 0.0f;
    isActive_ = true;
}

bool SpikeAttack::IsFinished() const
{
    return state_ == AttackState::Idle;
}

void SpikeAttack::Reset()
{
    state_ = AttackState::Idle;
    stateTimer_ = 0.0f;
    isActive_ = false;
    
    if (spike_)
    {
        spike_->isActive = false;
        spike_->animationTimer = 0.0f;
        if (spike_->collider)
        {
            spike_->collider->SetActive(false);
        }
    }
}

void SpikeAttack::UpdateWarningState()
{
    if (stateTimer_ >= kWarningDuration)
    {
        // 攻撃状態へ遷移
        state_ = AttackState::Attack;
        stateTimer_ = 0.0f;
        spike_->animationTimer = 0.0f;
        
        // コライダーを有効化
        if (spike_->collider)
        {
            spike_->collider->SetActive(true);
        }
    }
    else
    {
        // 警告マークの点滅アニメーション
        float alpha = 0.5f + 0.5f * std::sin(stateTimer_ * 10.0f);
        spike_->warningMark->SetMaterialColor(Vector4(1.0f, 0.0f, 0.0f, alpha));
    }
}

void SpikeAttack::UpdateAttackState()
{
    // 棘のアニメーション更新
    spike_->animationTimer += FrameTimer::GetInstance()->GetDeltaTime() * kSpikeAnimationSpeed;
    
    // 棘の出現アニメーション（地面から出てくる）
    float animProgress = std::min<float>(spike_->animationTimer, 1.0f);
    float yOffset = kSpikeHeight * animProgress;
    
    spike_->transform.translate = spike_->targetPosition;
    spike_->transform.translate.y += yOffset - kSpikeHeight;
    spike_->model->SetTransform(spike_->transform);
    spike_->collider->SetTransform(&spike_->transform);
    
    if (stateTimer_ >= kAttackDuration)
    {
        // クールダウン状態へ遷移
        state_ = AttackState::Cooldown;
        stateTimer_ = 0.0f;
        
        // コライダーを無効化
        if (spike_->collider)
        {
            spike_->collider->SetActive(false);
        }
    }
}

void SpikeAttack::UpdateCooldownState()
{
    // クールダウン開始時にブロックを白に変更
    static bool hasChangedColor = false;
    if (!hasChangedColor && terrain_)
    {
        try
        {
            auto block = terrain_->GetBlockAt(GetPlayerBlockPosition());
            block->SetColor(Block::Colors::White);
            hasChangedColor = true;
        }
        catch (...)
        {
            // 範囲外の場合は無視
        }
    }
    
    // 棘の引っ込みアニメーション
    float animProgress = 1.0f - std::min<float>(stateTimer_ / kCooldownDuration, 1.0f);
    float yOffset = kSpikeHeight * animProgress;
    
    //spike_->transform.translate = spike_->targetPosition;
    spike_->transform.translate.y += yOffset - kSpikeHeight;
    spike_->model->SetTransform(spike_->transform);
    
    if (stateTimer_ >= kCooldownDuration)
    {
        hasChangedColor = false;  // 次の攻撃のためにリセット
        // アイドル状態へ戻る
        Reset();
    }
}

Vector3 SpikeAttack::GetPlayerBlockPosition()
{
    if (!player_ || !terrain_) return Vector3(0, 0, 0);
    
    // プレイヤーの位置を取得
    Vector3 playerPos = player_->GetTransform().translate;
    
    // ブロックのグリッド位置に変換
    float blockX = std::floor(playerPos.x / Block::kScale) * Block::kScale + Block::kScale * 0.5f;
    float blockZ = std::floor(playerPos.z / Block::kScale) * Block::kScale + Block::kScale * 0.5f;
    
    // Y座標は地形から取得
    float blockY = 0.0f;
    try
    {
        blockY = terrain_->GetMaxYAt(blockX, blockZ);
    }
    catch (...)
    {
        // 範囲外の場合はデフォルト値を使用
        blockY = 0.0f;
    }
    
    return Vector3(blockX, blockY, blockZ);
}

void SpikeAttack::ActivateSpike(const Vector3& position)
{
    spike_->targetPosition = position;
    spike_->transform.translate = position;
    spike_->transform.translate.y -= kSpikeHeight; // 地面の下から開始
    spike_->isActive = true;
    
    // 警告マークの位置設定（ブロックの上面に表示）
    Transform warningTransform;
    warningTransform.translate = position;
    warningTransform.translate.y += 0.1f; // 少し浮かせる
    warningTransform.rotate = Vector3(-1.57f, 0.0f, 0.0f); // 水平に配置
    warningTransform.scale = Vector3(5.0f, 5.0f, 1.0f);
    spike_->warningMark->SetTransform(warningTransform);
}

