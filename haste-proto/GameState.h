#pragma once

#include <string>
#include <vector>
#include <variant>

#include "Scenario.h"

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

	Spell* GetNextSpell();
	void AdvanceToNextSpell();
};

using DamageEffect = DamageEffectData;
using BlockEffect = BlockEffectData;
using HeroHealEffect = HeroHealEffectData;
using ManaRestoreEffect = ManaRestoreEffectData;
using SlowEffect = SlowEffectData;

using SkillEffect = std::variant<
	DamageEffect,
	BlockEffect,
	HeroHealEffect,
	ManaRestoreEffect,
	SlowEffect
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

	// InteractionState
	int targetedEnemyIdx = kNoTarget; // runtime state
	int castedSkillIdx = kNoSkill; // runtime state

	InteractionState interactionState = InteractionState::kChoosingSkill; // runtime state
	float timeSinceLastTurn = 0.0f; // runtime state
};

} // namespace r0
