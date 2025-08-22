#include "ParryState.h"
#include "PlayerStateMachine.h"
#include "../Player.h"
#include "../Input/InputHandler.h"

void ParryState::Enter(Player* player)
{
	// パリィアニメーションを再生
	// TODO: Object3dにアニメーション再生機能を追加後に実装
	// player->GetModel()->PlayAnimation("Parry");
	
	parryTimer_ = 0.0f;
	perfectParryActive_ = false;
}

void ParryState::Update(Player* player, float deltaTime)
{
	parryTimer_ += deltaTime;
	
	// パリィ時間が終了したら元の状態に戻る
	if (parryTimer_ >= parryDuration_)
	{
		PlayerStateMachine* stateMachine = player->GetStateMachine();
		if (stateMachine)
		{
			stateMachine->ChangeState("Idle");
		}
	}
}

void ParryState::Exit(Player* player)
{
	parryTimer_ = 0.0f;
	perfectParryActive_ = false;
}

void ParryState::HandleInput(Player* player)
{
	// パリィ中は入力を受け付けない
}

void ParryState::OnParrySuccess(Player* player)
{
	if (IsInParryWindow())
	{
		// パーフェクトパリィ成功
		perfectParryActive_ = true;
		
		// カウンター攻撃への遷移
		PlayerStateMachine* stateMachine = player->GetStateMachine();
		if (stateMachine)
		{
			stateMachine->ChangeState("Attack");
		}
	}
}