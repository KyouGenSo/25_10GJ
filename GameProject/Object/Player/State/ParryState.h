#pragma once
#include "PlayerState.h"

class ParryState : public PlayerState
{
public:
	ParryState() : PlayerState("Parry") {}

	void Enter(Player* player) override;
	void Update(Player* player, float deltaTime) override;
	void Exit(Player* player) override;
	void HandleInput(Player* player) override;
	
	// パリィ成功時に呼ばれる（敵の攻撃と衝突した時など）
	void OnParrySuccess(Player* player);
	
	bool IsInParryWindow() const { return parryTimer_ <= parryWindow_; }

private:
	float parryTimer_ = 0.0f;
	float parryWindow_ = 0.2f;    // パーフェクトパリィの受付時間
	float parryDuration_ = 0.5f;  // パリィ全体の長さ
	bool perfectParryActive_ = false;
};