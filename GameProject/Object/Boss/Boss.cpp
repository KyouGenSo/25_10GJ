#include "Boss.h"

#include <algorithm>
#include <string>
#include "Object3d.h"
#include "AABBCollider.h"
#include "CollisionManager.h"
#include "Model.h"
#include "OBBCollider.h"
#include "FrameTimer.h"
#include "../../Collision/CollisionTypeIdDef.h"
#include "../../Collision/BossBodyCollider.h"

#ifdef _DEBUG
#include "ImGui.h"
#endif

Boss::Boss()
{
}

Boss::~Boss()
{
}

void Boss::Initialize()
{
    // HPの初期化
    hp_ = 200.0f; // ボスの初期HP

    isColliderActive = false; // コライダーの初期状態は非アクティブ

    isCollapse = false; // ボスの崩壊状態の初期化

    // モデルの初期化
    model_ = std::make_unique<Object3d>();
    model_->Initialize();
    model_->SetModel("Boss.gltf");

    transform_.translate = Vector3(95.0f, 5.0f, 95.0f);
    transform_.rotate = Vector3(0.0f, 0.0f, 0.0f);
    transform_.scale = Vector3(3.0f, 3.0f, 3.0f);

    model_->SetTransform(transform_);

    model_->GetModel()->SetAnimation("BossIdleAnimation_All");
    model_->GetModel()->SetAnimationLoop("CollapseAnimation_All", false);
    model_->GetModel()->SetAnimationSpeed(0.5f);

    // コライダーの初期化
    InitializeColliders();

}

void Boss::Finalize()
{
    // Colliderを削除
    for (auto& collider : bodyColliders_)
    {
        CollisionManager::GetInstance()->RemoveCollider(collider.get());
    }
}

void Boss::Update()
{
    Collapse();
    
    // ダメージフラッシュの更新
    UpdateDamageFlash();

    model_->SetTransform(transform_);
    model_->Update();
    UpdateColliders();
}

void Boss::Draw()
{
    model_->Draw();
}

