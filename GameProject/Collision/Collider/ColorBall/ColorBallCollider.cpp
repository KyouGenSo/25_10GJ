#include "ColorBallCollider.hpp"

#include "Collision/CollisionTypeIdDef.h"
#include "Object/Dispenser/Dispenser.hpp"
#include "Terrain/Block/Block.h"

ColorBallCollider::ColorBallCollider(ColorBall* _owner, Dispenser* _dispenser)
    :colorBall_(_owner), dispenser_(_dispenser){
    SetOwner(colorBall_);
}

void ColorBallCollider::OnCollisionEnter(Collider* other) {
    if (colorBall_->IsDead())return;
    if (other->GetTypeID() != static_cast<uint32_t>(CollisionTypeId::kTerrain))return;
    Block* block = static_cast<Block*>(other->GetOwner());
    block->SetColor(block->MixingColor(colorBall_->GetColorForPaint()));
    colorBall_->PlaySe();

    colorBall_->imDead();

    if (colorBall_->IsDiffused()){
        return;
    }

    Vector3 pos = block->GetPosition() + Vector3{0.f, Block::kScale, 0.f};
    dispenser_->Dispense(pos, {-1.f, 0.f, 0.f}, true);
    dispenser_->Dispense(pos, {1.f, 0.f, 0.f}, true);
    dispenser_->Dispense(pos, {0.f, 0.f, -1.f}, true);
    dispenser_->Dispense(pos, {0.f, 0.f, 1.f}, true);
}