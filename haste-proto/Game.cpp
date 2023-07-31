#include "Game.h"
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
	DrawAbilityBar();
}

void Game::DrawAbilityBar() {
	const int slots = 8;
	const float abilitySize = 96.0f; // square size
	const float abilityBarWidth = slots * abilitySize;
	const float abilityBarHeight = abilitySize;

	ImGui::SetNextWindowPos(ImVec2{
		(windowWidth_ - abilityBarWidth) * 0.5f,
		windowHeight_ - abilityBarHeight
	});
	ImGui::SetNextWindowSize(ImVec2{
		abilityBarWidth,
		abilityBarHeight
	});

	auto windowFlags = ImGuiWindowFlags_NoDecoration;
	if (ImGui::Begin("ability-bar", nullptr, windowFlags)) {
		auto& abilities = state_.hero.abilities;
		if (ImGui::BeginTable("ability-table", slots)) {
			for (int i = 0; i < slots; ++i) {
				ImGui::TableNextColumn();
				if (i < abilities.size()) {
					if (ImGui_DrawAbility(&abilities[i], abilitySize)) {
						tfm::printfln("Ability %s clicked", i);
					}
				} else {
					// TODO render empty ability
				}
			}
			ImGui::EndTable();
		}
	}

	ImGui::End();
}

} // namespace r0
