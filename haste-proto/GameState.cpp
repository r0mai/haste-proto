#include "GameState.h"
#include "Overloaded.h"

#include <ranges>

namespace r0 {

namespace {

std::vector<SkillEffect> Convert(const std::vector<SkillEffectData>& data) {
	std::vector<SkillEffect> res(data.size());
	for (int i = 0; i < data.size(); ++i) {
		res[i] = data[i].ToVariant();
	}
	return res;
}

std::vector<Skill> Convert(const std::vector<SkillData>& data) {
	std::vector<Skill> res(data.size());
	for (int i = 0; i < data.size(); ++i) {
		res[i] = Skill(data[i]);
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

Spell* Enemy::GetNextSpell() {
	if (sequence.spells.empty()) {
		return nullptr;
	}

	return &sequence.spells[sequence.currentIdx];
}

void Enemy::AdvanceToNextSpell() {
	castTime = 0;
	sequence.currentIdx = (sequence.currentIdx + 1) % sequence.spells.size();
}

Skill::Skill(const SkillData& data)
	: name(data.name)
	, castTime(data.castTime)
	, manaCost(data.manaCost)
	, effects(Convert(data.effects)) {}

bool Skill::NeedsTarget() const {
	for (auto& effect : effects) {
		bool needsTarget = std::visit(Overloaded{
			[](const DamageEffect& e) { return e.radius >= 0; },
			[](const SlowEffect& e) { return true; },
			[](const auto&) { return false; }
		}, effect);
		if (needsTarget) {
			return true;
		}
	}
	return false;
}

Hero::Hero(const HeroData& data)
	: skills(Convert(data.skills))
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