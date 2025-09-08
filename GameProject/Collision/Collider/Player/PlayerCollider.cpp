#include "PlayerCollider.hpp"

#include "Collision/CollisionTypeIdDef.h"

PlayerCollider::PlayerCollider(Player* _owner)
    :owner_(_owner){
}

void PlayerCollider::OnCollisionStay(Collider* other) {
    if (other->GetTypeID() != static_cast<uint32_t>(CollisionTypeId::kTerrain))return;
    Vector3 floor = other->GetTransform()->translate;
    floor.y += static_cast<Block*>(other->GetOwner())->kScale/2.f;
    auto tf = owner_->GetTransform();
    tf.translate = floor + Vector3(0.f, owner_->kSize/2.f, 0.f);
    owner_->SetTransform(tf);
}