#pragma once
#include "OBBCollider.h"
#include "Object/Boss/Boss.h"

class BossBodyCollider : public OBBCollider
{
private:
    Boss* pBoss_ = nullptr;
    float damage_;

public:
    BossBodyCollider(Boss* boss);
    virtual ~BossBodyCollider() = default;

    void OnCollisionEnter(Collider* other) override;

    void SetDamage(float damage) { damage_ = damage; }
    float GetDamage() const { return damage_; }
};