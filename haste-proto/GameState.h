#pragma once

#include <string>
#include <vector>
#include <variant>

#include "Scenario.h"

namespace r0 {

// Spell: enemy castable
// Ability: hero castable

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
};

struct Encounter {
};

using DamageEffect = DamageEffectData;
using BlockEffect = BlockEffectData;
using HeroHealEffect = HeroHealEffectData;
using ManaRestoreEffect = ManaRestoreEffectData;
using AbilityEffect = AbilityEffectData;

using Ability = AbilityData;

struct Hero {
	Hero() = default;
	Hero(const HeroData& data);

	std::vector<Ability> abilities;

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
constexpr int kNoAbility = -1;

enum class InteractionState {
	kChoosingAbility,
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
	int castedAbilityIdx = kNoAbility; // runtime state

	InteractionState interactionState = InteractionState::kChoosingAbility; // runtime state
	float timeSinceLastTurn = 0.0f; // runtime state
};

} // namespace r0
