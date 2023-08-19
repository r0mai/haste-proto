#pragma once

#include <string>
#include <vector>
#include <variant>

namespace r0 {

// Spell: enemy
// Ability: hero

enum class TargetType {
	kNoTarget = 0,
	kEnemy = 1,
};

struct Spell {
	int damage = 10;
	int castTime = 4;
};

struct SpellSequence {
	SpellSequence() = default;
	SpellSequence(std::vector<Spell> spells) : spells(std::move(spells)) {}
	int currentIdx = 0;
	std::vector<Spell> spells;
};

struct Enemy {
	std::string name;

	int hp = 100;
	int maxHp = 100;

	SpellSequence sequence;

	int castTime = 0;
};

struct Encounter {
	int turnIdx = 1;
	std::vector<Enemy> enemies;
};

struct DamageEffect {
	int damage = 0;

	// 0: just the target
	// -1: AOE
	int radius = 0;
};

struct BlockEffect {
	int block = 0;
};

struct HeroHealEffect {
	int heal = 0;
};

struct ManaRestoreEffect {
	int mana = 0;
};

using AbilityEffect = std::variant<
	DamageEffect,
	BlockEffect,
	HeroHealEffect,
	ManaRestoreEffect
>;


struct Ability {
	std::string name;
	int castTime = 1;
	int manaCost = 10;

	TargetType targetType = TargetType::kNoTarget;
	
	std::vector<AbilityEffect> effects;
};

struct Hero {
	std::vector<Ability> abilities;

	int hp = 100;
	int maxHp = 100;

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
	Encounter encounter;
	Hero hero;

	// InteractionState
	int targetedEnemyIdx = kNoTarget;
	int castedAbilityIdx = kNoAbility;

	InteractionState interactionState = InteractionState::kChoosingAbility;
	float timeSinceLastTurn = 0.0f;
};

} // namespace r0
