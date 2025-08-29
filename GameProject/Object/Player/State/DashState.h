#pragma once
#include "PlayerState.h"

class DashState : public PlayerState
{
public:
	DashState() : PlayerState("Dash") {}

	void Enter(Player* player) override;
	void Update(Player* player, float deltaTime) override;
	void Exit(Player* player) override;
	void HandleInput(Player* player) override;

private:
	float timer_ = 0.0f;
	float duration_ = 0.05f;
	float speed_ = 10.0f;
};