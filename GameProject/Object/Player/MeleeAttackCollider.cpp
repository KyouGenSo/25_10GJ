#include "MeleeAttackCollider.h"
#include "Player.h"
#include "../Boss/Boss.h"
#include "../../Collision/CollisionTypeIdDef.h"

MeleeAttackCollider::MeleeAttackCollider(Player* player) 
	: player_(player) {
	SetTypeID(static_cast<uint32_t>(CollisionTypeId::kPlayerMeleeAttack));
	SetTrigger(true);
	SetActive(false);
}

void MeleeAttackCollider::OnCollision(Collider* other) {
	if (!other) return;
	
#ifdef _DEBUG
	collisionCount_++;
#endif
	
	uint32_t typeID = other->GetTypeID();
	
	if (typeID == static_cast<uint32_t>(CollisionTypeId::kEnemy)) {
		Boss* enemy = static_cast<Boss*>(other->GetOwner());
		if (enemy && !detectedEnemy_) {
			detectedEnemy_ = enemy;
		}
	}
}

void MeleeAttackCollider::OnCollisionEnter(Collider* other) {
	if (!other) return;
	
	uint32_t typeID = other->GetTypeID();
	
	if (typeID == static_cast<uint32_t>(CollisionTypeId::kEnemy)) {
		Boss* enemy = static_cast<Boss*>(other->GetOwner());
		if (enemy) {
			uint32_t enemyId = enemy->GetID();
			
			if (hitEnemies_.find(enemyId) == hitEnemies_.end()) {
				hitEnemies_.insert(enemyId);
				
				enemy->OnHit(10.0f);
			}
			
			if (!detectedEnemy_) {
				detectedEnemy_ = enemy;
			}
		}
	}
}

void MeleeAttackCollider::Reset() {
	hitEnemies_.clear();
	detectedEnemy_ = nullptr;
#ifdef _DEBUG
	collisionCount_ = 0;
#endif
}

bool MeleeAttackCollider::HasHitEnemy(uint32_t enemyId) const {
	return hitEnemies_.find(enemyId) != hitEnemies_.end();
}