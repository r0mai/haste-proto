#pragma once	

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "tinyformat.h"

#include "GameState.h"

namespace r0 {

constexpr ImU32 kDefaultBorderColor = IM_COL32(36, 36, 36, 255);
constexpr ImU32 kHighlightedBorderColor = IM_COL32(72, 72, 72, 255);
constexpr ImU32 kSelectedBorderColor = IM_COL32(144, 144, 144, 255);
constexpr ImU32 kHPBarColor = IM_COL32(255, 0, 0, 255);
constexpr ImU32 kBlockColor = IM_COL32(0, 255, 255, 255);
constexpr ImU32 kManaBarColor = IM_COL32(0, 0, 255, 255);
constexpr ImU32 kCastTimeColor = IM_COL32(0xF6, 0xBE, 0x00, 255);

constexpr ImU32 kErrorRed = IM_COL32(246, 0, 0, 255);
constexpr ImU32 kHighlightYellow = IM_COL32(246, 190, 0, 255);

constexpr float kHorizonalBarHeight = 32.0f;

void ImGui_SetNextWindowPosition(float x, float y, float w, float h);
void ImGui_CenteredTextUnformatted(const char* text);

// [0, 1]
void ImGui_AlignedTextUnformatted(const char* text, const ImVec2& align);

bool ImGui_DrawSkill(Skill* skill);
void ImGui_DrawSkillEffect(SkillEffect* effect);
bool ImGui_DrawEnemy(Enemy* enemy, bool selected);
void ImGui_DrawSpell(Spell* spell);

void ImGui_DrawBuff(Buff* buff);
void ImGui_DrawBuffEffect(BuffEffect* effect);

struct HighlightButtonResult {
	bool isPressed = false;
	bool isHovered = false;
};

HighlightButtonResult ImGui_HighlightButton(
	const ImVec2& origin,
	const ImVec2& size,
	bool selected
);

void ImGui_HorizonalBar(
	float maxWidth,
	float height,
	int value,
	int maxValue,
	ImU32 fillColor,
	const char* prefix = ""
);

bool ImGui_DisabledButton(const char* label, bool disabled = true);
bool ImGui_RedButton(const char* label);
bool ImGui_GreenButton(const char* label);

void ImGui_ResourceBar(
	int value,
	int maxValue,
	ImU32 fillColor);

void ImGui_HealthBar(
	int hp,
	int maxHp,
	int block);

void ImGui_IntegerSlider(const char* label, int* v);

bool ImGui_BeginCenteredTable(const char* label, int columns, float maxColumnSize);

void ImGui_VerticalSpacing(float spacing);

template<typename T, typename NameFunc, typename DrawFunc, typename NewItemFunc>
void ImGui_VectorTabEditor(
	const char* label,
	std::vector<T>* data,
	NameFunc nameFunc,
	DrawFunc drawFunc,
	NewItemFunc newItemFunc,
	int maxSize = INT_MAX)
{
	auto tabName = [](auto name, int i) {
		return tfm::format("%s###%s", name, i);
	};
	if (ImGui::BeginTabBar(label)) {
		for (int i = 0; i < data->size(); ++i) {
			auto* item = &(*data)[i];
			if (ImGui::BeginTabItem(tabName(nameFunc(item), i).c_str())) {
				if (ImGui_RedButton("X")) {
					data->erase(data->begin() + i);
					ImGui::EndTabItem();
					continue;
				}

				ImGui::SameLine();
				if (ImGui_DisabledButton("<", i <= 0)) {
					std::swap((*data)[i], (*data)[i - 1]);
					ImGui::EndTabItem();
					continue;
				}

				ImGui::SameLine();
				if (ImGui_DisabledButton(">", i >= data->size() - 1)) {
					std::swap((*data)[i], (*data)[i + 1]);
					ImGui::EndTabItem();
					continue;
				}

				drawFunc(item);
				ImGui::EndTabItem();
			}
		}
		if (data->size() < maxSize) {
			// using a negative index here to have some compromise regarding automatic tab switching
			if (ImGui::BeginTabItem(tabName("+", -int(data->size())).c_str())) {
				data->push_back(newItemFunc());

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
}

template<typename T, typename NameFunc, typename DrawFunc, typename NewItemFunc>
void ImGui_VectorRadioButtonEditor(
	const char* label,
	std::vector<T>* data,
	NameFunc nameFunc,
	DrawFunc drawFunc,
	NewItemFunc newItemFunc,
	int maxSize = INT_MAX,
	bool reorderable = false)
{
	if (ImGui::BeginTable(label, 2)) {
		ImGui::TableSetupColumn("buff-list", ImGuiTableColumnFlags_WidthFixed, 200.0f + (reorderable ? 50.0f : 0.0f));
		ImGui::TableSetupColumn("buff-editor");
		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		auto* storage = ImGui::GetStateStorage();
		int storageKey = ImGui::GetID("#selectedIdx");
		int selectedIdx = storage->GetInt(storageKey, 0);

		for (int i = 0; i < data->size(); ++i) {
			ImGui::PushID(i);
			if (reorderable) {
				if (ImGui_DisabledButton("^", i <= 0)) {
					std::swap((*data)[i], (*data)[i - 1]);
				}

				ImGui::SameLine();
				if (ImGui_DisabledButton("v", i >= data->size() - 1)) {
					std::swap((*data)[i], (*data)[i + 1]);
				}

				ImGui::SameLine();
			}

			auto& item = (*data)[i];
			if (ImGui::RadioButton(nameFunc(&item).c_str(), i == selectedIdx)) {
				selectedIdx = i;
			}

			ImGui::SameLine();
			if (ImGui_RedButton("X")) {
				data->erase(data->begin() + i);
				--i;
			}
			ImGui::PopID();
		}

		if (data->size() < maxSize && ImGui_GreenButton("+")) {
			data->push_back(newItemFunc());
			selectedIdx = data->size() - 1;
		}

		storage->SetInt(storageKey, selectedIdx);

		ImGui::TableNextColumn();

		if (selectedIdx < data->size()) {
			drawFunc(&(*data)[selectedIdx]);
		}

		ImGui::EndTable();
	}
}

template<typename... Ts>
bool ImGui_VariantTypeChooser(const char* label, ExpandedVariant<Ts...>* variant) {
	const char* previewValue = variant->Visit<const char*>([]<typename T>(const T&) { return T::kName; });

	bool changed = false;
	if (ImGui::BeginCombo(label, previewValue)) {
		variant->VisitAll([&changed, variant]<typename T>(int which, T& value) {
			const char* name = T::kName;
			bool isSelected = variant->which == which;

			if (ImGui::Selectable(name, isSelected)) {
				variant->which = which;
				changed = true;
			}

			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		});

		ImGui::EndCombo();
	}
	return changed;
}

} // namespace r0
