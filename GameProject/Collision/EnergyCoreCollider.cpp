#include "EnergyCoreCollider.h"
#include "CollisionTypeIdDef.h"
#include "../Object/EnergyCore/EnergyCore.h"
#include "../Object/Boss/Boss.h"

EnergyCoreCollider::EnergyCoreCollider(EnergyCore* energyCore, Boss* boss)
    : pEnergyCore_(energyCore), pBoss_(boss) 
{
    SetTypeID(static_cast<uint32_t>(CollisionTypeId::kEnergyCore));
    SetActive(true);
    SetOwner(pEnergyCore_);
}

void EnergyCoreCollider::OnCollisionEnter(Collider* other)
{
    if (!other) return;
    
    uint32_t typeID = other->GetTypeID();
    
    // プレイヤーと衝突した場合
    if (typeID == static_cast<uint32_t>(CollisionTypeId::kAttack)) 
    {
        // エネルギーコアにダメージを与える
        if (pEnergyCore_ && !pEnergyCore_->GetIsDestroyed())
        {
            pEnergyCore_->Damage(damage_);
        }
    }
}