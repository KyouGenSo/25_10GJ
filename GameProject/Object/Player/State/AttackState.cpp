#include "AttackState.h"
#include "PlayerStateMachine.h"
#include "../Player.h"
#include "Input/InputHandler.h"
#include "../MeleeAttackCollider.h"
#include "../../Boss/Boss.h"
#include "CollisionManager.h"

void AttackState::Enter(Player* player)
{
	// 攻撃アニメーションを再生
	// TODO: アニメーション作成後に実装
	// player->GetModel()->PlayAnimation("Attack" + std::to_string(comboCount_));
	
	attackTimer_ = 0.0f;
	searchTimer_ = 0.0f;
	moveTimer_ = 0.0f;
	canCombo_ = false;
	phase_ = SearchTarget;
	targetEnemy_ = nullptr;
	
	// 攻撃範囲Colliderを有効化
	if (player->GetMeleeAttackCollider()) {
		player->GetMeleeAttackCollider()->SetActive(true);
		player->GetMeleeAttackCollider()->Reset();
	}
	
	// SearchForTargetはUpdateのSearchTargetフェーズで呼ばれる
}

void AttackState::Update(Player* player, float deltaTime)
{
	switch (phase_) {
		case SearchTarget:
			// 待機時間をカウント
			searchTimer_ += deltaTime;
			
			// 毎フレーム敵を検索
			SearchForTarget(player);
			
			// 一定時間待機後、次のフェーズへ
			if (searchTimer_ >= maxSearchTime_) {
				if (targetEnemy_) {
					phase_ = MoveToTarget;
				} else {
					phase_ = ExecuteAttack;
				}
			}
			break;
			
		case MoveToTarget:
			ProcessMoveToTarget(player, deltaTime);
			break;
			
		case ExecuteAttack:
			ProcessExecuteAttack(player, deltaTime);
			break;
			
		case Recovery:
			attackTimer_ += deltaTime;
			if (attackTimer_ >= attackDuration_ + 0.2f) {
				PlayerStateMachine* stateMachine = player->GetStateMachine();
				if (stateMachine) {
					stateMachine->ChangeState("Idle");
				}
			}
			break;
	}
}

void AttackState::Exit(Player* player)
{
	// 攻撃範囲Colliderを無効化
	if (player->GetMeleeAttackCollider()) {
		player->GetMeleeAttackCollider()->SetActive(false);
	}
	
	// コンボカウントをリセット
	if (attackTimer_ >= attackDuration_) {
		comboCount_ = 0;
	}
	
	targetEnemy_ = nullptr;
}

void AttackState::HandleInput(Player* player)
{
	InputHandler* input = player->GetInputHandler();
	if (!input) return;
	
	// 攻撃実行中のみコンボ受付
	if (phase_ == ExecuteAttack && canCombo_ && input->IsAttacking()) {
		comboCount_ = (comboCount_ + 1) % maxCombo_;
		Enter(player); // 次の攻撃を開始
	}
}

void AttackState::SearchForTarget(Player* player)
{
	if (!player->GetMeleeAttackCollider()) return;
	
	// 衝突判定は GameScene で既に実行されているため、
	// ここでは検出結果を取得するだけ
	// 次フレームで衝突判定が実行されて敵が検出される
	targetEnemy_ = player->GetMeleeAttackCollider()->GetDetectedEnemy();
}

void AttackState::ProcessMoveToTarget(Player* player, float deltaTime)
{
	if (!targetEnemy_) {
		phase_ = ExecuteAttack;
		return;
	}
	
	moveTimer_ += deltaTime;
	
	// 一定時間敵に近づく
	if (moveTimer_ < maxMoveTime_) {
		player->MoveToTarget(targetEnemy_, deltaTime);
	} else {
		phase_ = ExecuteAttack;
	}
}

void AttackState::ProcessExecuteAttack(Player* player, float deltaTime)
{
	attackTimer_ += deltaTime;
	
	// コンボ受付時間の判定
	if (attackTimer_ >= attackDuration_ - comboWindow_ && attackTimer_ < attackDuration_) {
		canCombo_ = true;
	}
	
	// 攻撃時間が終了したら硬直へ
	if (attackTimer_ >= attackDuration_) {
		phase_ = Recovery;
		attackTimer_ = 0.0f;
	}
}