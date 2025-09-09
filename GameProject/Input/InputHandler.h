#pragma once
#include <memory>
#include "dinput.h"
#include "vector2.h"
#include "Xinput.h"
#include "Object/Dispenser/Dispenser.hpp"

class Player;

class InputHandler
{
public:
	InputHandler();
	~InputHandler();

	void Initialize();
	void Update(Player* player);
	
	// 入力状態の取得
	bool IsMoving() const;
	bool IsDashing() const;
	bool IsAttacking() const;
	bool IsShooting() const;
	bool IsPaused() const;
    bool IsJumping() const;
    bool IsDispense() const;
	
	// 入力方向の取得
	Vector2 GetMoveDirection() const;

    Dispenser::Colors GetColor() const;

private:
	// 入力状態を更新
	void UpdateInputStates();
	
	// 入力状態のキャッシュ
	bool isMoving_ = false;
	bool isDashing_ = false;
    bool isJumping_ = false;
	bool isAttacking_ = false;
	bool isShooting_ = false;
	bool isPaused_ = false;
    bool isDispense = false;
	
	Vector2 moveDirection_;

    Dispenser::Colors color_ = Dispenser::Colors::GRAY;
};