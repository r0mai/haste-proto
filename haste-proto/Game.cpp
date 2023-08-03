#include "Game.h"

#include <format>
#include <algorithm>
#include "tinyformat.h"

namespace r0 {

void Game::Init(GLFWwindow* window) {
	window_ = window;

	// init to something
	state_.hero.abilities.push_back(Ability{ "Strike", 5, 30, 20, TargetType::kEnemy });
	state_.hero.abilities.push_back(Ability{ "Slice", 2, 10, 8, TargetType::kEnemy });
	state_.hero.abilities.push_back(Ability{ "Stomp", 5, 30, 15, TargetType::kNoTarget });
	state_.hero.abilities.push_back(Ability{ "Slash", 2, 10, 6, TargetType::kNoTarget });
	state_.hero.abilities.push_back(Ability{ "Block", 1, 5, 0, TargetType::kNoTarget });
	state_.hero.abilities.push_back(Ability{ "Rest", 8, -50, 0, TargetType::kNoTarget });

	state_.encounter.enemies.push_back(Enemy{ "Elden Beast", 100, 100, SpellSequence({Spell{10, 2}, Spell{20}}) });
	state_.encounter.enemies.push_back(Enemy{ "Diablo", 100, 100, SpellSequence({Spell{5, 1}, Spell{20}}) });
	state_.encounter.enemies.push_back(Enemy{ "Lich King", 100, 100 });
	state_.encounter.enemies.push_back(Enemy{ "Dumbledore", 100, 100, SpellSequence({Spell{10}, Spell{20}, Spell{15}}) });
}

void Game::Update() {
	DrawHeroWidget();
}

void Game::DrawHeroWidget() {

	ImGui_SetNextWindowPosition(kAbilityBarX, kAbilityBarY, kAbilityBarWidth, kAbilityBarHeight);
	if (ImGui::Begin("ability-button-bar", nullptr, ImGuiWindowFlags_NoDecoration)) {
		DrawAbilityButtonBar();
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
	if (ImGui::Begin("info-panel", nullptr, ImGuiWindowFlags_NoDecoration)) {
		DrawInfoPanel();
	}
	ImGui::End();
}

void Game::DrawAbilityButtonBar() {
	auto& abilities = state_.hero.abilities;
	if (ImGui::BeginTable("ability-table", kAbilitySlots)) {
		for (int i = 0; i < kAbilitySize; ++i) {
			ImGui::TableNextColumn();
			if (i >= abilities.size()) {
				// TODO render empty ability
				continue;
			}

			auto& ability = abilities[i];
			bool isPressed = ImGui_DrawAbility(&ability);
			if (!isPressed) {
				continue;
			}

			if (!HasEnoughMana(&ability)) {
				tfm::printfln("Not enough mana to case %s", ability.name);
				continue;
			}

			switch (ability.targetType) {
				case TargetType::kNoTarget:
					CastAbility(&ability, nullptr);
					break;
				case TargetType::kEnemy:
					if (state_.targetedEnemyIdx != -1) {
						CastAbility(&ability, &state_.encounter.enemies[state_.targetedEnemyIdx]);
					} else {
						tfm::printfln("This ability needs a target");
					}
					break;
			}
		}
		ImGui::EndTable();
	}
}

bool Game::HasEnoughMana(Ability* ability) const {
	return state_.hero.mana >= ability->manaCost;
}

void Game::CastAbility(Ability* ability, Enemy* target) {
	// check for mana again just in case
	if (!HasEnoughMana(ability)) {
		tfm::printfln("Not enough mana to case %s [when casting!]", ability->name);
		return;
	}

	state_.hero.mana = std::clamp(state_.hero.mana - ability->manaCost, 0, state_.hero.maxMana);

	assert((ability->targetType == TargetType::kNoTarget) == (target == nullptr));

	auto& enemies = state_.encounter.enemies;
	switch (ability->targetType) {
		case TargetType::kNoTarget: 
			for (auto& enemy : enemies) {
				DamageEnemy(&enemy, ability->damage);
			}
			break;
		case TargetType::kEnemy:
			DamageEnemy(target, ability->damage);
			break;
	}
}

bool Game::DamageEnemy(Enemy* target, int dmg) {
	target->hp = std::max(target->hp - dmg, 0);
	return target->hp == 0;
}

bool Game::DamageHero(int dmg) {
	state_.hero.hp = std::max(state_.hero.hp - dmg, 0);
	return state_.hero.hp == 0;
}

void Game::DrawHealthBar() {
	ImGui_ResourceBar(state_.hero.hp, state_.hero.maxHp, kHPBarColor);
}

void Game::DrawManaBar() {
	ImGui_ResourceBar(state_.hero.mana, state_.hero.maxMana, kManaBarColor);
}

void Game::DrawEnemyBar() {
	auto& enemies = state_.encounter.enemies;
	if (enemies.empty()) {
		return;
	}

	if (ImGui::BeginTable("enemy-table", enemies.size())) {
		for (int i = 0; i < enemies.size(); ++i) {
			auto& enemy = enemies[i];
			ImGui::TableNextColumn();
			bool isSelected = state_.targetedEnemyIdx == i;
			bool enemyClicked = ImGui_DrawEnemy(&enemy, isSelected);
			if (enemyClicked) {
				state_.targetedEnemyIdx = isSelected ? -1 : i;
			}
		}
		ImGui::EndTable();
	}
}

void Game::DrawInfoPanel() {
	ImGui::Text("Turn %d", state_.encounter.turnIdx);
	if (ImGui::Button("Next turn")) {
		AdvanceTurn();
	}
}

void Game::AdvanceTurn() {
	++state_.encounter.turnIdx;

	auto& enemies = state_.encounter.enemies;
	for (auto& enemy : enemies) {
		if (enemy.sequence.spells.empty()) {
			continue;
		}
		++enemy.castTime;
		auto& currentSpell = enemy.sequence.spells[enemy.sequence.currentIdx];
		if (enemy.castTime >= currentSpell.castTime) {
			// casting enemy spell
			DamageHero(currentSpell.damage);
			enemy.castTime = 0;
			enemy.sequence.currentIdx = (enemy.sequence.currentIdx + 1) % enemy.sequence.spells.size();
		} 
	}
}

} // namespace r0
