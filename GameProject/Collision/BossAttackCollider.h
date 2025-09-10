#pragma once
#include "AABBCollider.h"

class Player;

class BossAttackCollider : public AABBCollider
{
private:
    Player* pPlayer_ = nullptr;
    float damage_ = 20.0f;

public:
    BossAttackCollider(Player* player);
    virtual ~BossAttackCollider() = default;

    void OnCollisionEnter(Collider* other) override;
    
    void SetDamage(float damage) { damage_ = damage; }
    float GetDamage() const { return damage_; }
};