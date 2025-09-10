#pragma once
#include <cstdint>

enum class CollisionTypeId : uint32_t {
	kDefault,
	kPlayer,
    kAttack,
	kEnemy,
    kColorBall,
    kTerrain,
    kActiveTerrain,
	kBossBody,
	kBossAttack,
	kEnergyCore,
};