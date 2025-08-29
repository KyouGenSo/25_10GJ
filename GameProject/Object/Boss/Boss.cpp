#include "Boss.h"
#include "Object3d.h"
#include "AABBCollider.h"
#include "CollisionManager.h"
#include "../../Collision/CollisionTypeIdDef.h"

#ifdef _DEBUG
#include "ImGui.h"
#endif

uint32_t Boss::nextID_ = 0;

Boss::Boss()
{
  id_ = nextID_++;
}

Boss::~Boss()
{
}

void Boss::Initialize()
{
  model_ = std::make_unique<Object3d>();
  model_->Initialize();
  model_->SetModel("enemy.gltf");

  transform_.translate = Vector3(0.0f, 2.5f, 10.0f);
  transform_.rotate = Vector3(0.0f, 0.0f, 0.0f);
  transform_.scale = Vector3(1.0f, 1.0f, 1.0f);

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

void Boss::OnHit(float damage)
{
  hp_ -= damage;
  if (hp_ < 0.0f) {
    hp_ = 0.0f;
  }
  
  // ダメージを受けた時のエフェクトなどを追加可能
}

void Boss::DrawImGui()
{
#ifdef _DEBUG
	ImGui::Begin("Boss Debug");
	
	// ボスの状態
	ImGui::Text("=== Boss Status ===");
	ImGui::Text("ID: %d", id_);
	ImGui::Text("HP: %.1f", hp_);
	ImGui::Text("Phase: %d / %d", phase_, maxPhase_);
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", transform_.translate.x, transform_.translate.y, transform_.translate.z);
	ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", transform_.rotate.x, transform_.rotate.y, transform_.rotate.z);
	ImGui::Text("Scale: (%.2f, %.2f, %.2f)", transform_.scale.x, transform_.scale.y, transform_.scale.z);
	
	// Collider情報
	ImGui::Separator();
	ImGui::Text("=== Collider Info ===");
	if (bodyCollider_) {
		ImGui::Text("Collider Active: %s", bodyCollider_->IsActive() ? "Yes" : "No");
		ImGui::Text("Type ID: %d (Enemy)", bodyCollider_->GetTypeID());
		
		Vector3 offset = bodyCollider_->GetOffset();
		ImGui::Text("Offset: (%.2f, %.2f, %.2f)", offset.x, offset.y, offset.z);
		
		Vector3 size = bodyCollider_->GetSize();
		ImGui::Text("Size: (%.2f, %.2f, %.2f)", size.x, size.y, size.z);
		
		// 実際のAABBの位置
		AABB aabb = bodyCollider_->GetAABB();
		ImGui::Text("AABB Min: (%.2f, %.2f, %.2f)", aabb.min.x, aabb.min.y, aabb.min.z);
		ImGui::Text("AABB Max: (%.2f, %.2f, %.2f)", aabb.max.x, aabb.max.y, aabb.max.z);
		
		Vector3 center = bodyCollider_->GetCenter();
		ImGui::Text("Center: (%.2f, %.2f, %.2f)", center.x, center.y, center.z);
	}
	
	ImGui::End();
#endif
}