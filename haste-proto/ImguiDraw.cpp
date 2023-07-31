#include "ImguiDraw.h"

#include "imgui.h"

namespace r0 {

void ImGui_DrawAbility(Ability* ability) {
	ImGui::TextUnformatted(ability->name.c_str());
	ImGui::Text("Cast: %d", ability->castTime);
}

} // namespace r0
