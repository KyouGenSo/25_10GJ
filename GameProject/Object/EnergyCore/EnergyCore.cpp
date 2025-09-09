#include "EnergyCore.h"
#include "Object3d.h"
#include "CollisionManager.h"
#include "Model.h"
#include "../Boss/Boss.h"
#include "../../Collision/EnergyCoreCollider.h"
#include "../../Collision/CollisionTypeIdDef.h"

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
        // 回転アニメーション
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
    
    // コライダーを有効化
    if (collider_)
    {
        collider_->SetActive(true);
    }
}