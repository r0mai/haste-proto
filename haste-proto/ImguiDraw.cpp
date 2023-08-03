#include "ImguiDraw.h"
#include "tinyformat.h"

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
	if (ability->damage > 0) {
		auto suffix = ability->targetType == TargetType::kNoTarget ? " AOE" : "";
		ImGui::Text("Dmg: %d%s", ability->damage, suffix);
	}

	bool isPressed = ImGui_HighlightButton(origCursorPos, availSize, false);
	ImGui::PopID();
	return isPressed;
}

bool ImGui_DrawEnemy(Enemy* enemy, bool selected) {
	ImGui::PushID(enemy);

	auto origCursorPos = ImGui::GetCursorPos();
	auto availSize = ImGui::GetContentRegionAvail();

	if (enemy->hp == 0) { ImGui::PushStyleColor(ImGuiCol_Text, kHighlightedBorderColor); }
	ImGui_CenteredTextUnformatted(enemy->name.c_str());
	if (enemy->hp == 0) { ImGui::PopStyleColor(); }

	const float kBarMaxWidth = 128.0f;
	const float kBarHeight = 32.0f;

	ImGui_HorizonalBar(kBarMaxWidth, kBarHeight, enemy->hp, enemy->maxHp, kHPBarColor);

	// SpellSequence
	if (!enemy->sequence.spells.empty()) {
		auto& sequence = enemy->sequence;
		// spells to reveal
		int revealedSpells = 3;
		if (ImGui::BeginTable("spell-table", revealedSpells)) {
			for (int i = 0; i < revealedSpells; ++i) {
				ImGui::TableNextColumn();
				int spellIdx = (sequence.currentIdx + i) % sequence.spells.size();
				auto& spell = sequence.spells[spellIdx];
				ImGui_DrawSpell(&spell);
			}
			ImGui::EndTable();
		}

		ImGui_HorizonalBar(
			kBarMaxWidth, kBarHeight,
			enemy->castTime, sequence.spells[sequence.currentIdx].castTime,
			kCastTimeColor);
	}


	bool isPressed = ImGui_HighlightButton(origCursorPos, availSize, selected);
	ImGui::PopID();
	return isPressed;
}

void ImGui_DrawSpell(Spell* spell) {
	ImGui_CenteredTextUnformatted(tfm::format("%s damage", spell->damage).c_str());
	ImGui_CenteredTextUnformatted(tfm::format("%s turns", spell->castTime).c_str());
}

void ImGui_CenteredTextUnformatted(const char* text) {
	auto avail = ImGui::GetContentRegionAvail().x;
	auto textWidth = ImGui::CalcTextSize(text).x;
	auto offset = (avail - textWidth) * 0.5f;

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
	ImGui::TextUnformatted(text);
}

bool ImGui_HighlightButton(
	const ImVec2& origin,
	const ImVec2& size,
	bool selected
) {
	auto* drawList = ImGui::GetWindowDrawList();
	auto windowOrigin = ImGui::GetWindowPos();

	ImGui::SetCursorPos(origin);
	bool isPressed = ImGui::InvisibleButton("inv-button", size);
	bool isHovered = ImGui::IsItemHovered();

	ImU32 borderColor = kDefaultBorderColor;
	if (selected) {
		borderColor = kSelectedBorderColor;
	} else if (isHovered) {
		borderColor = kHighlightedBorderColor;
	}

	const ImVec2 expandBorder = ImVec2(2, 2);
	drawList->AddRect(windowOrigin + origin - expandBorder, windowOrigin + origin + size + ImVec2(2, 2) * expandBorder, borderColor);

	return isPressed;
}

void ImGui_HorizonalBar(
	float maxWidth,
	float height,
	int value,
	int maxValue,
	ImU32 fillColor
) {
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	auto avail = ImGui::GetContentRegionAvail().x;

	float width = std::min(avail, maxWidth);

	float barX = (avail - width) * 0.5f;

	auto cursor = ImGui::GetCursorPos();


	float fillRatio = float(value) / maxValue;

	drawList->AddRectFilled(
		ImVec2{ cursor.x + barX, cursor.y },
		ImVec2{ cursor.x + barX + fillRatio * width, cursor.y + height },
		fillColor
	);

	drawList->AddRect(
		ImVec2{ cursor.x + barX, cursor.y },
		ImVec2{ cursor.x + barX + width, cursor.y + height },
		kHighlightedBorderColor
	);

	auto text = tfm::format("%d/%d", value, maxValue);

	auto textSize = ImGui::CalcTextSize(text.c_str());
	ImGui::SetCursorPosY(cursor.y + (height - textSize.y) * 0.5f);
	ImGui::SetCursorPosX(cursor.x + barX + (width - textSize.x) * 0.5f);

	ImGui::TextUnformatted(text.c_str());

	ImGui::SetCursorPos(ImVec2(cursor.x, cursor.y + height));
}

} // namespace r0
