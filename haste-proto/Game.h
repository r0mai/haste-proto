#pragma once

#include "GameState.h"
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

namespace r0 {

class Game {
public:
	void Init(GLFWwindow* window);
	void Update();

private:
	void DrawHeroWidget();
	void DrawAbilityButtonBar();
	void DrawHealthBar();

	static constexpr int kAbilitySlots = 8;
	static constexpr float kAbilitySize = 96.0f; // square size
	static constexpr float kHealthBarWidth = 128.0f;
	static constexpr float kHealthBarHeight = 128.0f;
	static constexpr float kManaBarWidth = 192.0f;
	static constexpr float kManaBarHeight = 192.0f;

	GLFWwindow* window_ = nullptr;
	float windowWidth_ = 0.0f;
	float windowHeight_ = 0.0f;

	GameState state_;
};

} // namespace r0
