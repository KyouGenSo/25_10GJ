#include "InputHandler.h"
#include "Object/Player/Player.h"
#include "Input.h"
#include "Vector2.h"

InputHandler::InputHandler()
{
}

InputHandler::~InputHandler()
{
}

void InputHandler::Initialize()
{
	// 初期化処理
}

void InputHandler::Update(Player* player)
{
	UpdateInputStates();
}

void InputHandler::UpdateInputStates()
{
	Input* input = Input::GetInstance();
	
	// 移動入力
	moveDirection_ = input->GetLeftStick();
	isMoving_ = !input->LStickInDeadZone();
	
	// 各アクションの入力状態を更新
	isDashing_ = input->TriggerKey(DIK_SPACE) || input->TriggerButton(XButtons.A);
	isAttacking_ = input->TriggerKey(DIK_Z) || input->TriggerButton(XButtons.X);
	isShooting_ = input->PushKey(DIK_LCONTROL) || input->GetRightTrigger() > 0.5f;
	isPaused_ = input->TriggerKey(DIK_ESCAPE) || input->TriggerButton(XButtons.Start);
}

bool InputHandler::IsMoving() const
{
	return isMoving_;
}

bool InputHandler::IsDashing() const
{
	return isDashing_;
}

bool InputHandler::IsAttacking() const
{
	return isAttacking_;
}

bool InputHandler::IsShooting() const
{
	return isShooting_;
}

bool InputHandler::IsPaused() const
{
	return isPaused_;
}

Vector2 InputHandler::GetMoveDirection() const
{
	return moveDirection_;
}