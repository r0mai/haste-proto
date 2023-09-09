#pragma once

#include <vector>
#include "ExpandedVariant.h"

namespace r0 {

struct ManaFlowBuffEffect {
	int mana = 0;
};

struct DamageFlowBuffEffect {
	int damage = 0;
};

using BuffEffect = ExpandedVariant<
	ManaFlowBuffEffect,
	DamageFlowBuffEffect
>;

struct Buff {
	std::vector<BuffEffect> effects;
};

} // namespace r0
