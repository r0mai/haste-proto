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
	int block = 0;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.block, "block");
	}
};

struct HeroHealEffectData {
	int heal = 0;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.heal, "heal");
	}
};

struct ManaRestoreEffectData {
	int mana = 0;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.mana, "mana");
	}
};

using AbilityEffectData = ExpandedVariant<
	DamageEffectData,
	BlockEffectData,
	HeroHealEffectData,
	ManaRestoreEffectData
>;

inline const char* AbilityEffectName(const DamageEffectData&) { return "Damage"; }
inline const char* AbilityEffectName(const BlockEffectData&) { return "Block"; }
inline const char* AbilityEffectName(const HeroHealEffectData&) { return "HeroHeal"; }
inline const char* AbilityEffectName(const ManaRestoreEffectData&) { return "ManaRestore"; }

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