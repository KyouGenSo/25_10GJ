#pragma once
#include "PlayerState.h"

class IdleState : public PlayerState
{
public:
	IdleState() : PlayerState("Idle") {}

	void Enter(Player* player) override;
	void Update(Player* player, float deltaTime) override;
	void Exit(Player* player) override;
	void HandleInput(Player* player) override;
};