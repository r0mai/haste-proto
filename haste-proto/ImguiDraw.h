#pragma once

#include "GameState.h"

namespace r0 {

void ImGui_SetNextWindowPosition(float x, float y, float w, float h);
void ImGui_CenteredUnformattedText(const char* text);

bool ImGui_DrawAbility(Ability* ability);
bool ImGui_DrawEnemy(Enemy* enemy);

} // namespace r0
