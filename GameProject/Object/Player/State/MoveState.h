#pragma once
#include "PlayerState.h"

class MoveState : public PlayerState
{
public:
	MoveState() : PlayerState("Walk") {}

	void Enter(Player* player) override;
	void Update(Player* player, float deltaTime) override;
	void Exit(Player* player) override;
	void HandleInput(Player* player) override;
};