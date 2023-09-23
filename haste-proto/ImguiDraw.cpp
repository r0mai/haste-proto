#include "ImguiDraw.h"
#include "tinyformat.h"
#include "Overloaded.h"

#include <algorithm>

namespace r0 {

namespace {

std::string ShortName(const std::string& name) {
	if (name.empty()) {
		return "<>";
	}

	std::string result;

	result.push_back(name.front());

	for (int i = 1; i < name.size(); ++i) {
		if (std::isupper(name[i])) {
			result.push_back(name[i]);
		}
	}

	return result;
}

} // namespace

void ImGui_SetNextWindowPosition(float x, float y, float w, float h) {
	ImGui::SetNextWindowPos(ImVec2{ x, y });
	ImGui::SetNextWindowSize(ImVec2{ w, h });
}

bool ImGui_DrawSkill(Skill* skill) {
	ImGui::PushID(skill);

	auto origCursorPos = ImGui::GetCursorPos();
	auto availSize = ImGui::GetContentRegionAvail();

	ImGui::TextUnformatted(skill->name.c_str());
	ImGui::Text("Time: %d", skill->castTime);
	ImGui::Text("Cost: %d", skill->manaCost);

	if (skill->effects.size() == 1) {
		ImGui_DrawSkillEffect(&skill->effects[0]);
	} else {
		ImGui::Text("%d effects", int(skill->effects.size()));
	}

	auto [isPressed, isHovered] = ImGui_HighlightButton(origCursorPos, availSize, false);

	if (isHovered) {
		ImGui::BeginTooltip();
		for (auto& effect : skill->effects) {
			ImGui_DrawSkillEffect(&effect);
		}
		ImGui::EndTooltip();
	}

	ImGui::PopID();
	return isPressed;
}

void ImGui_DrawSkillEffect(SkillEffect* effect) {
	std::visit(Overloaded{
		[&](const DamageSkillEffect& e) {
			if (e.radius == -1) {
				ImGui::Text("%d AOE dmg", e.damage);
			} else if (e.radius == 0) {
				ImGui::Text("%d dmg", e.damage);
			} else {
				ImGui::Text("%d dmg r=%d", e.damage, e.radius);
			}
		},
		[](const BlockSkillEffect& e) {
			ImGui::Text("%d block", e.block);
		},
		[](const HeroHealSkillEffect& e) {
			ImGui::Text("Heal %d", e.heal);
		},
		[](const ManaRestoreSkillEffect& e) {
			ImGui::Text("Mana +%d", e.mana);
		},
		[](const SlowSkillEffect& e) {
			ImGui::Text("Slow %d", e.slow);
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

	ImGui_VerticalSpacing(20.0f);

	auto* currentSpell = enemy->GetCurrentSpell();

	// SpellSequence
	if (currentSpell) {
		ImGui_DrawSpell(currentSpell);

		ImGui_VerticalSpacing(16.0f);

		int turnsLeft = currentSpell->castTime - enemy->castTime;
		ImGui::PushStyleColor(ImGuiCol_Text, kHighlightYellow);
		ImGui_CenteredTextUnformatted(tfm::format("%s", turnsLeft).c_str());
		ImGui_CenteredTextUnformatted(tfm::format("turn%s", turnsLeft > 1 ? "s" : "").c_str());
		ImGui::PopStyleColor();
	}

	auto [isPressed, _] = ImGui_HighlightButton(origCursorPos, availSize, selected);
	ImGui::PopID();
	return isPressed;
}

void ImGui_DrawSpell(Spell* spell) {
	ImGui_CenteredTextUnformatted(tfm::format("%s damage", spell->damage).c_str());
}

void ImGui_DrawBuff(Buff* buff) {
	ImGui::PushID(buff);

	auto origCursorPos = ImGui::GetCursorPos();
	auto availSize = ImGui::GetContentRegionAvail();

	auto shortName = ShortName(buff->name);

	ImGui_CenteredTextUnformatted(shortName.c_str());

	int turnsLeft = buff->duration - buff->appliedTime;

	ImGui::PushStyleColor(ImGuiCol_Text, kHighlightYellow);
	ImGui_CenteredTextUnformatted(tfm::format("%s", turnsLeft).c_str());
	ImGui::PopStyleColor();

	auto [_, highlighted] = ImGui_HighlightButton(origCursorPos, availSize, false);
	if (highlighted) {
		ImGui::BeginTooltip();

		ImGui::TextUnformatted(buff->name.c_str());

		for (auto& effect : buff->effects) {
			ImGui_DrawBuffEffect(&effect);
		}

		ImGui::PushStyleColor(ImGuiCol_Text, kHighlightYellow);
		ImGui::Text("%d turn%s left", turnsLeft, turnsLeft > 1 ? "s" : "");
		ImGui::PopStyleColor();

		ImGui::EndTooltip();
	}

	ImGui::PopID();
}

void ImGui_DrawBuffEffect(BuffEffect* effect) {
	std::visit(Overloaded{
		[](const DamageFlowBuffEffect& e) {
			if (e.damage > 0) {
				ImGui::Text("%d dmg / turn", e.damage);
			} else if (e.damage < 0) {
				ImGui::Text("%d heal / turn", -e.damage);
			}
		},
		[](const ManaFlowBuffEffect& e) {
			ImGui::Text("%d mana / turn", e.mana);
		},
	}, *effect);
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

bool ImGui_DisabledButton(const char* label, bool disabled) {
	ImGui::BeginDisabled(disabled);
	bool res = ImGui::Button(label);
	ImGui::EndDisabled();
	return res;
}

bool ImGui_RedButton(const char* label) {
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.98f, 0.39f, 0.26f, 0.40f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.98f, 0.39f, 0.26f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.98f, 0.33f, 0.06f, 1.00f));
	bool res = ImGui::Button(label);
	ImGui::PopStyleColor(3);
	return res;
}

bool ImGui_GreenButton(const char* label) {
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.39f, 0.98f, 0.26f, 0.40f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.39f, 0.98f, 0.26f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.33f, 0.98f, 0.06f, 1.00f));
	bool res = ImGui::Button(label);
	ImGui::PopStyleColor(3);
	return res;
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

void ImGui_IntegerSlider(const char* label, int* v) {
	ImGui::DragInt(label, v, 0.2f, 0, 1000);
}

bool ImGui_BeginCenteredTable(const char* label, int columns, float maxColumnSize) {
	if (columns == 0) {
		return false;
	}

	auto windowWidth = ImGui::GetContentRegionAvail().x;
	auto columnWidth = std::min(maxColumnSize, windowWidth / columns);

	float padding = (windowWidth - columnWidth * columns) / 2.0f;
	ImGui::Indent(padding);

	bool begin = ImGui::BeginTable(label, columns);

	if (begin) {
		for (int i = 0; i < columns; ++i) {
			ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, columnWidth);
		}
	}

	return begin;
}

void ImGui_VerticalSpacing(float spacing) {
	ImGui::Dummy(ImVec2{ 0.0f, spacing });
}

} // namespace r0
