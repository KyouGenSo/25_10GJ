#pragma once
#include <memory>
#include <unordered_map>
#include <string>

class Player;
class PlayerState;

class PlayerStateMachine
{
public:
	PlayerStateMachine(Player* player);
	~PlayerStateMachine();

	void Initialize();
	void Update(float deltaTime);
	void HandleInput();

	void ChangeState(const std::string& stateName);
	void ChangeState(PlayerState* newState);

	void RegisterState(const std::string& name, std::unique_ptr<PlayerState> state);

	PlayerState* GetCurrentState() const { return currentState_; }
	PlayerState* GetState(const std::string& name) const;

	Player* GetPlayer() const { return player_; }

private:
	Player* player_;
	PlayerState* currentState_;
	PlayerState* previousState_;
	std::unordered_map<std::string, std::unique_ptr<PlayerState>> states_;
};