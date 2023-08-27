#pragma once

#include "Scenario.h"

namespace r0 {

class ScenarioEditor {
public:
	ScenarioEditor();

	// returns true if scenario should be reloaded
	bool DrawUI();

	ScenarioData scenario;

private:
	void DrawHeroEditor(HeroData* data);
	void DrawAbilitiesEditor(std::vector<AbilityData>* data);
	void DrawAbilityEditor(AbilityData* data);

	void DrawEffectEditor(AbilityEffectData* data);
	void DrawEffectEditor(DamageEffectData* data);
	void DrawEffectEditor(BlockEffectData* data);
	void DrawEffectEditor(HeroHealEffectData* data);
	void DrawEffectEditor(ManaRestoreEffectData* data);

	void DrawEnemiesEditor(std::vector<EnemyData>* data);
	void DrawEnemyEditor(EnemyData* data);
	void DrawSpellSequenceEditor(SpellSequenceData* data);
	void DrawSpellListEditor(std::vector<SpellData>* data);
	void DrawSpellEditor(SpellData* data);
};

} // namespace r0
