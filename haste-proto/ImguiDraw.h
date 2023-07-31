#pragma once

#include "GameState.h"

namespace r0 {

void ImGui_SetNextWindowPosition(float x, float y, float w, float h);
bool ImGui_DrawAbility(Ability* ability, float abilitySize);

} // namespace r0
