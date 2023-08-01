#include "ImguiDraw.h"

#include "imgui.h"

namespace r0 {

void ImGui_SetNextWindowPosition(float x, float y, float w, float h) {
	ImGui::SetNextWindowPos(ImVec2{ x, y });
	ImGui::SetNextWindowSize(ImVec2{ w, h });
}

bool ImGui_DrawAbility(Ability* ability) {
	ImGui::PushID(ability);

	auto origCursorPos = ImGui::GetCursorPos();
	auto availSize = ImGui::GetContentRegionAvail();

	ImGui::TextUnformatted(ability->name.c_str());
	ImGui::Text("Time: %d", ability->castTime);
	ImGui::Text("Cost: %d", ability->manaCost);

	ImGui::SetCursorPos(origCursorPos);
	bool result = ImGui::InvisibleButton("inv-button", availSize);
	ImGui::PopID();
	return result;
}

bool ImGui_DrawEnemy(Enemy* enemy) {
	ImGui::PushID(enemy);

	auto origCursorPos = ImGui::GetCursorPos();
	auto availSize = ImGui::GetContentRegionAvail();

	ImGui_CenteredUnformattedText(enemy->name.c_str());

	// HP bar
	{
		const float hpBarHeight = 32.0f;
		const float hpBarMaxWidth = 128.0f;

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		auto avail = ImGui::GetContentRegionAvail().x;

		float hpBarWidth = std::min(avail, hpBarMaxWidth);

		float hpBarX = (avail - hpBarWidth) * 0.5f;

		auto cursor = ImGui::GetCursorPos();

		float fillRatio = float(enemy->hp) / enemy->maxHp;

		drawList->AddRectFilled(
			ImVec2{ cursor.x + hpBarX + (1.0f - fillRatio) * hpBarWidth, cursor.y},
			ImVec2{ cursor.x + hpBarX + hpBarWidth, cursor.y + hpBarHeight },
			IM_COL32(255, 0, 0, 255)
		);

		drawList->AddRect(
			ImVec2{ cursor.x + hpBarX, cursor.y },
			ImVec2{ cursor.x + hpBarX + hpBarWidth, cursor.y + hpBarHeight },
			IM_COL32(72, 72, 72, 255)
		);
		ImGui::SetCursorPosY(cursor.y + hpBarHeight);
	}

	ImGui::SetCursorPos(origCursorPos);
	bool result = ImGui::InvisibleButton("inv-button", availSize);

	ImGui::PopID();
	return result;
}

void ImGui_CenteredUnformattedText(const char* text) {
	auto avail = ImGui::GetContentRegionAvail().x;
	auto textWidth = ImGui::CalcTextSize(text).x;
	auto offset = (avail - textWidth) * 0.5f;

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
	ImGui::TextUnformatted(text);
}

} // namespace r0
