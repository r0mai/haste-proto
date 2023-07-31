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

} // namespace r0
