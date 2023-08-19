#include "GameState.h"

#include <ranges>

namespace r0 {

SpellSequence::SpellSequence(const SpellSequenceData& data)
	: spells(data.spells) {}

Enemy::Enemy(const EnemyData& enemy)
	: name(enemy.name)
	, hp(enemy.maxHp)
	, maxHp(enemy.maxHp)
	, sequence(enemy.sequence) {}

Hero::Hero(const HeroData& data)
	: abilities(data.abilities)
	, hp(data.maxHp)
	, maxHp(data.maxHp)
	, mana(data.maxMana)
	, maxMana(data.maxMana) {}

GameState::GameState(const ScenarioData& scenario) {
	hero = Hero(scenario.hero);
	for (auto& enemy : scenario.enemies) {
		enemies.push_back(Enemy(enemy));
	}
}

} // namespace r0