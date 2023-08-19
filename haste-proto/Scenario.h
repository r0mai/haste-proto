#pragma once

#include <vector>

namespace r0 {

enum class TargetType {
	kNoTarget = 0,
	kEnemy = 1,
};

struct SpellData {
	int damage = 10;
	int castTime = 4;
};

struct SpellSequenceData {
	SpellSequenceData() = default;
	SpellSequenceData(std::vector<SpellData> spells) : spells(std::move(spells)) {}

	std::vector<SpellData> spells;
};

struct EnemyData {
	std::string name;

	int maxHp = 100;

	SpellSequenceData sequence;
};

struct DamageEffectData {
	int damage = 0;

	// 0: just the target
	// -1: AOE
	int radius = 0;
};

struct BlockEffectData {
	int block = 0;
};

struct HeroHealEffectData {
	int heal = 0;
};

struct ManaRestoreEffectData {
	int mana = 0;
};

using AbilityEffectData = std::variant<
	DamageEffectData,
	BlockEffectData,
	HeroHealEffectData,
	ManaRestoreEffectData
>;

struct AbilityData {
	std::string name;
	int castTime = 1;
	int manaCost = 10;

	// TODO optimize this out
	TargetType targetType = TargetType::kNoTarget;
	
	std::vector<AbilityEffectData> effects;
};

struct HeroData {
	std::vector<AbilityData> abilities;
	int maxHp = 100;
	int maxMana = 100;
};

struct ScenarioData {
	HeroData hero;
	std::vector<EnemyData> enemies;
};

} // namespace r0