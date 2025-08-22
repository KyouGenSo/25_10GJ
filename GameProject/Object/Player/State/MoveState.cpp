#include "MoveState.h"
#include "PlayerStateMachine.h"
#include "../Player.h"
#include "../Input/InputHandler.h"

void MoveState::Enter(Player* player)
{
	// モデルの歩行アニメーションを再生
	// TODO: Object3dにアニメーション再生機能を追加後に実装
	// player->GetModel()->PlayAnimation("Walk");
}

void MoveState::Update(Player* player, float deltaTime)
{
	// 通常速度で移動
	player->Move(1.0f);
}

void MoveState::Exit(Player* player)
{
	// 歩行状態終了時の処理
}

void MoveState::HandleInput(Player* player)
{
	InputHandler* input = player->GetInputHandler();
	if (!input) return;
	
	PlayerStateMachine* stateMachine = player->GetStateMachine();
	if (!stateMachine) return;
	
	// 優先度順に状態遷移をチェック
	
	// パリィ
	if (input->IsParrying())
	{
		stateMachine->ChangeState("Parry");
		return;
	}
	
	// 攻撃
	if (input->IsAttacking())
	{
		stateMachine->ChangeState("Attack");
		return;
	}
	
	// 射撃
	if (input->IsShooting())
	{
		stateMachine->ChangeState("Shoot");
		return;
	}
	
	// ダッシュ
	if (input->IsDashing())
	{
		stateMachine->ChangeState("Dash");
		return;
	}
	
	// 移動入力がなければIdleへ
	if (!input->IsMoving())
	{
		stateMachine->ChangeState("Idle");
		return;
	}
}