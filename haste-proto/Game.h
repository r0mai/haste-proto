#pragma once

#include "ImguiDraw.h"
#include "GameState.h"
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
	void DrawHeroWidget();
	void DrawAbilityButtonBar();
	void DrawHealthBar();
	void DrawManaBar();
	void DrawEnemyBar();
	void DrawInfoPanel();

	bool HasEnoughMana(Ability* ability) const;

	void CastAbility(Ability* ability, Enemy* target);

	// returns true if enemy dies
	bool DamageEnemy(Enemy* target, int dmg);

	// returns true if hero dies
	bool DamageHero(int dmg);

	void AdvanceTurn();
	
	static constexpr float kWindowWidth = 1280.0f;
	static constexpr float kWindowHeight = 720.0f;

	// Ability bar
	static constexpr int kAbilitySlots = 8;
	static constexpr float kAbilitySize = 96.0f; // square size
	static constexpr float kAbilityBarWidth = kAbilitySlots * kAbilitySize;
	static constexpr float kAbilityBarHeight = kAbilitySize;
	static constexpr float kAbilityBarX = (kWindowWidth - kAbilityBarWidth) * 0.5f;
	static constexpr float kAbilityBarY = kWindowHeight - kAbilityBarHeight;

	// Health bar
	static constexpr float kHealthBarWidth = 192.0f;
	static constexpr float kHealthBarHeight = 192.0f;
	static constexpr float kHealthBarX = kAbilityBarX - kHealthBarWidth;
	static constexpr float kHealthBarY = kWindowHeight - kHealthBarHeight;

	// Mana bar
	static constexpr float kManaBarWidth = 192.0f;
	static constexpr float kManaBarHeight = 192.0f;
	static constexpr float kManaBarX = kAbilityBarX + kAbilityBarWidth;
	static constexpr float kManaBarY = kWindowHeight - kManaBarHeight;

	// Enemy bar
	static constexpr float kEnemyBarHeight = 192.0f;
	static constexpr float kEnemyBarX = 0.0f;
	static constexpr float kEnemyBarY = 0.0f;
	static constexpr float kEnemyBarWidth = kWindowWidth;

	// Info panel
	static constexpr float kInfoPanelX = kAbilityBarX;
	static constexpr float kInfoPanelY = kEnemyBarHeight;
	static constexpr float kInfoPanelWidth = kAbilityBarWidth;
	static constexpr float kInfoPanelHeight = kWindowHeight - kAbilityBarHeight - kEnemyBarHeight;


	GLFWwindow* window_;
	GameState state_;
};

} // namespace r0
