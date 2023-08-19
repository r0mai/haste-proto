#include "ImguiDraw.h"
#include "tinyformat.h"
#include "Overloaded.h"

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

	if (ability->effects.size() == 1) {
		ImGui_DrawAbilityEffect(&ability->effects[0]);
	} else {
		ImGui::Text("%d effects", ability->effects.size());
	}

	auto [isPressed, isHovered] = ImGui_HighlightButton(origCursorPos, availSize, false);

	if (isHovered) {
		ImGui::BeginTooltip();
		for (auto& effect : ability->effects) {
			ImGui_DrawAbilityEffect(&effect);
		}
		ImGui::EndTooltip();
	}

	ImGui::PopID();
	return isPressed;
}

void ImGui_DrawAbilityEffect(AbilityEffect* effect) {
	std::visit(Overloaded{
		[&](const DamageEffect& e) {
			if (e.radius == -1) {
				ImGui::Text("%d AOE dmg", e.damage);
			} else if (e.radius == 0) {
				ImGui::Text("%d dmg", e.damage);
			} else {
				ImGui::Text("%d dmg r=%d", e.damage, e.radius);
			}
		},
		[](const BlockEffect& e) {
			ImGui::Text("%d block", e.block);
		},
		[](const HeroHealEffect& e) {
			ImGui::Text("Heal %d", e.heal);
		},
		[](const ManaRestoreEffect& e) {
			ImGui::Text("Mana +%d", e.mana);
		},
	}, *effect);
}

bool ImGui_DrawEnemy(Enemy* enemy, bool selected) {
	ImGui::PushID(enemy);

	auto origCursorPos = ImGui::GetCursorPos();
	auto availSize = ImGui::GetContentRegionAvail();

	if (enemy->hp == 0) { ImGui::PushStyleColor(ImGuiCol_Text, kHighlightedBorderColor); }
	ImGui_CenteredTextUnformatted(enemy->name.c_str());
	if (enemy->hp == 0) { ImGui::PopStyleColor(); }

	const float kBarMaxWidth = 128.0f;

	ImGui_HorizonalBar(kBarMaxWidth, kHorizonalBarHeight, enemy->hp, enemy->maxHp, kHPBarColor);

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
			kBarMaxWidth, kHorizonalBarHeight,
			enemy->castTime, sequence.spells[sequence.currentIdx].castTime,
			kCastTimeColor);
	}


	auto [isPressed, _] = ImGui_HighlightButton(origCursorPos, availSize, selected);
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

void ImGui_AlignedTextUnformatted(const char* text, const ImVec2& align) {
	auto availSize = ImGui::GetContentRegionAvail();
	auto textSize = ImGui::CalcTextSize(text);

	auto cursorPos = ImGui::GetCursorPos();

	auto align0 = cursorPos;
	auto align1 = cursorPos + (availSize - textSize);

	float xPos = std::lerp(align0.x, align1.x, align.x);
	float yPos = std::lerp(align0.y, align1.y, align.y);

	ImGui::SetCursorPos(ImVec2{ xPos, yPos });

	ImGui::TextUnformatted(text);
}

HighlightButtonResult ImGui_HighlightButton(
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

	return { .isPressed = isPressed, .isHovered = isHovered };
}

void ImGui_HorizonalBar(
	float maxWidth,
	float height,
	int value,
	int maxValue,
	ImU32 fillColor,
	const char* prefix
) {
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	auto windowPos = ImGui::GetWindowPos();

	auto avail = ImGui::GetContentRegionAvail().x;

	float width = std::min(avail, maxWidth);

	float barX = (avail - width) * 0.5f;

	auto cursor = ImGui::GetCursorPos();


	float fillRatio = float(value) / maxValue;

	drawList->AddRectFilled(
		windowPos + cursor + ImVec2{ barX, 0 },
		windowPos + cursor + ImVec2{ barX + fillRatio * width, height },
		fillColor
	);

	drawList->AddRect(
		windowPos + cursor + ImVec2{ barX, 0 },
		windowPos + cursor + ImVec2{ barX + width, height },
		kHighlightedBorderColor
	);

	auto text = tfm::format("%s%s%d/%d", prefix, *prefix ? ": " : "", value, maxValue);

	auto textSize = ImGui::CalcTextSize(text.c_str());
	ImGui::SetCursorPosY(cursor.y + (height - textSize.y) * 0.5f);
	ImGui::SetCursorPosX(cursor.x + barX + (width - textSize.x) * 0.5f);

	ImGui::TextUnformatted(text.c_str());

	ImGui::SetCursorPos(ImVec2(cursor.x, cursor.y + height));
}

void ImGui_ResourceBar(
	int value,
	int maxValue,
	ImU32 fillColor
) {
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	auto origin = ImGui::GetWindowPos();
	auto size = ImGui::GetWindowSize();

	float fillRatio = float(value) / maxValue;

	drawList->AddRectFilled(ImVec2{ origin.x, origin.y + size.y * (1.0f - fillRatio) }, origin + size, fillColor);

	auto text = tfm::format("%s/%s", value, maxValue);
	ImGui_AlignedTextUnformatted(text.c_str(), ImVec2{ 0.5f, 1.0f });
}

void ImGui_HealthBar(
	int hp,
	int maxHp,
	int block) 
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	auto origin = ImGui::GetWindowPos();
	auto size = ImGui::GetWindowSize();

	float fillRatio = float(hp) / maxHp;

	float hpBarTop = origin.y + size.y * (1.0f - fillRatio);

	drawList->AddRectFilled(ImVec2{ origin.x, hpBarTop }, origin + size, kHPBarColor);

	if (block > 0) {
		float blockFillRatio = std::min(1.0f, float(hp + block) / maxHp);

		float blockBarTop = origin.y + size.y * (1.0f - blockFillRatio);

		drawList->AddRectFilled(
			ImVec2{
				origin.x,
				blockBarTop,
			},
			ImVec2{
				origin.x + size.x,
				hpBarTop /*make sure at least one pixel is visible*/ + 2.0f,
			},
			kBlockColor
		);
	}

	auto hpText = tfm::format("%s/%s", hp, maxHp);
	if (block > 0) {
		hpText += tfm::format(" (+%s)", block);
	}
	ImGui_AlignedTextUnformatted(hpText.c_str(), ImVec2{ 0.5f, 1.0f });
}

} // namespace r0
