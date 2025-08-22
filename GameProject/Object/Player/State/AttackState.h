#pragma once
#include "PlayerState.h"

class AttackState : public PlayerState
{
public:
	AttackState() : PlayerState("Attack") {}

	void Enter(Player* player) override;
	void Update(Player* player, float deltaTime) override;
	void Exit(Player* player) override;
	void HandleInput(Player* player) override;

private:
	float attackTimer_ = 0.0f;
	float attackDuration_ = 0.5f;
	int comboCount_ = 0;
	int maxCombo_ = 3;
	float comboWindow_ = 0.2f;
	bool canCombo_ = false;
};