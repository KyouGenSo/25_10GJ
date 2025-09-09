#pragma once
#include "OBBCollider.h"

class EnergyCore;
class Boss;

class EnergyCoreCollider : public OBBCollider
{
private:
    EnergyCore* pEnergyCore_ = nullptr;
    Boss* pBoss_ = nullptr;
    float damage_ = 20.0f;

public:
    EnergyCoreCollider(EnergyCore* energyCore, Boss* boss);
    virtual ~EnergyCoreCollider() = default;

    void OnCollisionEnter(Collider* other) override;
};