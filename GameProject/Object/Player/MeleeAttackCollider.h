#pragma once
#include "OBBCollider.h"
#include <vector>
#include <unordered_set>

class Player;
class Boss;

class MeleeAttackCollider : public OBBCollider {
private:
	Player* player_ = nullptr;
	std::unordered_set<uint32_t> hitEnemies_;
	Boss* detectedEnemy_ = nullptr;
	
#ifdef _DEBUG
	int collisionCount_ = 0;  // デバッグ用：衝突検出回数
#endif
	
public:
	MeleeAttackCollider(Player* player);
	virtual ~MeleeAttackCollider() = default;
	
	void OnCollision(Collider* other) override;
	void OnCollisionEnter(Collider* other) override;
	
	void Reset();
	
	Boss* GetDetectedEnemy() const { return detectedEnemy_; }
	bool HasHitEnemy(uint32_t enemyId) const;
	
#ifdef _DEBUG
	int GetCollisionCount() const { return collisionCount_; }
#endif
};