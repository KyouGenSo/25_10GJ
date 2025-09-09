#pragma once
#include <cstdint>

enum class CollisionTypeId : uint32_t {
	kDefault,
	kPlayer,
	kBossBody,
	kEnergyCore,
};