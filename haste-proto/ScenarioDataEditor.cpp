#include "imgui.h"
#include "imgui_stdlib.h"

#include "ScenarioDataEditor.h"
#include "ImguiDraw.h"
#include "Serialization.h"

#include "tinyformat.h"

namespace r0 {

ScenarioDataEditor::ScenarioDataEditor() {
	SkillData strikeSkill{
		.name = "Strike",
		.castTime = 5,
		.manaCost = 30,
		.effects = {DamageSkillEffectData{.damage = 20, .radius = 0}}
	};

	SkillData sliceSkill{
		.name = "Slice",
		.castTime = 2,
		.manaCost = 10,
		.effects = {DamageSkillEffectData{.damage = 8, .radius = 0}}
	};

	SkillData stompSkill{
		.name = "Stomp",
		.castTime = 5,
		.manaCost = 30,
		.effects = {DamageSkillEffectData{.damage = 8, .radius = -1}}
	};

	SkillData slashSkill{
		.name = "Slash",
		.castTime = 2,
		.manaCost = 10,
		.effects = {DamageSkillEffectData{.damage = 6, .radius = 1}}
	};

	SkillData blockSkill{
		.name = "Block",
		.castTime = 1,
		.manaCost = 5,
		.effects = {BlockSkillEffectData{.block = 20}}
	};

	SkillData restSkill{
		.name = "Rest",
		.castTime = 8,
		.manaCost = 0,
		.effects = {
			HeroHealSkillEffectData{.heal = 50},
			ManaRestoreSkillEffectData{.mana = 50}
		}
	};

	// init to something
	scenario.hero.skills = {
		strikeSkill,
		sliceSkill,
		stompSkill,
		slashSkill,
		blockSkill,
		restSkill,
	};

	scenario.enemies.push_back(EnemyData{ "Elden Beast", 100, SpellSequenceData({SpellData{10, 2}, SpellData{20}}) });
	scenario.enemies.push_back(EnemyData{ "Diablo", 100, SpellSequenceData({SpellData{5, 1}, SpellData{20}}) });
	scenario.enemies.push_back(EnemyData{ "Lich King", 100, {} });
	scenario.enemies.push_back(EnemyData{ "Dumbledore", 100, SpellSequenceData({SpellData{10}, SpellData{20}, SpellData{15}}) });

	BuffData poisonBuff = {
		.name = "Poison",
		.duration = 5,
		.effects = {
			DamageFlowBuffEffectData{.damage = 5}
		}
	};

	BuffData manaRestoreBuff = {
		.name = "Mana Restore",
		.duration = 3,
		.effects = {
			ManaFlowBuffEffectData{.mana = 5}
		}
	};

	scenario.buffs = {
		poisonBuff,
		manaRestoreBuff
	};
}

bool ScenarioDataEditor::DrawUI() {
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
		if (ImGui::BeginTabItem("Buffs")) {
			DrawBuffListEditor(&scenario.buffs);
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

void ScenarioDataEditor::DrawHeroEditor(HeroData* data) {
	ImGui_IntegerSlider("Max HP", &data->maxHp);
	DrawAbilitiesEditor(&scenario.hero.skills);
}

void ScenarioDataEditor::DrawAbilitiesEditor(std::vector<SkillData>* data) {
	ImGui::Text("Abilities:");
	ImGui_VectorTabEditor("abilities", data, 8,
		[](SkillData* ability) { return ability->name; },
		[this](SkillData* ability) { DrawAbilityEditor(ability); },
		[]() { return SkillData{ .name = "New" }; }
	);
}

void ScenarioDataEditor::DrawAbilityEditor(SkillData* data) {
	ImGui::InputText("Name", &data->name);
	ImGui::SameLine();
	ImGui_IntegerSlider("Mana cost", &data->manaCost);
	ImGui::SameLine();
	ImGui_IntegerSlider("Cast time", &data->castTime);
	ImGui::TextUnformatted("SkillEffects:");
	ImGui_VectorTabEditor("effects", &data->effects, 8,
		[](SkillEffectData* effect) {
			return effect->Visit<const char*>([]<typename T>(const T&) { return T::kName; });
		},
		[this](SkillEffectData* effect) { DrawSkillEffectEditor(effect); },
		[]() { return SkillEffectData{}; }
	);
}

void ScenarioDataEditor::DrawSkillEffectEditor(SkillEffectData* data) {
	ImGui_VariantTypeChooser("Type", data);

	data->Visit<void>([this](auto& subData) {
		DrawSkillEffectEditor(&subData);
	});
}

void ScenarioDataEditor::DrawSkillEffectEditor(DamageSkillEffectData* data) {
	ImGui_IntegerSlider("damage", &data->damage);
	bool isAOE = data->radius == -1;
	if (ImGui::Checkbox("AOE", &isAOE)) {
		data->radius = isAOE ? -1 : 0;
	}
	if (!isAOE) {
		ImGui::SameLine();
		ImGui_IntegerSlider("radius", &data->radius);
	}
}

void ScenarioDataEditor::DrawSkillEffectEditor(BlockSkillEffectData* data) {
	ImGui_IntegerSlider("block", &data->block);
}

void ScenarioDataEditor::DrawSkillEffectEditor(HeroHealSkillEffectData* data) {
	ImGui_IntegerSlider("heal", &data->heal);
}

void ScenarioDataEditor::DrawSkillEffectEditor(ManaRestoreSkillEffectData* data) {
	ImGui_IntegerSlider("mana", &data->mana);
}

void ScenarioDataEditor::DrawSkillEffectEditor(SlowSkillEffectData* data) {
	ImGui_IntegerSlider("slow", &data->slow);
}

void ScenarioDataEditor::DrawEnemiesEditor(std::vector<EnemyData>* data) {
	ImGui_VectorTabEditor("enemies", data, 5,
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

void ScenarioDataEditor::DrawEnemyEditor(EnemyData* data) {
	ImGui::InputText("Name", &data->name);
	ImGui_IntegerSlider("Max HP", &data->maxHp);
	DrawSpellSequenceEditor(&data->sequence);
}

void ScenarioDataEditor::DrawSpellSequenceEditor(SpellSequenceData* data) {
	ImGui::TextUnformatted("Spell sequence:");
	DrawSpellListEditor(&data->spells);
}

void ScenarioDataEditor::DrawSpellListEditor(std::vector<SpellData>* data) {
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

void ScenarioDataEditor::DrawSpellEditor(SpellData* data) {
	ImGui_IntegerSlider("Cast", &data->castTime);
	ImGui::SameLine();
	ImGui_IntegerSlider("Damage", &data->damage);
}

void ScenarioDataEditor::DrawBuffListEditor(std::vector<BuffData>* data) {
	ImGui_VectorCollapsingHeaderEditor("buffs", data,
		[](BuffData* buff) { return buff->name; },
		[this](BuffData* buff) { DrawBuffEditor(buff); },
		[]() { return BuffData{}; }
	);
}

void ScenarioDataEditor::DrawBuffEditor(BuffData* data) {
	ImGui::InputText("Name", &data->name);
	ImGui_IntegerSlider("Duration", &data->duration);
	ImGui_VectorTabEditor("effects", &data->effects, 8,
		[](BuffEffectData* effect) {
			return effect->Visit<const char*>([]<typename T>(const T&) { return T::kName; });
		},
		[this](BuffEffectData* effect) { DrawBuffEffectEditor(effect); },
		[]() { return BuffEffectData{}; }
	);
}

void ScenarioDataEditor::DrawBuffEffectEditor(BuffEffectData* data) {
	ImGui_VariantTypeChooser("Type", data);

	data->Visit<void>([this](auto& subData) {
		DrawBuffEffectEditor(&subData);
	});
}

void ScenarioDataEditor::DrawBuffEffectEditor(ManaFlowBuffEffectData* data) {
	ImGui_IntegerSlider("Mana", &data->mana);
}

void ScenarioDataEditor::DrawBuffEffectEditor(DamageFlowBuffEffectData* data) {
	ImGui_IntegerSlider("Damage", &data->damage);
}

void ScenarioDataEditor::DrawSaveTab(ScenarioData* data) {
	auto jsonStr = Write(*data).dump(2);
	ImGui::InputTextMultiline("JSON", &jsonStr, ImVec2{ 400, 220 }, ImGuiInputTextFlags_ReadOnly);
}

bool ScenarioDataEditor::DrawLoadTab(ScenarioData* data) {
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
