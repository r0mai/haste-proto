#include "Game.h"

#include "ImguiDraw.h"

#include <format>
#include <algorithm>
#include "tinyformat.h"

namespace r0 {

void Game::Init(GLFWwindow* window) {
	window_ = window;
	int w, h;
	glfwGetWindowSize(window_, &w, &h);
	windowWidth_ = float(w);
	windowHeight_ = float(h);

	// init to something
	state_.hero.abilities.push_back(Ability{ "Strike", 5, 30, 20, TargetType::kEnemy });
	state_.hero.abilities.push_back(Ability{ "Slice", 2, 10, 8, TargetType::kEnemy });
	state_.hero.abilities.push_back(Ability{ "Stomp", 5, 30, 15, TargetType::kNoTarget });
	state_.hero.abilities.push_back(Ability{ "Slash", 2, 10, 6, TargetType::kNoTarget });
	state_.hero.abilities.push_back(Ability{ "Block", 1, 5, 0, TargetType::kNoTarget });
	state_.hero.abilities.push_back(Ability{ "Rest", 8, -50, 0, TargetType::kNoTarget });

	state_.encounter.enemies.push_back(Enemy{ "Elden Beast" });
	state_.encounter.enemies.push_back(Enemy{ "Diablo" });
	state_.encounter.enemies.push_back(Enemy{ "Lich King" });
	state_.encounter.enemies.push_back(Enemy{ "Dumbledore" });
}

void Game::Update() {
	DrawHeroWidget();
}

void Game::DrawHeroWidget() {
	const float abilityBarWidth = kAbilitySlots * kAbilitySize;
	const float abilityBarHeight = kAbilitySize;
	const float abilityBarX = (windowWidth_ - abilityBarWidth) * 0.5f;
	const float abilityBarY = windowHeight_ - abilityBarHeight;

	ImGui_SetNextWindowPosition(abilityBarX, abilityBarY, abilityBarWidth, abilityBarHeight);
	if (ImGui::Begin("ability-button-bar", nullptr, ImGuiWindowFlags_NoDecoration)) {
		DrawAbilityButtonBar();
	}
	ImGui::End();


	const float healthBarX = abilityBarX - kHealthBarWidth;
	const float healthBarY = windowHeight_ - kHealthBarHeight;

	ImGui_SetNextWindowPosition(healthBarX, healthBarY, kHealthBarWidth, kHealthBarHeight);
	if (ImGui::Begin("health-bar", nullptr, ImGuiWindowFlags_NoDecoration)) {
		DrawHealthBar();
	}
	ImGui::End();

	const float manaBarX = abilityBarX + abilityBarWidth;
	const float manaBarY = windowHeight_ - kManaBarHeight;

	ImGui_SetNextWindowPosition(manaBarX, manaBarY, kManaBarWidth, kManaBarHeight);
	if (ImGui::Begin("mana-bar", nullptr, ImGuiWindowFlags_NoDecoration)) {
		DrawManaBar();
	}
	ImGui::End();

	const float enemyBarX = 0.0f;
	const float enemyBarY = 0.0f;
	const float enemyBarWidth = windowWidth_;
	ImGui_SetNextWindowPosition(enemyBarX, enemyBarY, enemyBarWidth, kEnemyBarHeight);
	if (ImGui::Begin("enemy-bar", nullptr, ImGuiWindowFlags_NoDecoration)) {
		DrawEnemyBar();
	}
	ImGui::End();
}

void Game::DrawAbilityButtonBar() {
	auto& abilities = state_.hero.abilities;
	if (ImGui::BeginTable("ability-table", kAbilitySlots)) {
		for (int i = 0; i < kAbilitySize; ++i) {
			ImGui::TableNextColumn();
			if (i < abilities.size()) {
				auto& ability = abilities[i];
				if (ImGui_DrawAbility(&ability)) {
					if (HasEnoughMana(&ability)) {
						switch (ability.targetType) {
							case TargetType::kNoTarget:
								CastAbility(&ability, nullptr);
								break;
							case TargetType::kEnemy:
								if (state_.targetedEnemyIdx != -1) {
									CastAbility(&ability, &state_.encounter.enemies[state_.targetedEnemyIdx]);
								}
								break;
						}
					} else {
						tfm::printfln("Not enough mana to case %s", ability.name);
					}
				}
			} else {
				// TODO render empty ability
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

void Game::DrawHealthBar() {
	DrawResourceBar(IM_COL32(255, 0, 0, 255), float(state_.hero.hp) / state_.hero.maxHp);
}

void Game::DrawManaBar() {
	DrawResourceBar(IM_COL32(0, 0, 255, 255), float(state_.hero.mana) / state_.hero.maxMana);
}

void Game::DrawResourceBar(const ImColor& color, float filledRatio) {
	// TODO this could go into ImguiDraw
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	auto origin = ImGui::GetWindowPos();
	auto size = ImGui::GetWindowSize();

	drawList->AddRectFilled(ImVec2{ origin.x, origin.y + size.y * (1.0f - filledRatio) }, origin + size, color);
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
			bool enemyClicked = ImGui_DrawEnemy(&enemy);
			if (enemyClicked) {
				state_.targetedEnemyIdx = i;
			}
		}
		ImGui::EndTable();
	}
}

} // namespace r0
