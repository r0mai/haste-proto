#pragma once

#include <string>
#include <vector>
#include <variant>
#include "ExpandedVariant.h"

namespace r0 {

struct SpellData {
	int damage = 10;
	int castTime = 4;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.damage, "damage");
		visitor.Visit(self.castTime, "castTime");
	}
};

struct SpellSequenceData {
	SpellSequenceData() = default;
	SpellSequenceData(std::vector<SpellData> spells) : spells(std::move(spells)) {}

	std::vector<SpellData> spells;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.spells, "spells");
	}
};

struct EnemyData {
	std::string name;

	int maxHp = 100;

	SpellSequenceData sequence;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.name, "name");
		visitor.Visit(self.maxHp, "maxHp");
		visitor.Visit(self.sequence, "sequence");
	}
};

struct DamageEffectData {
	static constexpr const char* kName = "DamageEffect";

	int damage = 0;

	// 0: just the target
	// -1: AOE
	int radius = 0;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.damage, "damage");
		visitor.Visit(self.radius, "radius");
	}
};

struct BlockEffectData {
	static constexpr const char* kName = "BlockEffect";

	int block = 0;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.block, "block");
	}
};

struct HeroHealEffectData {
	static constexpr const char* kName = "HeroHealEffect";

	int heal = 0;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.heal, "heal");
	}
};

struct ManaRestoreEffectData {
	static constexpr const char* kName = "ManaRestoreEffect";

	int mana = 0;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.mana, "mana");
	}
};

// increases cast time of current spell of the enemy
struct SlowEffectData {
	static constexpr const char* kName = "SlowEffect";

	int slow = 0;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.slow, "slow");
	}
};

using AbilityEffectData = ExpandedVariant<
	DamageEffectData,
	BlockEffectData,
	HeroHealEffectData,
	ManaRestoreEffectData,
	SlowEffectData
>;

struct AbilityData {
	std::string name;
	int castTime = 1;
	int manaCost = 10;
	
	std::vector<AbilityEffectData> effects;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.name, "name");
		visitor.Visit(self.castTime, "castTime");
		visitor.Visit(self.manaCost, "manaCost");
		visitor.Visit(self.effects, "effects");
	}
};

struct HeroData {
	std::vector<AbilityData> abilities;
	int maxHp = 100;
	int maxMana = 100;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.abilities, "abilities");
		visitor.Visit(self.maxHp, "maxHp");
		visitor.Visit(self.maxMana, "maxMana");
	}
};

struct ScenarioData {
	HeroData hero;
	std::vector<EnemyData> enemies;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.hero, "hero");
		visitor.Visit(self.enemies, "enemies");
	}
};

} // namespace r0