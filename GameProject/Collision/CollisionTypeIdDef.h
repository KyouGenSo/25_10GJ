#pragma once
#include <cstdint>

enum class CollisionTypeId : uint32_t {
	kDefault,
	kPlayer,
	kPlayerMeleeAttack,
	kPlayerBullet,
	kEnemy,
	kEnemyAttack,
	kEnvironment,
};