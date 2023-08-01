#pragma once

#include <string>
#include <vector>
#include <variant>

namespace r0 {

struct Enemy {
	std::string name;

	int hp = 100;
	int maxHp = 100;

	// TODO cast sequence
};

struct Encounter {
	std::vector<Enemy> enemies;
};


struct Ability {
	std::string name;
	int castTime = 1;
	int manaCost = 10;
};

struct Hero {
	std::vector<Ability> abilities;

	int hp = 100;
	int maxHp = 100;

	int mana = 100;
	int maxMana = 100;
};

struct GameState {
	Encounter encounter;
	Hero hero;
};

} // namespace r0
