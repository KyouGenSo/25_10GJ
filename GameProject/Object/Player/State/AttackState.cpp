#include "AttackState.h"
#include "PlayerStateMachine.h"
#include "../Player.h"
#include "../Input/InputHandler.h"

void AttackState::Enter(Player* player)
{
	// 攻撃アニメーションを再生
	// TODO: Object3dにアニメーション再生機能を追加後に実装
	// player->GetModel()->PlayAnimation("Attack" + std::to_string(comboCount_));
	
	attackTimer_ = 0.0f;
	canCombo_ = false;
}

void AttackState::Update(Player* player, float deltaTime)
{
	attackTimer_ += deltaTime;
	
	// コンボ受付時間の判定
	if (attackTimer_ >= attackDuration_ - comboWindow_ && attackTimer_ < attackDuration_)
	{
		canCombo_ = true;
	}
	
	// 攻撃時間が終了したら元の状態に戻る
	if (attackTimer_ >= attackDuration_)
	{
		PlayerStateMachine* stateMachine = player->GetStateMachine();
		if (stateMachine)
		{
			stateMachine->ChangeState("Idle");
		}
	}
}

void AttackState::Exit(Player* player)
{
	// コンボカウントをリセット
	if (attackTimer_ >= attackDuration_)
	{
		comboCount_ = 0;
	}
}

void AttackState::HandleInput(Player* player)
{
	InputHandler* input = player->GetInputHandler();
	if (!input) return;
	
	// コンボ入力の受付
	if (canCombo_ && input->IsAttacking())
	{
		comboCount_ = (comboCount_ + 1) % maxCombo_;
		Enter(player); // 次の攻撃を開始
	}
}