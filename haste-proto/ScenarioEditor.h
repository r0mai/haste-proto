#pragma once

#include "Scenario.h"

namespace r0 {

class ScenarioEditor {
public:
	ScenarioEditor();

	// returns true if scenario should be reloaded
	bool DrawUI();

	ScenarioData scenario;
};

} // namespace r0
