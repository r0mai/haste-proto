#include "imgui.h"
#include "imgui_stdlib.h"

#include "ScenarioEditor.h"
#include "ImguiDraw.h"
#include "Serialization.h"

#include "tinyformat.h"

namespace r0 {

ScenarioEditor::ScenarioEditor() {
	AbilityData strikeAbility{
		.name = "Strike",
		.castTime = 5,
		.manaCost = 30,
		.effects = {DamageEffectData{.damage = 20, .radius = 0}}
	};

	AbilityData sliceAbility{
		.name = "Slice",
		.castTime = 2,
		.manaCost = 10,
		.effects = {DamageEffectData{.damage = 8, .radius = 0}}
	};

	AbilityData stompAbility{
		.name = "Stomp",
		.castTime = 5,
		.manaCost = 30,
		.effects = {DamageEffectData{.damage = 8, .radius = -1}}
	};

	AbilityData slashAbility{
		.name = "Slash",
		.castTime = 2,
		.manaCost = 10,
		.effects = {DamageEffectData{.damage = 6, .radius = 1}}
	};

	AbilityData blockAbility{
		.name = "Block",
		.castTime = 1,
		.manaCost = 5,
		.effects = {BlockEffectData{.block = 20}}
	};

	AbilityData restAbility{
		.name = "Rest",
		.castTime = 8,
		.manaCost = 0,
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
	bool reload = ImGui::Button("Restart");

	ImGui::PushItemWidth(160.0f);
	if (ImGui::BeginTabBar("editor-tab-bar")) {
		if (ImGui::BeginTabItem("Hero")) {
			DrawHeroEditor(&scenario.hero);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Enemies")) {
			DrawEnemiesEditor(&scenario.enemies);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Save")) {
			DrawSaveTab(&scenario);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Load")) {
			reload |= DrawLoadTab(&scenario);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::PopItemWidth();

	return reload;
}

void ScenarioEditor::DrawHeroEditor(HeroData* data) {
	ImGui_IntegerSlider("Max HP", &data->maxHp);
	DrawAbilitiesEditor(&scenario.hero.abilities);
}

void ScenarioEditor::DrawAbilitiesEditor(std::vector<AbilityData>* data) {
	ImGui::Text("Abilities:");
	ImGui_VectorEditor("abilities", data, 8,
		[](AbilityData* ability) { return ability->name; },
		[this](AbilityData* ability) { DrawAbilityEditor(ability); },
		[]() { return AbilityData{ .name = "New" }; }
	);
}

void ScenarioEditor::DrawAbilityEditor(AbilityData* data) {
	ImGui::InputText("Name", &data->name);
	ImGui_IntegerSlider("Mana cost", &data->manaCost);
	ImGui_IntegerSlider("Cast time", &data->castTime);
	ImGui::TextUnformatted("Effects:");
	ImGui_VectorEditor("effects", &data->effects, 8,
		[](AbilityEffectData* effect) {
			return effect->Visit<const char*>([]<typename T>(const T&) { return T::kName; });
		},
		[this](AbilityEffectData* effect) { DrawEffectEditor(effect); },
		[]() { return AbilityEffectData{}; }
	);
}

void ScenarioEditor::DrawEffectEditor(AbilityEffectData* data) {
	ImGui_VariantTypeChooser("Type", data);

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

void ScenarioEditor::DrawEffectEditor(SlowEffectData* data) {
	ImGui_IntegerSlider("slow", &data->slow);
}

void ScenarioEditor::DrawEnemiesEditor(std::vector<EnemyData>* data) {
	ImGui_VectorEditor("enemies", data, 5,
		[](EnemyData* enemy) {
			return enemy->name.c_str();
		},
		[this](EnemyData* enemy) { DrawEnemyEditor(enemy); },
		[]() {
			EnemyData enemy;
			enemy.name = "Goblin";
			return enemy;
		}
	);
}

void ScenarioEditor::DrawEnemyEditor(EnemyData* data) {
	ImGui::InputText("Name", &data->name);
	ImGui_IntegerSlider("Max HP", &data->maxHp);
	DrawSpellSequenceEditor(&data->sequence);
}

void ScenarioEditor::DrawSpellSequenceEditor(SpellSequenceData* data) {
	ImGui::TextUnformatted("Spell sequence:");
	DrawSpellListEditor(&data->spells);
}

void ScenarioEditor::DrawSpellListEditor(std::vector<SpellData>* data) {
	for (int i = 0; i < data->size(); ++i) {
		ImGui::PushID(i);
		auto* item = &(*data)[i];
		DrawSpellEditor(item);

		ImGui::SameLine();
		if (ImGui_RedButton("X")) {
			data->erase(data->begin() + i);
			ImGui::PopID();
			continue;
		}

		ImGui::SameLine();
		if (ImGui_DisabledButton("^", i <= 0)) {
			std::swap((*data)[i], (*data)[i - 1]);
			ImGui::PopID();
			continue;
		}

		ImGui::SameLine();
		if (ImGui_DisabledButton("v", i >= data->size() - 1)) {
			std::swap((*data)[i], (*data)[i + 1]);
			ImGui::PopID();
			continue;
		}

		ImGui::PopID();
	}

	if (ImGui::Button("+")) {
		data->push_back(SpellData{ .damage = 5, .castTime = 5 });
	}
}

void ScenarioEditor::DrawSpellEditor(SpellData* data) {
	ImGui_IntegerSlider("Cast", &data->castTime);
	ImGui::SameLine();
	ImGui_IntegerSlider("Damage", &data->damage);
}

void ScenarioEditor::DrawSaveTab(ScenarioData* data) {
	auto jsonStr = Write(*data).dump(2);
	ImGui::InputTextMultiline("JSON", &jsonStr, ImVec2{ 400, 220 }, ImGuiInputTextFlags_ReadOnly);
}

bool ScenarioEditor::DrawLoadTab(ScenarioData* data) {
	ImGui::InputTextMultiline("JSON", &loadStr_, ImVec2{ 400, 220 });

	bool reload = false;
	if (ImGui::Button("Load")) {
		// TODO error checking
		auto json = nlohmann::json::parse(loadStr_, nullptr, /* allow exceptions */ false);

		if (json.is_discarded()) {
			loadStatusStr_ = "Failed to parse JSON";
		} else {
			ScenarioData newData;

			if (!Read(json, newData)) {
				loadStatusStr_ = "Failed to read JSON";
			} else {
				loadStatusStr_ = "Success";
				*data = newData;
				reload = true;
			}
		}
	}

	ImGui::SameLine();
	ImGui::TextUnformatted(loadStatusStr_.c_str());

	return reload;
}

} // namespace r0
