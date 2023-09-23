#pragma once

#include "Data.h"

namespace r0 {

class ScenarioDataEditor {
public:
	ScenarioDataEditor();

	// returns true if scenario should be reloaded
	bool DrawUI();

	ScenarioData scenario;


private:
	void DrawHeroEditor(HeroData* data);
	void DrawSkillsEditor(std::vector<SkillData>* data);
	void DrawSkillEditor(SkillData* data);

	void DrawSkillEffectEditor(SkillEffectData* data);
	void DrawSkillEffectEditor(DamageSkillEffectData* data);
	void DrawSkillEffectEditor(BlockSkillEffectData* data);
	void DrawSkillEffectEditor(HeroHealSkillEffectData* data);
	void DrawSkillEffectEditor(ManaRestoreSkillEffectData* data);
	void DrawSkillEffectEditor(SlowSkillEffectData* data);
	void DrawSkillEffectEditor(BuffSkillEffectData* data);

	void DrawEnemiesEditor(std::vector<EnemyData>* data);
	void DrawEnemyEditor(EnemyData* data);
	void DrawSpellSequenceEditor(SpellSequenceData* data);
	void DrawSpellListEditor(std::vector<SpellData>* data);
	void DrawSpellEditor(SpellData* data);

	void DrawBuffListEditor(std::vector<BuffData>* data);
	void DrawBuffEditor(BuffData* data);
	void DrawBuffEffectEditor(BuffEffectData* data);
	void DrawBuffEffectEditor(ManaFlowBuffEffectData* data);
	void DrawBuffEffectEditor(DamageFlowBuffEffectData* data);

	void DrawSaveTab(ScenarioData* data);
	bool DrawLoadTab(ScenarioData* data);

	std::string loadStr_;
	std::string loadStatusStr_;
};

} // namespace r0
