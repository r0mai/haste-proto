#include "imgui.h"

#include "ScenarioEditor.h"
#include "ImguiDraw.h"

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
	if (ImGui::BeginTabBar("editor-tab-bar")) {
		if (ImGui::BeginTabItem("Abilities")) {
			DrawEffectEditor(&scenario.hero.abilities[0].effects[0]);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	bool reload = ImGui::Button("Restart");

	return reload;
}

void ScenarioEditor::DrawEffectEditor(AbilityEffectData* data) {
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
