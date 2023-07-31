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
	void DrawAbilityBar();

	GLFWwindow* window_ = nullptr;
	float windowWidth_ = 0.0f;
	float windowHeight_ = 0.0f;

	GameState state_;
};

} // namespace r0
