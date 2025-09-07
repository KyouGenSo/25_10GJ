#include "Boss.h"
#include "Object3d.h"
#include "AABBCollider.h"
#include "CollisionManager.h"
#include "../../Collision/CollisionTypeIdDef.h"

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
    model_ = std::make_unique<Object3d>();
    model_->Initialize();
    model_->SetModel("Boss.gltf");

    transform_.translate = Vector3(95.0f, 0.0f, 95.0f);
    transform_.rotate = Vector3(0.0f, 0.0f, 0.0f);
    transform_.scale = Vector3(3.0f, 3.0f, 3.0f);

    model_->SetTransform(transform_);

    // Colliderの初期化
    bodyCollider_ = std::make_unique<AABBCollider>();
    bodyCollider_->SetTransform(&transform_);
    bodyCollider_->SetSize(Vector3(1.5f, 1.5f, 1.5f));
    bodyCollider_->SetOffset(Vector3(0.0f, 0.0f, 0.0f));
    bodyCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeId::kEnemy));
    bodyCollider_->SetOwner(this);

    // CollisionManagerに登録
    CollisionManager::GetInstance()->AddCollider(bodyCollider_.get());
}

void Boss::Finalize()
{
    // Colliderを削除
    if (bodyCollider_) {
        CollisionManager::GetInstance()->RemoveCollider(bodyCollider_.get());
    }
}

void Boss::Update()
{
    model_->SetTransform(transform_);
    model_->Update();
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
    ImGui::End();

    model_->DrawImGui();
#endif
}