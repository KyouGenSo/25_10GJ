#include "ColorBallCollider.hpp"

#include "Collision/CollisionTypeIdDef.h"
#include "Object/Dispenser/Dispenser.hpp"
#include "Terrain/Block/Block.h"

ColorBallCollider::ColorBallCollider(ColorBall* _owner)
    :colorBall_(_owner){
    SetOwner(colorBall_);
}

void ColorBallCollider::OnCollisionEnter(Collider* other) {
    if (other->GetTypeID() != static_cast<uint32_t>(CollisionTypeId::kTerrain))return;
    Block* block = static_cast<Block*>(other->GetOwner());
    block->SetColor(block->MixingColor(colorBall_->GetColorForPaint()));
    colorBall_->imDead();
}