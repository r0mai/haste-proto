#pragma once

#include <string>
#include <vector>
#include <variant>

#include "Data.h"

namespace r0 {

// Spell: enemy castable
// Skill: hero castable

using Spell = SpellData;

struct SpellSequence {
	SpellSequence() = default;
	SpellSequence(const SpellSequenceData& data);

	int currentIdx = 0;
	std::vector<Spell> spells;
};

struct Enemy {
	Enemy() = default;
	Enemy(const EnemyData& enemy);

	std::string name;

	int hp = 100;
	int maxHp = 100;

	SpellSequence sequence;

	int castTime = 0;

	Spell* GetCurrentSpell();
	void AdvanceToNextSpell();
};

using DamageSkillEffect = DamageSkillEffectData;
using BlockSkillEffect = BlockSkillEffectData;
using HeroHealSkillEffect = HeroHealSkillEffectData;
using ManaRestoreSkillEffect = ManaRestoreSkillEffectData;
using SlowSkillEffect = SlowSkillEffectData;
using BuffSkillEffect = BuffSkillEffectData;

using SkillEffect = std::variant<
	DamageSkillEffect,
	BlockSkillEffect,
	HeroHealSkillEffect,
	ManaRestoreSkillEffect,
	SlowSkillEffect,
	BuffSkillEffect
>;

struct Skill {
	Skill() = default;
	Skill(const SkillData& data);

	std::string name;
	int castTime = 1;
	int manaCost = 10;

	bool NeedsTarget() const;
	
	std::vector<SkillEffect> effects;
};

using ManaFlowBuffEffect = ManaFlowBuffEffectData;
using DamageFlowBuffEffect = DamageFlowBuffEffectData;

using BuffEffect = std::variant<
	ManaFlowBuffEffect,
	DamageFlowBuffEffect
>;

struct Buff {
	Buff() = default;
	Buff(const BuffData& data);

	std::string name;

	std::vector<BuffEffect> effects;

	int duration = 5;
	int appliedTime = 0; // [0 -> duration)
};

struct Hero {
	Hero() = default;
	Hero(const HeroData& data);

	std::vector<Skill> skills;

	int hp = 100;
	int maxHp = 100;

	// temporary hp over the real hp
	// lost on first damage
	int block = 0;

	int mana = 100;
	int maxMana = 100;

	int castTime = 0;

	// maybe these could just point to the buff list by string id?
	std::vector<Buff> buffs;
};

constexpr int kNoTarget = -1;
constexpr int kNoSkill = -1;

enum class InteractionState {
	kChoosingSkill,
	kAnimatingTurns
};

struct GameState {
	GameState() = default;
	GameState(const ScenarioData& scenario);

	Hero hero;
	std::vector<Enemy> enemies;

	int turnIdx = 1;

	// static data
	std::unordered_map<std::string, Buff> buffs;

	// InteractionState
	int targetedEnemyIdx = kNoTarget; // runtime state
	int castedSkillIdx = kNoSkill; // runtime state

	InteractionState interactionState = InteractionState::kChoosingSkill; // runtime state
	float timeSinceLastTurn = 0.0f; // runtime state
};

} // namespace r0
