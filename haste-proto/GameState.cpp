#include "GameState.h"
#include "Overloaded.h"

#include <ranges>

namespace r0 {

namespace {

std::vector<AbilityEffect> Convert(const std::vector<AbilityEffectData>& data) {
	std::vector<AbilityEffect> res(data.size());
	for (int i = 0; i < data.size(); ++i) {
		res[i] = data[i].ToVariant();
	}
	return res;
}

std::vector<Ability> Convert(const std::vector<AbilityData>& data) {
	std::vector<Ability> res(data.size());
	for (int i = 0; i < data.size(); ++i) {
		res[i] = Ability(data[i]);
	}
	return res;
}

} // namespace

SpellSequence::SpellSequence(const SpellSequenceData& data)
	: spells(data.spells) {}

Enemy::Enemy(const EnemyData& enemy)
	: name(enemy.name)
	, hp(enemy.maxHp)
	, maxHp(enemy.maxHp)
	, sequence(enemy.sequence) {}

Ability::Ability(const AbilityData& data)
	: name(data.name)
	, castTime(data.castTime)
	, manaCost(data.manaCost)
	, effects(Convert(data.effects)) {}

bool Ability::NeedsTarget() const {
	for (auto& effect : effects) {
		bool needsTarget = std::visit(Overloaded{
			[](const DamageEffect& e) {
				return e.radius >= 0;
			},
			[](const auto&) { return false; }
		}, effect);
		if (needsTarget) {
			return true;
		}
	}
	return false;
}

Hero::Hero(const HeroData& data)
	: abilities(Convert(data.abilities))
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