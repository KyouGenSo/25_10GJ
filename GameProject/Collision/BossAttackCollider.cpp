#include "BossAttackCollider.h"
#include "CollisionTypeIdDef.h"
#include "../Object/Player/Player.h"

BossAttackCollider::BossAttackCollider(Player* player)
    : pPlayer_(player)
{
    SetTypeID(static_cast<uint32_t>(CollisionTypeId::kBossAttack));
    SetActive(false);
}

void BossAttackCollider::OnCollisionEnter(Collider* other)
{
    if (!other) return;
    
    uint32_t typeID = other->GetTypeID();
    
    // プレイヤーと衝突した場合
    if (typeID == static_cast<uint32_t>(CollisionTypeId::kPlayer))
    {
        // プレイヤーにダメージを与える
        if (pPlayer_)
        {
            pPlayer_->SetHp(pPlayer_->GetHp() - damage_);
            pPlayer_->PlayHitSE();
        }
    }
}