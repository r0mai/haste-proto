#include "ImguiDraw.h"

#include "imgui.h"

namespace r0 {

void ImGui_SetNextWindowPosition(float x, float y, float w, float h) {
	ImGui::SetNextWindowPos(ImVec2{ x, y });
	ImGui::SetNextWindowSize(ImVec2{ w, h });
}

bool ImGui_DrawAbility(Ability* ability, float abilitySize) {
	ImGui::PushID(ability);

	auto cursorPos = ImGui::GetCursorPos();
	ImGui::TextUnformatted(ability->name.c_str());
	ImGui::Text("Time: %d", ability->castTime);
	ImGui::Text("Cost: %d", ability->manaCost);

	ImGui::SetCursorPos(cursorPos);
	bool result = ImGui::InvisibleButton("", ImVec2(abilitySize, abilitySize));
	ImGui::PopID();
	return result;
}

void ImGui_DrawEnemy(Enemy* enemy) {
	ImGui::PushID(enemy);

	ImGui_CenteredUnformattedText(enemy->name.c_str());

	ImGui::PopID();
}

void ImGui_CenteredUnformattedText(const char* text) {
	auto avail = ImGui::GetContentRegionAvail().x;
	auto textWidth = ImGui::CalcTextSize(text).x;
	auto offset = (avail - textWidth) * 0.5f;

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
	ImGui::TextUnformatted(text);
}

} // namespace r0
