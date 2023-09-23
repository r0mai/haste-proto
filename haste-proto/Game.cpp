#include "Game.h"

#include <format>
#include <algorithm>
#include "tinyformat.h"
#include "Overloaded.h"
#include "Serialization.h"

namespace r0 {

void Game::Init(GLFWwindow* window) {
	window_ = window;

	Reload();
}

void Game::Update() {
	float deltaTime = ImGui::GetIO().DeltaTime;
	LogicUpdate(deltaTime);
	DrawHeroWidget();
}

void Game::Reload() {
	state_ = GameState{ editor_.scenario };
	logLines_.clear();

	{
		auto json = Write(editor_.scenario);
		editor_.scenario = {};
		Read(json, editor_.scenario);
		json = Write(editor_.scenario);
		std::cout << json.dump(4) << std::endl;
	}
}

void Game::LogicUpdate(float deltaTime) {
	switch (state_.interactionState) {
	case InteractionState::kChoosingSkill:
		// nothing to do
		break;
	case InteractionState::kAnimatingTurns:
		state_.timeSinceLastTurn += deltaTime;
		while (state_.timeSinceLastTurn >= kTimeBetweenTurns) {
			if (AdvanceTurn()) {
				state_.interactionState = InteractionState::kChoosingSkill;
				break;
			}
			state_.timeSinceLastTurn -= kTimeBetweenTurns;
		}
		break;
	}
}

void Game::DrawHeroWidget() {
	ImGui_SetNextWindowPosition(kSkillBarX, kSkillBarY, kSkillBarWidth, kSkillBarHeight);
	if (ImGui::Begin("skill-button-bar", nullptr, ImGuiWindowFlags_NoDecoration)) {
		DrawSkillButtonBar();
	}
	ImGui::End();


	ImGui_SetNextWindowPosition(kHealthBarX, kHealthBarY, kHealthBarWidth, kHealthBarHeight);
	if (ImGui::Begin("health-bar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {
		DrawHealthBar();
	}
	ImGui::End();

	ImGui_SetNextWindowPosition(kManaBarX, kManaBarY, kManaBarWidth, kManaBarHeight);
	if (ImGui::Begin("mana-bar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {
		DrawManaBar();
	}
	ImGui::End();

	ImGui_SetNextWindowPosition(kEnemyBarX, kEnemyBarY, kEnemyBarWidth, kEnemyBarHeight);
	if (ImGui::Begin("enemy-bar", nullptr, ImGuiWindowFlags_NoDecoration)) {
		DrawEnemyBar();
	}
	ImGui::End();

	ImGui_SetNextWindowPosition(kInfoPanelX, kInfoPanelY, kInfoPanelWidth, kInfoPanelHeight);
	if (ImGui::Begin("central-panel", nullptr, ImGuiWindowFlags_NoDecoration)) {
		DrawCentralPanel();
	}
	ImGui::End();

	ImGui_SetNextWindowPosition(kHeroCastBarX, kHeroCastBarY, kHeroCastBarWidth, kHeroCastBarHeight);
	if (ImGui::Begin("hero-cast-bar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {
		DrawHeroCastBar();
	}
	ImGui::End();

	ImGui_SetNextWindowPosition(kHeroBuffBarX, kHeroBuffBarY, kHeroBuffBarWidth, kHeroBuffBarHeight);
	if (ImGui::Begin("hero-buff-bar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {
		DrawHeroBuffBar();
	}
	ImGui::End();
}

void Game::DrawSkillButtonBar() {
	auto& skills = state_.hero.skills;
	if (ImGui::BeginTable("skill-table", kSkillSlots)) {
		for (int i = 0; i < kSkillSize; ++i) {
			ImGui::TableNextColumn();
			if (i >= skills.size()) {
				// TODO render empty skill
				continue;
			}

			auto& skill = skills[i];
			bool isPressed = ImGui_DrawSkill(&skill);
			if (!isPressed) {
				continue;
			}

			if (!HasEnoughMana(&skill)) {
				Log("Not enough mana to cast %s", skill.name);
				continue;
			}

			if (skill.NeedsTarget()) {
				if (state_.targetedEnemyIdx == kNoTarget) {
					Log("%s skill needs a target", skill.name);
					continue;
				}
				StartCastingSkill(i);
			} else {
				StartCastingSkill(i);
			}
		}
		ImGui::EndTable();
	}
}

bool Game::HasEnoughMana(Skill* skill) const {
	return state_.hero.mana >= skill->manaCost;
}


void Game::StartCastingSkill(int skillIdx) {
	state_.castedSkillIdx = skillIdx;
	state_.hero.castTime = 0;
	state_.interactionState = InteractionState::kAnimatingTurns;
	state_.timeSinceLastTurn = 0.0f;
}

void Game::CastSkill(Skill* skill, int targetEnemyIdx) {
	// check for mana again just in case
	if (!HasEnoughMana(skill)) {
		Log("Not enough mana to case %s [when casting!]", skill->name);
		return;
	}

	state_.hero.mana = std::clamp(state_.hero.mana - skill->manaCost, 0, state_.hero.maxMana);

	assert(skill->NeedsTarget() != (targetEnemyIdx == kNoTarget));

	auto& enemies = state_.enemies;
	for (auto& effect : skill->effects) {
		ApplySkillEffect(&effect, targetEnemyIdx);
	}
}

void Game::ApplySkillEffect(SkillEffect* effect, int targetEnemyIdx) {
	std::visit(Overloaded{
		[&](const DamageSkillEffect& e) {
			auto& enemies = state_.enemies;
			int beginIdx;
			int endIdx;
			if (e.radius == -1) {
				beginIdx = 0;
				endIdx = enemies.size();
			} else {
				assert(targetEnemyIdx != kNoTarget);
				beginIdx = std::max(0, targetEnemyIdx - e.radius);
				endIdx = std::min(int(enemies.size()), targetEnemyIdx + e.radius + 1);
			}
			for (int i = beginIdx; i < endIdx; ++i) {
				DamageEnemy(&enemies[i], e.damage);
			}
		},
		[&](const BlockSkillEffect& e) {
			ApplyBlock(e.block);
		},
		[&](const HeroHealSkillEffect& e) {
			HealHero(e.heal);
		},
		[&](const ManaRestoreSkillEffect& e) {
			RestoreMana(e.mana);
		},
		[&](const SlowSkillEffect& e) {
			assert(targetEnemyIdx != kNoTarget);
			Slow(&state_.enemies[targetEnemyIdx], e.slow);
		},
		[&](const BuffSkillEffect& e) {
			auto it = state_.buffs.find(e.buffName);
			if (it == state_.buffs.end()) {
				Log("Buff '%s' not found", e.buffName);
				return;
			}

			state_.hero.buffs.push_back(it->second);
		},
	}, *effect);
}

bool Game::DamageEnemy(Enemy* target, int dmg) {
	target->hp = std::max(target->hp - dmg, 0);
	return target->hp == 0;
}

bool Game::DamageHero(int dmg) {
	assert(dmg >= 0);
	dmg -= state_.hero.block;
	if (dmg > 0) {
		state_.hero.hp = std::max(state_.hero.hp - dmg, 0);
	}
	state_.hero.block = 0;
	return state_.hero.hp == 0;
}

void Game::HealHero(int heal) {
	assert(heal >= 0);
	state_.hero.hp = std::min(state_.hero.hp + heal, state_.hero.maxHp);
}

void Game::RestoreMana(int mana) {
	assert(mana >= 0);
	state_.hero.mana = std::min(state_.hero.mana + mana, state_.hero.maxMana);
}

void Game::Slow(Enemy* target, int slow) {
	assert(slow >= 0);
	target->castTime -= slow;
}

void Game::ApplyBlock(int block) {
	assert(block >= 0);
	state_.hero.block += block;
}

void Game::DrawHealthBar() {
	ImGui_HealthBar(state_.hero.hp, state_.hero.maxHp, state_.hero.block);
}

void Game::DrawManaBar() {
	ImGui_ResourceBar(state_.hero.mana, state_.hero.maxMana, kManaBarColor);
}

void Game::DrawEnemyBar() {
	auto& enemies = state_.enemies;

	if (ImGui_BeginCenteredTable("enemy-table", enemies.size(), 256.0f)) {
		for (int i = 0; i < enemies.size(); ++i) {
			auto& enemy = enemies[i];
			ImGui::TableNextColumn();
			bool isSelected = state_.targetedEnemyIdx == i;
			bool enemyClicked = ImGui_DrawEnemy(&enemy, isSelected);
			if (enemyClicked) {
				state_.targetedEnemyIdx = isSelected ? kNoTarget : i;
				state_.hero.castTime = 0;
				state_.castedSkillIdx = kNoSkill;
			}
		}
		ImGui::EndTable();
	}
}

void Game::DrawCentralPanel() {
	if (ImGui::BeginTabBar("tabs")) {
		if (ImGui::BeginTabItem("Game")) {

			ImGui::Text("Turn %d", state_.turnIdx);
			if (state_.interactionState == InteractionState::kChoosingSkill) {
				if (ImGui::Button("Pass")) {
					AdvanceTurn();
				}
			}

			if (ImGui::BeginChild("log")) {
				for (const auto& line : logLines_) {
					ImGui::TextUnformatted(line.c_str());
				}
			}
			ImGui::EndChild();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Editor")) {
			if (editor_.DrawUI()) {
				Reload();
			}

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void Game::DrawHeroCastBar() {
	if (state_.castedSkillIdx == kNoSkill) {
		return;
	}
	auto& skill = state_.hero.skills[state_.castedSkillIdx];
	ImGui_HorizonalBar(
		kHeroCastBarWidth, kHorizonalBarHeight,
		state_.hero.castTime, skill.castTime,
		kCastTimeColor, skill.name.c_str());
}

void Game::DrawHeroBuffBar() {
	auto& buffs = state_.hero.buffs;
	if (buffs.empty()) {
		return;
	}
	if (ImGui::BeginTable("hero-buff-table", buffs.size())) {
		for (int i = 0; i < buffs.size(); ++i) {
			ImGui::TableSetupColumn(tfm::format("%s", i).c_str(), ImGuiTableColumnFlags_WidthFixed, 32.0f);
		}
		ImGui::TableNextRow();
		for (int i = 0; i < buffs.size(); ++i) {
			ImGui::TableNextColumn();
			auto& buff = buffs[i];
			ImGui_DrawBuff(&buff);
		}
		ImGui::EndTable();
	}
}

bool Game::AdvanceTurn() {
	++state_.turnIdx;

	bool finishedCasting = false;

	if (state_.castedSkillIdx != kNoSkill) {
		auto& hero = state_.hero;
		++hero.castTime;

		auto& skill = hero.skills[state_.castedSkillIdx];

		if (hero.castTime >= skill.castTime) {
			if (skill.NeedsTarget()) {
				if (state_.targetedEnemyIdx != -1) {
					CastSkill(&skill, state_.targetedEnemyIdx);
				} else {
					Log("%s skill needs a target [when casting]", skill.name);
				}
			} else {
				CastSkill(&skill, kNoTarget);
			}
			finishedCasting = true;
			state_.hero.castTime = 0;
			state_.castedSkillIdx = kNoSkill;
		}
	}

	auto& enemies = state_.enemies;
	for (auto& enemy : enemies) {
		auto* currentSpell = enemy.GetCurrentSpell();
		if (!currentSpell) {
			continue;
		}
		++enemy.castTime;
		if (enemy.castTime >= currentSpell->castTime) {
			// casting enemy spell
			DamageHero(currentSpell->damage);
			enemy.AdvanceToNextSpell();
		} 
	}

	return finishedCasting;
}

template<typename... Args>
void Game::Log(const char* format, const Args&... args) {
	logLines_.push_back(tfm::format(format, args...));
}

} // namespace r0
