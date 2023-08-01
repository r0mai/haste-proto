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
								CastAbility(&ability, -1);
								break;
							case TargetType::kEnemy:
								state_.targeting = true;
								state_.targetingAbilityIdx = i;
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

void Game::CastAbility(Ability* ability, int targetEnemyIdx) {
	state_.targeting = false;
	state_.targetingAbilityIdx = -1;

	// check for mana again just in case
	if (!HasEnoughMana(ability)) {
		tfm::printfln("Not enough mana to case %s [when casting!]", ability->name);
		return;
	}

	state_.hero.mana = std::clamp(state_.hero.mana - ability->manaCost, 0, state_.hero.maxMana);

	assert((ability->targetType == TargetType::kNoTarget) == (targetEnemyIdx == -1));

	auto& enemies = state_.encounter.enemies;
	switch (ability->targetType) {
		case TargetType::kNoTarget: 
			for (int i = 0; i < enemies.size(); ++i) {
				auto& enemy = enemies[i];
				// TODO if enemies were in an intrusive list, we wouldn't have to juggle indices
				if (DamageEnemy(&enemies[i], ability->damage)) {
					enemies.erase(enemies.begin() + i);
					--i;
				}
			}
			break;
		case TargetType::kEnemy:
			if (DamageEnemy(&enemies[targetEnemyIdx], ability->damage)) {
				enemies.erase(enemies.begin() + targetEnemyIdx);
			}
			break;
	}
}

bool Game::DamageEnemy(Enemy* target, int dmg) {
	if (target->hp <= dmg) {
		// dead
		return true;
	}

	target->hp -= dmg;
	return false;
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
			if (state_.targeting && enemyClicked) {
				CastAbility(&state_.hero.abilities[state_.targetingAbilityIdx], i);
			}
		}
		ImGui::EndTable();
	}
}

} // namespace r0
