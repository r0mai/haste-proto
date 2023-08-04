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


struct Ability {
	std::string name;
	int castTime = 1;
	int manaCost = 10;
	
	int damage = 0; /*should be an Effect list*/

	TargetType targetType = TargetType::kNoTarget;
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

struct GameState {
	Encounter encounter;
	Hero hero;

	// InteractionState
	int targetedEnemyIdx = kNoTarget;
	int castedAbilityIdx = kNoAbility;
};

} // namespace r0
