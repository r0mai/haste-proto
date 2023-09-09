#pragma once

#include "ImguiDraw.h"
#include "GameState.h"
#include "ScenarioDataEditor.h"
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

namespace r0 {

class Game {
public:
	void Init(GLFWwindow* window);

	void Update();

private:
	void Reload();

	void LogicUpdate(float delatTime);

	void DrawHeroWidget();
	void DrawSkillButtonBar();
	void DrawHealthBar();
	void DrawManaBar();
	void DrawEnemyBar();
	void DrawCentralPanel();
	void DrawHeroCastBar();

	bool HasEnoughMana(Skill* skill) const;

	void StartCastingSkill(int skillIdx);
	void CastSkill(Skill* skill, int targetEnemyIdx);
	void ApplySkillEffect(SkillEffect* effect, int targetEnemyIdx);

	// returns true if enemy dies
	bool DamageEnemy(Enemy* target, int dmg);

	// returns true if hero dies
	bool DamageHero(int dmg);

	void HealHero(int heal);

	void RestoreMana(int mana);

	void Slow(Enemy* target, int slow);

	void ApplyBlock(int block);

	// returns true if hero's casting is finished
	bool AdvanceTurn();

	template<typename... Args>
	void Log(const char* format, const Args&... args);

	static constexpr float kTimeBetweenTurns = 0.5f; // seconds
	
	static constexpr float kWindowWidth = 1280.0f;
	static constexpr float kWindowHeight = 720.0f;

	// Skill bar
	static constexpr int kSkillSlots = 8;
	static constexpr float kSkillSize = 96.0f; // square size
	static constexpr float kSkillBarWidth = kSkillSlots * kSkillSize;
	static constexpr float kSkillBarHeight = kSkillSize;
	static constexpr float kSkillBarX = (kWindowWidth - kSkillBarWidth) * 0.5f;
	static constexpr float kSkillBarY = kWindowHeight - kSkillBarHeight;

	// Hero cast bar
	static constexpr float kHeroCastBarWidth = 256.0f + 4.0f;
	static constexpr float kHeroCastBarHeight = kHorizonalBarHeight + 16.0f;
	static constexpr float kHeroCastBarX = (kWindowWidth - kHeroCastBarWidth) * 0.5f;
	static constexpr float kHeroCastBarY = kSkillBarY - kHeroCastBarHeight - kHeroCastBarHeight;


	// Health bar
	static constexpr float kHealthBarWidth = 192.0f;
	static constexpr float kHealthBarHeight = 192.0f;
	static constexpr float kHealthBarX = kSkillBarX - kHealthBarWidth;
	static constexpr float kHealthBarY = kWindowHeight - kHealthBarHeight;

	// Mana bar
	static constexpr float kManaBarWidth = 192.0f;
	static constexpr float kManaBarHeight = 192.0f;
	static constexpr float kManaBarX = kSkillBarX + kSkillBarWidth;
	static constexpr float kManaBarY = kWindowHeight - kManaBarHeight;

	// Enemy bar
	static constexpr float kEnemyBarHeight = 192.0f;
	static constexpr float kEnemyBarX = 0.0f;
	static constexpr float kEnemyBarY = 0.0f;
	static constexpr float kEnemyBarWidth = kWindowWidth;

	// Info panel
	static constexpr float kInfoPanelX = kSkillBarX;
	static constexpr float kInfoPanelY = kEnemyBarHeight;
	static constexpr float kInfoPanelWidth = kSkillBarWidth;
	static constexpr float kInfoPanelHeight = kHeroCastBarY - kEnemyBarHeight;


	GLFWwindow* window_;
	ScenarioDataEditor editor_;
	GameState state_;

	std::vector<std::string> logLines_;
};

} // namespace r0
