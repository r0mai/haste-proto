#include "ImguiDraw.h"

#include "imgui.h"

namespace r0 {

bool ImGui_DrawAbility(Ability* ability, float abilitySize) {
	ImGui::PushID(ability);

	auto cursorPos = ImGui::GetCursorPos();
	ImGui::TextUnformatted(ability->name.c_str());
	ImGui::Text("Cast: %d", ability->castTime);

	ImGui::SetCursorPos(cursorPos);
	bool result = ImGui::InvisibleButton("", ImVec2(abilitySize, abilitySize));
	ImGui::PopID();
	return result;
}

} // namespace r0
