#pragma once

#include <vector>
#include "ExpandedVariant.h"

namespace r0 {

struct ManaFlowBuffEffectData {
	static constexpr const char* kName = "ManaFlowBuffEffectData";

	int mana = 0;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.mana, "mana");
	}
};

struct DamageFlowBuffEffectData {
	static constexpr const char* kName = "DamageFlowBuffEffectData";

	int damage = 0;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.damage, "damage");
	}
};

using BuffEffectData = ExpandedVariant<
	ManaFlowBuffEffectData,
	DamageFlowBuffEffectData
>;

struct BuffData {
	std::string name;

	int duration = 1;

	std::vector<BuffEffectData> effects; 

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.name, "name");
		visitor.Visit(self.duration, "duration");
		visitor.Visit(self.effects, "effects");
	}
};

} // namespace r0
