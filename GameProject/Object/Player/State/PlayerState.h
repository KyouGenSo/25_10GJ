#pragma once
#include <string>

class Player;
class PlayerStateMachine;

class PlayerState
{
public:
	PlayerState(const std::string& name) : stateName_(name) {}
	virtual ~PlayerState() = default;

	virtual void Enter(Player* player) = 0;
	virtual void Update(Player* player, float deltaTime) = 0;
	virtual void Exit(Player* player) = 0;

	virtual void HandleInput(Player* player) {}

	virtual bool CanTransitionTo(const std::string& stateName) const { return true; }

	const std::string& GetName() const { return stateName_; }

protected:
	void ChangeState(PlayerStateMachine* stateMachine, PlayerState* newState);

private:
	std::string stateName_;
};