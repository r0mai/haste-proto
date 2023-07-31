#include "Game.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

#include "ImguiDraw.h"

#include <format>
#include "tinyformat.h"

namespace r0 {

void Game::Init(GLFWwindow* window) {
	window_ = window;
	int w, h;
	glfwGetWindowSize(window_, &w, &h);
	windowWidth_ = float(w);
	windowHeight_ = float(h);

	// init to something
	state_.hero.abilities.push_back(Ability{ "Slice", 3 });
	state_.hero.abilities.push_back(Ability{ "Defend", 5 });
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

}

void Game::DrawAbilityButtonBar() {
	auto& abilities = state_.hero.abilities;
	if (ImGui::BeginTable("ability-table", kAbilitySlots)) {
		for (int i = 0; i < kAbilitySize; ++i) {
			ImGui::TableNextColumn();
			if (i < abilities.size()) {
				if (ImGui_DrawAbility(&abilities[i], kAbilitySize)) {
					tfm::printfln("Ability %s clicked", i);
				}
			} else {
				// TODO render empty ability
			}
		}
		ImGui::EndTable();
	}
}

void Game::DrawHealthBar() {
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	auto origin = ImGui::GetWindowPos();
	auto size = ImGui::GetWindowSize();

	float filledRatio = state_.hero.hp / state_.hero.maxHp;

	drawList->AddRectFilled(ImVec2{origin.x, origin.y + size.y * (1.0f - filledRatio)}, origin + size, IM_COL32(255, 0, 0, 255));
}

} // namespace r0
