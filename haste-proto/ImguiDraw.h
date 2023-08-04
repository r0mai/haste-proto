#pragma once	

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

#include "GameState.h"

namespace r0 {

constexpr ImU32 kDefaultBorderColor = IM_COL32(24, 24, 24, 255);
constexpr ImU32 kHighlightedBorderColor = IM_COL32(72, 72, 72, 255);
constexpr ImU32 kSelectedBorderColor = IM_COL32(144, 144, 144, 255);
constexpr ImU32 kHPBarColor = IM_COL32(255, 0, 0, 255);
constexpr ImU32 kManaBarColor = IM_COL32(0, 0, 255, 255);
constexpr ImU32 kCastTimeColor = IM_COL32(0xF6, 0xBE, 0x00, 255);

constexpr float kHorizonalBarHeight = 32.0f;

void ImGui_SetNextWindowPosition(float x, float y, float w, float h);
void ImGui_CenteredTextUnformatted(const char* text);

bool ImGui_DrawAbility(Ability* ability);
bool ImGui_DrawEnemy(Enemy* enemy, bool selected);
void ImGui_DrawSpell(Spell* spell);

bool ImGui_HighlightButton(
	const ImVec2& origin,
	const ImVec2& size,
	bool selected
);

void ImGui_HorizonalBar(
	float maxWidth,
	float height,
	int value,
	int maxValue,
	ImU32 fillColor
);

void ImGui_ResourceBar(
	int value,
	int maxValue,
	ImU32 fillColor);

} // namespace r0
