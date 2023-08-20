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
	void DrawAbilitiesEditor(std::vector<AbilityData>* data);
	void DrawAbilityEditor(AbilityData* data);

	void DrawEffectEditor(AbilityEffectData* data);
	void DrawEffectEditor(DamageEffectData* data);
	void DrawEffectEditor(BlockEffectData* data);
	void DrawEffectEditor(HeroHealEffectData* data);
	void DrawEffectEditor(ManaRestoreEffectData* data);
};

} // namespace r0
