#include "BossBodyCollider.h"
#include "Collision/CollisionTypeIdDef.h"

BossBodyCollider::BossBodyCollider(Boss* boss)
    : pBoss_(boss), damage_(5.f) {
    SetTypeID(static_cast<uint32_t>(CollisionTypeId::kBossBody));
    SetActive(true);
    SetOwner(pBoss_);
}

void BossBodyCollider::OnCollisionEnter(Collider* other)
{
    if (!other) return;

    uint32_t typeID = other->GetTypeID();

    if (typeID == static_cast<uint32_t>(CollisionTypeId::kAttack)) {

        pBoss_->Damage(damage_); // Example damage value

    }
}