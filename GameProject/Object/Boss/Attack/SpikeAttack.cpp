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
    //spike_->collider->SetSize(Vector3(5.0f, kSpikeHeight, 5.0f));
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
    if (stateTimer_ < 0.01f && !hasChangedColor && terrain_)
    {
        try
        {
            terrain_->SetBlockColorAt(GetPlayerBlockPosition(), Block::Colors::White);
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