void Boss::DrawImGui()
{
#ifdef _DEBUG
    ImGui::Begin("Boss");
    // Transformの編集
    ImGui::DragFloat3("Position", &transform_.translate.x, 0.1f);
    ImGui::DragFloat3("Rotation", &transform_.rotate.x, 0.1f);
    ImGui::DragFloat3("Scale", &transform_.scale.x, 0.1f, 0.1f, 10.0f);
    // isCollapseの編集
    ImGui::Checkbox("isCollapse", &isCollapse);
    // HPの表示(progress bar)
    ImGui::ProgressBar(hp_ / 200, ImVec2(0.0f, 0.0f), ("HP: " + std::to_string(static_cast<int>(hp_)) + " / " + std::to_string(static_cast<int>(200))).c_str());
    
    // フラッシュ設定
    ImGui::Separator();
    ImGui::Text("Flash Settings");
    if (ImGui::DragFloat("Flash Duration", &flashDuration_, 0.01f, 0.01f, 1.0f))
    {
        // フラッシュ時間の調整
    }
    if (ImGui::Button("Test Flash"))
    {
        StartDamageFlash(flashDuration_);
    }

    ImGui::End();

    // BodyColliderの編集
    ImGui::Begin("Boss BodyColliders");
    for (int i = 0; i < 4; i++)
    {
        ImGui::PushID(i);
        if (ImGui::TreeNode(("BodyCollider " + std::to_string(i)).c_str()))
        {
            // Transformの編集
            ImGui::Text("Transform");
            ImGui::DragFloat3("Position", &bodyColliderTransforms_[i].translate.x, 0.1f);
            ImGui::DragFloat3("Rotation", &bodyColliderTransforms_[i].rotate.x, 0.1f);
            ImGui::DragFloat3("Scale", &bodyColliderTransforms_[i].scale.x, 0.1f, 0.1f, 10.0f);
            
            // Collider設定の編集
            ImGui::Separator();
            ImGui::Text("Collider Settings");
            
            // Size
            Vector3 size = bodyColliders_[i]->GetSize();
            if (ImGui::DragFloat3("Size", &size.x, 0.1f, 0.1f, 100.0f))
            {
                bodyColliders_[i]->SetSize(size);
            }
            
            // Offset
            Vector3 offset = bodyColliders_[i]->GetOffset();
            if (ImGui::DragFloat3("Offset", &offset.x, 0.1f))
            {
                bodyColliders_[i]->SetOffset(offset);
            }
            
            // Damage
            float damage = bodyColliders_[i]->GetDamage();
            if (ImGui::DragFloat("Damage", &damage, 0.1f, 0.0f, 100.0f))
            {
                bodyColliders_[i]->SetDamage(damage);
            }
            
            // Active状態
            bool isActive = bodyColliders_[i]->IsActive();
            if (ImGui::Checkbox("Active", &isActive))
            {
                bodyColliders_[i]->SetActive(isActive);
            }
            
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
    ImGui::End();

    model_->DrawImGui();
#endif
}

void Boss::Damage(float damage)
{
    // ダメージ処理
    hp_ -= damage;
    hp_ = std::max<float>(hp_, 0);
    
    // ダメージフラッシュを開始
    StartDamageFlash();
}

//--------------------------------private--------------------------------//
void Boss::InitializeColliders()
{
    // Colliderの初期化
    bodyColliders_.resize(4);
    bodyColliderTransforms_.resize(4);

    bodyColliderTransforms_[0].translate = Vector3(94.85f, 5.0f, 71.4f);
    bodyColliderTransforms_[0].rotate = Vector3(0.0f, 0.0f, 0.0f);
    bodyColliderTransforms_[0].scale = Vector3(7.3f, 7.4f, 6.7f);

    bodyColliderTransforms_[1].translate = Vector3(95.0f, 5.0f, 85.2f);
    bodyColliderTransforms_[1].rotate = Vector3(0.0f, 0.0f, 0.0f);
    bodyColliderTransforms_[1].scale = Vector3(12.1f, 9.2f, 18.0f);

    bodyColliderTransforms_[2].translate = Vector3(108.7f, 5.0f, 84.5f);
    bodyColliderTransforms_[2].rotate = Vector3(0.0f, 1.05f, 0.0f);
    bodyColliderTransforms_[2].scale = Vector3(4.4f, 4.8f, 14.2f);

    bodyColliderTransforms_[3].translate = Vector3(81.4f, 5.0f, 84.5f);
    bodyColliderTransforms_[3].rotate = Vector3(0.0f, -1.05f, 0.0f);
    bodyColliderTransforms_[3].scale = Vector3(4.4f, 4.8f, 14.2f);

    for (int i = 0; i < 4; i++)
    {
        bodyColliders_[i] = std::make_unique<BossBodyCollider>(this);
        bodyColliders_[i]->SetTransform(&bodyColliderTransforms_[i]);
        bodyColliders_[i]->SetSize(Vector3(bodyColliderTransforms_[i].scale.x, bodyColliderTransforms_[i].scale.y, bodyColliderTransforms_[i].scale.z));
        bodyColliders_[i]->SetDamage(10);
        bodyColliders_[i]->SetActive(false);
        // CollisionManagerに登録
        CollisionManager::GetInstance()->AddCollider(bodyColliders_[i].get());
    }
}

void Boss::UpdateColliders()
{
    for (int i = 0; i < 4; i++)
    {
        bodyColliders_[i]->SetTransform(&bodyColliderTransforms_[i]);
        bodyColliders_[i]->SetActive(isColliderActive);
    }
}

void Boss::Collapse()
{
    if (isCollapse)
    {
        model_->GetModel()->SetAnimation("CollapseAnimation_All", 0.2f);

        if (model_->GetModel()->IsAnimationFinished("CollapseAnimation_All"))
        {
            isColliderActive = true;
            collapseTimer += FrameTimer::GetInstance()->GetDeltaTime();
        }

        if (collapseTimer >= collapseDuration_)
        {
            model_->GetModel()->SetAnimation("BossIdleAnimation_All", 0.8f);
            isColliderActive = false;
            collapseTimer = 0.0f;
            isCollapse = false; // 崩壊状態を解除
        }
    }
}

void Boss::StartDamageFlash(float duration)
{
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

void Boss::UpdateDamageFlash()
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