#include "Game.h"

#include <format>
#include <algorithm>
#include "tinyformat.h"
#include "Overloaded.h"
#include "Serialization.h"

namespace r0 {

void Game::Init(GLFWwindow* window) {
	window_ = window;

	Reload();
}

void Game::Update() {
	float deltaTime = ImGui::GetIO().DeltaTime;
	LogicUpdate(deltaTime);
	DrawHeroWidget();
}

void Game::Reload() {
	state_ = GameState{ editor_.scenario };
	logLines_.clear();

	{
		auto json = Write(editor_.scenario);
		editor_.scenario = {};
		Read(json, editor_.scenario);
		json = Write(editor_.scenario);
		std::cout << json.dump(4) << std::endl;
	}
}

void Game::LogicUpdate(float deltaTime) {
	switch (state_.interactionState) {
	case InteractionState::kChoosingAbility:
		// nothing to do
		break;
	case InteractionState::kAnimatingTurns:
		state_.timeSinceLastTurn += deltaTime;
		while (state_.timeSinceLastTurn >= kTimeBetweenTurns) {
			if (AdvanceTurn()) {
				state_.interactionState = InteractionState::kChoosingAbility;
				break;
			}
			state_.timeSinceLastTurn -= kTimeBetweenTurns;
		}
		break;
	}
}

void Game::DrawHeroWidget() {
	ImGui_SetNextWindowPosition(kAbilityBarX, kAbilityBarY, kAbilityBarWidth, kAbilityBarHeight);
	if (ImGui::Begin("ability-button-bar", nullptr, ImGuiWindowFlags_NoDecoration)) {
		DrawAbilityButtonBar();
	}
	ImGui::End();


	ImGui_SetNextWindowPosition(kHealthBarX, kHealthBarY, kHealthBarWidth, kHealthBarHeight);
	if (ImGui::Begin("health-bar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {
		DrawHealthBar();
	}
	ImGui::End();

	ImGui_SetNextWindowPosition(kManaBarX, kManaBarY, kManaBarWidth, kManaBarHeight);
	if (ImGui::Begin("mana-bar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {
		DrawManaBar();
	}
	ImGui::End();

	ImGui_SetNextWindowPosition(kEnemyBarX, kEnemyBarY, kEnemyBarWidth, kEnemyBarHeight);
	if (ImGui::Begin("enemy-bar", nullptr, ImGuiWindowFlags_NoDecoration)) {
		DrawEnemyBar();
	}
	ImGui::End();

	ImGui_SetNextWindowPosition(kInfoPanelX, kInfoPanelY, kInfoPanelWidth, kInfoPanelHeight);
	if (ImGui::Begin("central-panel", nullptr, ImGuiWindowFlags_NoDecoration)) {
		DrawCentralPanel();
	}
	ImGui::End();

	ImGui_SetNextWindowPosition(kHeroCastBarX, kHeroCastBarY, kHeroCastBarWidth, kHeroCastBarHeight);
	if (ImGui::Begin("hero-cast-bar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {
		DrawHeroCastBar();
	}
	ImGui::End();
}

void Game::DrawAbilityButtonBar() {
	auto& abilities = state_.hero.abilities;
	if (ImGui::BeginTable("ability-table", kAbilitySlots)) {
		for (int i = 0; i < kAbilitySize; ++i) {
			ImGui::TableNextColumn();
			if (i >= abilities.size()) {
				// TODO render empty ability
				continue;
			}

			auto& ability = abilities[i];
			bool isPressed = ImGui_DrawAbility(&ability);
			if (!isPressed) {
				continue;
			}

			if (!HasEnoughMana(&ability)) {
				Log("Not enough mana to cast %s", ability.name);
				continue;
			}

			if (ability.NeedsTarget()) {
				if (state_.targetedEnemyIdx == kNoTarget) {
					Log("%s ability needs a target", ability.name);
					continue;
				}
				StartCastingAbility(i);
			} else {
				StartCastingAbility(i);
			}
		}
		ImGui::EndTable();
	}
}

bool Game::HasEnoughMana(Ability* ability) const {
	return state_.hero.mana >= ability->manaCost;
}


void Game::StartCastingAbility(int abilityIdx) {
	state_.castedAbilityIdx = abilityIdx;
	state_.hero.castTime = 0;
	state_.interactionState = InteractionState::kAnimatingTurns;
	state_.timeSinceLastTurn = 0.0f;
}

void Game::CastAbility(Ability* ability, int targetEnemyIdx) {
	// check for mana again just in case
	if (!HasEnoughMana(ability)) {
		Log("Not enough mana to case %s [when casting!]", ability->name);
		return;
	}

	state_.hero.mana = std::clamp(state_.hero.mana - ability->manaCost, 0, state_.hero.maxMana);

	assert(ability->NeedsTarget() != (targetEnemyIdx == kNoTarget));

	auto& enemies = state_.enemies;
	for (auto& effect : ability->effects) {
		ApplyAbilityEffect(&effect, targetEnemyIdx);
	}
}

void Game::ApplyAbilityEffect(AbilityEffect* effect, int targetEnemyIdx) {
	std::visit(Overloaded{
		[&](const DamageEffect& e) {
			auto& enemies = state_.enemies;
			int beginIdx;
			int endIdx;
			if (e.radius == -1) {
				beginIdx = 0;
				endIdx = enemies.size();
			} else {
				assert(targetEnemyIdx != kNoTarget);
				beginIdx = std::max(0, targetEnemyIdx - e.radius);
				endIdx = std::min(int(enemies.size()), targetEnemyIdx + e.radius + 1);
			}
			for (int i = beginIdx; i < endIdx; ++i) {
				DamageEnemy(&enemies[i], e.damage);
			}
		},
		[&](const BlockEffect& e) {
			ApplyBlock(e.block);
		},
		[&](const HeroHealEffect& e) {
			HealHero(e.heal);
		},
		[&](const ManaRestoreEffect& e) {
			RestoreMana(e.mana);
		},
	}, *effect);
}

bool Game::DamageEnemy(Enemy* target, int dmg) {
	target->hp = std::max(target->hp - dmg, 0);
	return target->hp == 0;
}

bool Game::DamageHero(int dmg) {
	assert(dmg >= 0);
	dmg -= state_.hero.block;
	if (dmg > 0) {
		state_.hero.hp = std::max(state_.hero.hp - dmg, 0);
	}
	state_.hero.block = 0;
	return state_.hero.hp == 0;
}

void Game::HealHero(int heal) {
	assert(heal >= 0);
	state_.hero.hp = std::min(state_.hero.hp + heal, state_.hero.maxHp);
}

void Game::RestoreMana(int mana) {
	assert(mana >= 0);
	state_.hero.mana = std::min(state_.hero.mana + mana, state_.hero.maxMana);
}

void Game::ApplyBlock(int block) {
	assert(block >= 0);
	state_.hero.block += block;
}

void Game::DrawHealthBar() {
	ImGui_HealthBar(state_.hero.hp, state_.hero.maxHp, state_.hero.block);
}

void Game::DrawManaBar() {
	ImGui_ResourceBar(state_.hero.mana, state_.hero.maxMana, kManaBarColor);
}

void Game::DrawEnemyBar() {
	auto& enemies = state_.enemies;
	if (enemies.empty()) {
		return;
	}

	auto windowWidth = ImGui::GetContentRegionAvail().x;
	auto columnWidth = std::min(256.0f, windowWidth / enemies.size());

	float padding = (windowWidth - columnWidth * enemies.size()) / 2.0f;
	ImGui::Indent(padding);

	if (ImGui::BeginTable("enemy-table", int(enemies.size()))) {
		for (int i = 0; i < enemies.size(); ++i) {
			ImGui::TableSetupColumn(std::to_string(i).c_str(), ImGuiTableColumnFlags_WidthFixed, columnWidth);
		}

		for (int i = 0; i < enemies.size(); ++i) {
			auto& enemy = enemies[i];
			ImGui::TableNextColumn();
			bool isSelected = state_.targetedEnemyIdx == i;
			bool enemyClicked = ImGui_DrawEnemy(&enemy, isSelected);
			if (enemyClicked) {
				state_.targetedEnemyIdx = isSelected ? kNoTarget : i;
				state_.hero.castTime = 0;
				state_.castedAbilityIdx = kNoAbility;
			}
		}
		ImGui::EndTable();
	}
}

void Game::DrawCentralPanel() {
	if (ImGui::BeginTabBar("tabs")) {
		if (ImGui::BeginTabItem("Game")) {

			ImGui::Text("Turn %d", state_.turnIdx);
			if (state_.interactionState == InteractionState::kChoosingAbility) {
				if (ImGui::Button("Pass")) {
					AdvanceTurn();
				}
			}

			if (ImGui::BeginChild("log")) {
				for (const auto& line : logLines_) {
					ImGui::TextUnformatted(line.c_str());
				}
			}
			ImGui::EndChild();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Editor")) {
			if (editor_.DrawUI()) {
				Reload();
			}

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void Game::DrawHeroCastBar() {
	if (state_.castedAbilityIdx == kNoAbility) {
		return;
	}
	auto& ability = state_.hero.abilities[state_.castedAbilityIdx];
	ImGui_HorizonalBar(
		kHeroCastBarWidth, kHorizonalBarHeight,
		state_.hero.castTime, ability.castTime,
		kCastTimeColor, ability.name.c_str());
}

bool Game::AdvanceTurn() {
	++state_.turnIdx;

	bool finishedCasting = false;

	if (state_.castedAbilityIdx != kNoAbility) {
		auto& hero = state_.hero;
		++hero.castTime;

		auto& ability = hero.abilities[state_.castedAbilityIdx];

		if (hero.castTime >= ability.castTime) {
			if (ability.NeedsTarget()) {
				if (state_.targetedEnemyIdx != -1) {
					CastAbility(&ability, state_.targetedEnemyIdx);
				} else {
					Log("%s ability needs a target [when casting]", ability.name);
				}
			} else {
				CastAbility(&ability, kNoTarget);
			}
			finishedCasting = true;
			state_.hero.castTime = 0;
			state_.castedAbilityIdx = kNoAbility;
		}
	}

	auto& enemies = state_.enemies;
	for (auto& enemy : enemies) {
		if (enemy.sequence.spells.empty()) {
			continue;
		}
		++enemy.castTime;
		auto& currentSpell = enemy.sequence.spells[enemy.sequence.currentIdx];
		if (enemy.castTime >= currentSpell.castTime) {
			// casting enemy spell
			DamageHero(currentSpell.damage);
			enemy.castTime = 0;
			enemy.sequence.currentIdx = (enemy.sequence.currentIdx + 1) % enemy.sequence.spells.size();
		} 
	}

	return finishedCasting;
}

template<typename... Args>
void Game::Log(const char* format, const Args&... args) {
	logLines_.push_back(tfm::format(format, args...));
}

} // namespace r0
