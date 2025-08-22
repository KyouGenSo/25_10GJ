#pragma once
#include "PlayerState.h"
#include "Vector3.h"

class ShootState : public PlayerState
{
public:
	ShootState() : PlayerState("Shoot") {}

	void Enter(Player* player) override;
	void Update(Player* player, float deltaTime) override;
	void Exit(Player* player) override;
	void HandleInput(Player* player) override;

private:
	float fireRate_ = 0.2f;
	float fireRateTimer_ = 0.0f;
	Vector3 aimDirection_;
	
	void CalculateAimDirection(Player* player);
	void Fire(Player* player);
};