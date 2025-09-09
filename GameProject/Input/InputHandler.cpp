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
    moveDirection_ = {static_cast<float>(input->PushKey(DIK_D) - input->PushKey(DIK_A)), static_cast<float>(input->PushKey(DIK_W) - input->PushKey(DIK_S)) };

    if (input->IsConnect()){
        moveDirection_ = input->GetLeftStick();
    }

    color_ = input->TriggerButton(XButtons.DPad_Left) ? Dispenser::Colors::BLUE : Dispenser::Colors::GRAY;
    color_ = input->TriggerButton(XButtons.DPad_Up) ? Dispenser::Colors::YELLOW : Dispenser::Colors::GRAY;
    color_ = input->TriggerButton(XButtons.DPad_Right) ? Dispenser::Colors::RED : Dispenser::Colors::GRAY;

	// 各アクションの入力状態を更新
	isMoving_ = !input->LStickInDeadZone();
	isDashing_ = input->TriggerKey(DIK_LSHIFT) || input->TriggerButton(XButtons.B);
    isJumping_ = input->TriggerKey(DIK_SPACE) || input->TriggerButton(XButtons.A);
	isAttacking_ = input->TriggerKey(DIK_Z) || input->TriggerButton(XButtons.X);
	isShooting_ = input->PushKey(DIK_LCONTROL) || input->GetRightTrigger() > 0.5f;
	isPaused_ = input->TriggerKey(DIK_ESCAPE) || input->TriggerButton(XButtons.Start);
    isDispense = input->TriggerKey(DIK_Q) || input->TriggerButton(XButtons.L_Shoulder);
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

bool InputHandler::IsJumping() const {
    return isJumping_;
}

bool InputHandler::IsDispense() const {
    return isDispense;
}

Vector2 InputHandler::GetMoveDirection() const
{
	return moveDirection_;
}

Dispenser::Colors InputHandler::GetColor() const {
    return color_;
}