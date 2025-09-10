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

    transform_.translate = Vector3(95.0f, 0.0f, 95.0f);
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
    ImGui::Begin("Boss Control Panel");
    
    // HP表示を最上部に大きく表示
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
    ImGui::SetNextItemWidth(-1);
    float hpRatio = hp_ / kMaxHp;
    ImGui::ProgressBar(hpRatio, ImVec2(0.0f, 30.0f), 
        ("HP: " + std::to_string(static_cast<int>(hp_)) + " / " + std::to_string(static_cast<int>(kMaxHp))).c_str());
    ImGui::PopStyleColor();
    
    // HP関連のコントロール
    ImGui::SameLine();
    if (ImGui::Button("Damage Test"))
    {
        Damage(10.0f);
    }
    
    ImGui::Separator();
    
    // タブシステムで整理
    if (ImGui::BeginTabBar("BossTabBar"))
    {
        // メイン設定タブ
        if (ImGui::BeginTabItem("Main Settings"))
        {
            // Transform設定
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();
                ImGui::DragFloat3("Position", &transform_.translate.x, 0.1f);
                ImGui::DragFloat3("Rotation", &transform_.rotate.x, 0.1f);
                ImGui::DragFloat3("Scale", &transform_.scale.x, 0.1f, 0.1f, 10.0f);
                ImGui::Unindent();
            }
            
            // 戦闘状態設定
            if (ImGui::CollapsingHeader("Battle Status", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();
                
                // 崩壊状態
                ImGui::Checkbox("Collapse State", &isCollapse);
                ImGui::SameLine();
                ImGui::TextColored(isCollapse ? ImVec4(1.0f, 0.5f, 0.0f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f), 
                                  isCollapse ? "[COLLAPSING]" : "[NORMAL]");
                
                if (isCollapse)
                {
                    ImGui::DragFloat("Collapse Duration", &collapseDuration_, 0.1f, 0.1f, 10.0f);
                    ImGui::ProgressBar(collapseTimer / collapseDuration_, ImVec2(0.0f, 0.0f), 
                                      ("Time: " + std::to_string(collapseTimer)).c_str());
                }
                
                // コライダー状態
                ImGui::Checkbox("Colliders Active", &isColliderActive);
                ImGui::SameLine();
                ImGui::TextColored(isColliderActive ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f), 
                                  isColliderActive ? "[ACTIVE]" : "[INACTIVE]");
                
                ImGui::Unindent();
            }
            
            // フラッシュエフェクト設定
            if (ImGui::CollapsingHeader("Flash Effect"))
            {
                ImGui::Indent();
                
                if (isFlashing_)
                {
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "FLASHING!");
                    ImGui::ProgressBar(flashTimer_ / flashDuration_, ImVec2(0.0f, 0.0f));
                }
                
                ImGui::DragFloat("Flash Duration", &flashDuration_, 0.01f, 0.01f, 1.0f);
                
                if (ImGui::Button("Test Flash"))
                {
                    StartDamageFlash(flashDuration_);
                }
                ImGui::SameLine();
                if (ImGui::Button("Quick Flash"))
                {
                    StartDamageFlash(0.05f);
                }
                ImGui::SameLine();
                if (ImGui::Button("Long Flash"))
                {
                    StartDamageFlash(0.5f);
                }
                
                ImGui::Unindent();
            }
            
            ImGui::EndTabItem();
        }
        
        // コライダー設定タブ
        if (ImGui::BeginTabItem("Body Colliders"))
        {
            // 全体のアクティブ状態をトグル
            if (ImGui::Button(isColliderActive ? "Deactivate All" : "Activate All"))
            {
                isColliderActive = !isColliderActive;
            }
            ImGui::SameLine();
            ImGui::Text("Global Active: ");
            ImGui::SameLine();
            ImGui::TextColored(isColliderActive ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 
                              isColliderActive ? "ON" : "OFF");
            
            ImGui::Separator();
            
            // 左右に分けて表示するため、利用可能な幅を計算
            float availableWidth = ImGui::GetContentRegionAvail().x;
            float childWidth = (availableWidth - ImGui::GetStyle().ItemSpacing.x) / 2.0f;
            float childHeight = 400.0f; // 固定高さ
            
            // 左側のコライダー (0, 1)
            ImGui::BeginChild("LeftColliders", ImVec2(childWidth, childHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::Text("Left Side Colliders");
            ImGui::Separator();
            
            for (int i = 0; i < 2; i++)
            {
                ImGui::PushID(i);
                
                // コライダー名に色を付ける
                bool isActive = bodyColliders_[i]->IsActive();
                ImVec4 headerColor = isActive ? ImVec4(0.7f, 1.0f, 0.7f, 1.0f) : ImVec4(1.0f, 0.7f, 0.7f, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(headerColor.x * 0.3f, headerColor.y * 0.3f, headerColor.z * 0.3f, 1.0f));
                
                if (ImGui::CollapsingHeader(("Collider " + std::to_string(i) + (isActive ? " [ON]" : " [OFF]")).c_str()))
                {
                    ImGui::Indent();
                    
                    // Active状態
                    if (ImGui::Checkbox("Active", &isActive))
                    {
                        bodyColliders_[i]->SetActive(isActive);
                    }
                    
                    // Transform (コンパクトに表示)
                    ImGui::Text("Transform:");
                    ImGui::DragFloat3("Pos", &bodyColliderTransforms_[i].translate.x, 0.1f);
                    ImGui::DragFloat3("Rot", &bodyColliderTransforms_[i].rotate.x, 0.1f);
                    ImGui::DragFloat3("Scale", &bodyColliderTransforms_[i].scale.x, 0.1f, 0.1f, 10.0f);
                    
                    ImGui::Separator();
                    
                    // Collider設定
                    ImGui::Text("Settings:");
                    
                    Vector3 size = bodyColliders_[i]->GetSize();
                    if (ImGui::DragFloat3("Size", &size.x, 0.1f, 0.1f, 100.0f))
                    {
                        bodyColliders_[i]->SetSize(size);
                    }
                    
                    Vector3 offset = bodyColliders_[i]->GetOffset();
                    if (ImGui::DragFloat3("Offset", &offset.x, 0.1f))
                    {
                        bodyColliders_[i]->SetOffset(offset);
                    }
                    
                    float damage = bodyColliders_[i]->GetDamage();
                    if (ImGui::DragFloat("Damage", &damage, 0.1f, 0.0f, 100.0f))
                    {
                        bodyColliders_[i]->SetDamage(damage);
                    }
                    
                    ImGui::Unindent();
                }
                
                ImGui::PopStyleColor();
                ImGui::PopID();
                
                if (i < 1) ImGui::Separator();
            }
            
            ImGui::EndChild();
            
            // 右側のコライダー (2, 3)
            ImGui::SameLine();
            
            ImGui::BeginChild("RightColliders", ImVec2(childWidth, childHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::Text("Right Side Colliders");
            ImGui::Separator();
            
            for (int i = 2; i < 4; i++)
            {
                ImGui::PushID(i);
                
                // コライダー名に色を付ける
                bool isActive = bodyColliders_[i]->IsActive();
                ImVec4 headerColor = isActive ? ImVec4(0.7f, 1.0f, 0.7f, 1.0f) : ImVec4(1.0f, 0.7f, 0.7f, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(headerColor.x * 0.3f, headerColor.y * 0.3f, headerColor.z * 0.3f, 1.0f));
                
                if (ImGui::CollapsingHeader(("Collider " + std::to_string(i) + (isActive ? " [ON]" : " [OFF]")).c_str()))
                {
                    ImGui::Indent();
                    
                    // Active状態
                    if (ImGui::Checkbox("Active", &isActive))
                    {
                        bodyColliders_[i]->SetActive(isActive);
                    }
                    
                    // Transform (コンパクトに表示)
                    ImGui::Text("Transform:");
                    ImGui::DragFloat3("Pos", &bodyColliderTransforms_[i].translate.x, 0.1f);
                    ImGui::DragFloat3("Rot", &bodyColliderTransforms_[i].rotate.x, 0.1f);
                    ImGui::DragFloat3("Scale", &bodyColliderTransforms_[i].scale.x, 0.1f, 0.1f, 10.0f);
                    
                    ImGui::Separator();
                    
                    // Collider設定
                    ImGui::Text("Settings:");
                    
                    Vector3 size = bodyColliders_[i]->GetSize();
                    if (ImGui::DragFloat3("Size", &size.x, 0.1f, 0.1f, 100.0f))
                    {
                        bodyColliders_[i]->SetSize(size);
                    }
                    
                    Vector3 offset = bodyColliders_[i]->GetOffset();
                    if (ImGui::DragFloat3("Offset", &offset.x, 0.1f))
                    {
                        bodyColliders_[i]->SetOffset(offset);
                    }
                    
                    float damage = bodyColliders_[i]->GetDamage();
                    if (ImGui::DragFloat("Damage", &damage, 0.1f, 0.0f, 100.0f))
                    {
                        bodyColliders_[i]->SetDamage(damage);
                    }
                    
                    ImGui::Unindent();
                }
                
                ImGui::PopStyleColor();
                ImGui::PopID();
                
                if (i < 3) ImGui::Separator();
            }
            
            ImGui::EndChild();
            
            ImGui::EndTabItem();
        }
        
        // デバッグ情報タブ
        if (ImGui::BeginTabItem("Debug Info"))
        {
            ImGui::Text("Debug Information:");
            ImGui::Separator();
            
            ImGui::Text("Current HP: %.1f / %.1f (%.1f%%)", hp_, kMaxHp, (hp_ / kMaxHp) * 100.0f);
            ImGui::Text("Collapse State: %s", isCollapse ? "True" : "False");
            ImGui::Text("Collapse Timer: %.2f / %.2f", collapseTimer, collapseDuration_);
            ImGui::Text("Colliders Active: %s", isColliderActive ? "True" : "False");
            ImGui::Text("Flash Active: %s", isFlashing_ ? "True" : "False");
            if (isFlashing_)
            {
                ImGui::Text("Flash Timer: %.3f / %.3f", flashTimer_, flashDuration_);
            }
            
            ImGui::Separator();
            ImGui::Text("Active Colliders:");
            for (int i = 0; i < 4; i++)
            {
                bool active = bodyColliders_[i]->IsActive();
                ImGui::Text("  Collider %d: %s", i, active ? "Active" : "Inactive");
            }
            
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
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

    bodyColliderTransforms_[0].translate = Vector3(94.85f, transform_.translate.y, 71.4f);
    bodyColliderTransforms_[0].rotate = Vector3(0.0f, 0.0f, 0.0f);
    bodyColliderTransforms_[0].scale = Vector3(7.3f, 7.4f, 6.7f);

    bodyColliderTransforms_[1].translate = Vector3(95.0f, transform_.translate.y, 85.2f);
    bodyColliderTransforms_[1].rotate = Vector3(0.0f, 0.0f, 0.0f);
    bodyColliderTransforms_[1].scale = Vector3(12.1f, 9.2f, 18.0f);

    bodyColliderTransforms_[2].translate = Vector3(108.7f, transform_.translate.y, 84.5f);
    bodyColliderTransforms_[2].rotate = Vector3(0.0f, 1.05f, 0.0f);
    bodyColliderTransforms_[2].scale = Vector3(4.4f, 4.8f, 14.2f);

    bodyColliderTransforms_[3].translate = Vector3(81.4f, transform_.translate.y, 84.5f);
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