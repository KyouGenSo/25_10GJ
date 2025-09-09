#include "PlayerCollider.hpp"

#include "Collision/CollisionTypeIdDef.h"
#include "Object/Player/Player.h"
#include "Terrain/Block/Block.h"

PlayerCollider::PlayerCollider(Player* _owner)
    :player_(_owner){
    SetOwner(player_);
}

void PlayerCollider::OnCollisionStay(Collider* other) {
    if (other->GetTypeID() != static_cast<uint32_t>(CollisionTypeId::kTerrain))return;
    Vector3 floor = other->GetTransform()->translate;
    floor.y += static_cast<Block*>(other->GetOwner())->kScale/2.f;
    auto tf = player_->GetTransform();
    tf.translate = floor + Vector3(0.f, player_->kSize/2.f, 0.f);
    player_->SetTransform(tf);
    player_->OnGround();
}