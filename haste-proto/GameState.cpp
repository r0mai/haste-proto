#include "GameState.h"
#include "Overloaded.h"

#include <ranges>

namespace r0 {

namespace {

template<typename... Ts>
std::vector<std::variant<Ts...>> Convert(const std::vector<ExpandedVariant<Ts...>>& data) {
	std::vector<std::variant<Ts...>> res(data.size());
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

Spell* Enemy::GetCurrentSpell() {
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
			[](const DamageSkillEffect& e) { return e.radius >= 0; },
			[](const SlowSkillEffect& e) { return true; },
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

Buff::Buff(const BuffData& data)
	: name(data.name)
	, effects(Convert(data.effects))
	, duration(data.duration) {}

GameState::GameState(const ScenarioData& scenario) {
	hero = Hero(scenario.hero);
	for (auto& enemyData : scenario.enemies) {
		enemies.push_back(Enemy(enemyData));
	}

	for (auto& buffData : scenario.buffs) {
		if (buffs.count(buffData.name)) {
			tfm::printfln("Error: Duplicate buff name found '%s'", buffData.name);
		}

		buffs[buffData.name] = Buff(buffData);
	}
}

} // namespace r0