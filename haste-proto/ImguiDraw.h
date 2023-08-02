#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

#include "GameState.h"

namespace r0 {

void ImGui_SetNextWindowPosition(float x, float y, float w, float h);
void ImGui_CenteredUnformattedText(const char* text);

bool ImGui_DrawAbility(Ability* ability);
bool ImGui_DrawEnemy(Enemy* enemy);

bool ImGui_HighlightButton(
	const ImVec2& origin,
	const ImVec2& size
);

} // namespace r0
