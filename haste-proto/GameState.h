#pragma once

#include <string>
#include <vector>

namespace r0 {

struct Enemy {
	std::string name;

	// cast sequence
};

struct Encounter {
	std::vector<Enemy> enemies;
};


struct Ability {
	std::string name;
	int castTime = 1;
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
