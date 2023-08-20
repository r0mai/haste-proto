#include "imgui.h"

#include "ScenarioEditor.h"
#include "ImguiDraw.h"

#include "tinyformat.h"

namespace r0 {

ScenarioEditor::ScenarioEditor() {
	AbilityData strikeAbility{
		.name = "Strike",
		.castTime = 5,
		.manaCost = 30,
		.targetType = TargetType::kEnemy,
		.effects = {DamageEffectData{.damage = 20, .radius = 0}}
	};

	AbilityData sliceAbility{
		.name = "Slice",
		.castTime = 2,
		.manaCost = 10,
		.targetType = TargetType::kEnemy,
		.effects = {DamageEffectData{.damage = 8, .radius = 0}}
	};

	AbilityData stompAbility{
		.name = "Stomp",
		.castTime = 5,
		.manaCost = 30,
		.targetType = TargetType::kNoTarget,
		.effects = {DamageEffectData{.damage = 8, .radius = -1}}
	};

	AbilityData slashAbility{
		.name = "Slash",
		.castTime = 2,
		.manaCost = 10,
		.targetType = TargetType::kEnemy,
		.effects = {DamageEffectData{.damage = 6, .radius = 1}}
	};

	AbilityData blockAbility{
		.name = "Block",
		.castTime = 1,
		.manaCost = 5,
		.targetType = TargetType::kNoTarget,
		.effects = {BlockEffectData{.block = 20}}
	};

	AbilityData restAbility{
		.name = "Rest",
		.castTime = 8,
		.manaCost = 0,
		.targetType = TargetType::kNoTarget,
		.effects = {
			HeroHealEffectData{.heal = 50},
			ManaRestoreEffectData{.mana = 50}
		}
	};

	// init to something
	scenario.hero.abilities = {
		strikeAbility,
		sliceAbility,
		stompAbility,
		slashAbility,
		blockAbility,
		restAbility,
	};

	scenario.enemies.push_back(EnemyData{ "Elden Beast", 100, SpellSequenceData({SpellData{10, 2}, SpellData{20}}) });
	scenario.enemies.push_back(EnemyData{ "Diablo", 100, SpellSequenceData({SpellData{5, 1}, SpellData{20}}) });
	scenario.enemies.push_back(EnemyData{ "Lich King", 100, {} });
	scenario.enemies.push_back(EnemyData{ "Dumbledore", 100, SpellSequenceData({SpellData{10}, SpellData{20}, SpellData{15}}) });
}

bool ScenarioEditor::DrawUI() {
	ImGui::PushItemWidth(120.0f);
	if (ImGui::BeginTabBar("editor-tab-bar")) {
		if (ImGui::BeginTabItem("Abilities")) {
			DrawAbilitiesEditor(&scenario.hero.abilities);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::PopItemWidth();

	bool reload = ImGui::Button("Restart");

	return reload;
}

void ScenarioEditor::DrawAbilitiesEditor(std::vector<AbilityData>* data) {
	auto tabName = [](const char* name, int i) {
		return tfm::format("%s###%s", name, i);
	};
	if (ImGui::BeginTabBar("abilities-tab-bar")) {
		for (int i = 0; i < data->size(); ++i) {
			auto& ability = (*data)[i];
			if (ImGui::BeginTabItem(tabName(ability.name.c_str(), i).c_str())) {
				if (ImGui::Button("Delete")) {
					data->erase(data->begin() + i);
					ImGui::EndTabItem();
					continue;
				}

				ImGui::BeginDisabled(i <= 0);
				ImGui::SameLine();
				if (ImGui::Button("Move Left")) {
					ImGui::SameLine();
					std::swap((*data)[i], (*data)[i - 1]);
					ImGui::EndTabItem();
					continue;
				}
				ImGui::EndDisabled();

				ImGui::BeginDisabled(i >= data->size() - 1);
				ImGui::SameLine();
				if (ImGui::Button("Move Right")) {
					std::swap((*data)[i], (*data)[i + 1]);
					ImGui::EndTabItem();
					continue;
				}
				ImGui::EndDisabled();

				DrawAbilityEditor(&ability);
				ImGui::EndTabItem();
			}
		}
		if (data->size() < 8) {
			// using a negative index here to have some compromise regarding automatic tab switching
			if (ImGui::BeginTabItem(tabName("+", -int(data->size())).c_str())) {
				data->push_back(AbilityData{});
				data->back().name = "New";

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
}

void ScenarioEditor::DrawAbilityEditor(AbilityData* data) {
	if (data->effects.size() > 0) {
		DrawEffectEditor(&data->effects[0]);
	}
}

void ScenarioEditor::DrawEffectEditor(AbilityEffectData* data) {
	ImGui::TextUnformatted("Type:");
	ImGui::SameLine();
	ImGui::RadioButton("Damage", &data->which, data->IndexOf<DamageEffectData>());
	ImGui::SameLine();
	ImGui::RadioButton("Block", &data->which, data->IndexOf<BlockEffectData>());
	ImGui::SameLine();
	ImGui::RadioButton("HeroHeal", &data->which, data->IndexOf<HeroHealEffectData>());
	ImGui::SameLine();
	ImGui::RadioButton("ManaRestore", &data->which, data->IndexOf<ManaRestoreEffectData>());

	data->Visit<void>([this](auto& subData) {
		DrawEffectEditor(&subData);
	});
}

void ScenarioEditor::DrawEffectEditor(DamageEffectData* data) {
	ImGui_IntegerSlider("damage", &data->damage);
	bool isAOE = data->radius == -1;
	if (ImGui::Checkbox("AOE", &isAOE)) {
		data->radius = isAOE ? -1 : 0;
	}
	if (!isAOE) {
		ImGui_IntegerSlider("radius", &data->radius);
	}
}

void ScenarioEditor::DrawEffectEditor(BlockEffectData* data) {
	ImGui_IntegerSlider("block", &data->block);
}

void ScenarioEditor::DrawEffectEditor(HeroHealEffectData* data) {
	ImGui_IntegerSlider("heal", &data->heal);
}

void ScenarioEditor::DrawEffectEditor(ManaRestoreEffectData* data) {
	ImGui_IntegerSlider("mana", &data->mana);
}

} // namespace r0
