#pragma once

#include <string>
#include <vector>
#include <variant>
#include "ExpandedVariant.h"

#include "SkillData.h"
#include "BuffData.h"

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

struct HeroData {
	std::vector<SkillData> skills;
	int maxHp = 100;
	int maxMana = 100;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.skills, "skills");
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