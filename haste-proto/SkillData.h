#include "ExpandedVariant.h"
#include "Serialization.h"

namespace r0 {

struct DamageSkillEffectData {
	static constexpr const char* kName = "DamageSkillEffect";

	int damage = 0;

	// 0: just the target
	// -1: AOE
	int radius = 0;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.damage, "damage");
		visitor.Visit(self.radius, "radius");
	}
};

struct BlockSkillEffectData {
	static constexpr const char* kName = "BlockSkillEffect";

	int block = 0;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.block, "block");
	}
};

struct HeroHealSkillEffectData {
	static constexpr const char* kName = "HeroHealSkillEffect";

	int heal = 0;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.heal, "heal");
	}
};

struct ManaRestoreSkillEffectData {
	static constexpr const char* kName = "ManaRestoreSkillEffect";

	int mana = 0;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.mana, "mana");
	}
};

// increases cast time of current spell of the enemy
struct SlowSkillEffectData {
	static constexpr const char* kName = "SlowSkillEffect";

	int slow = 0;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.slow, "slow");
	}
};

struct BuffSkillEffectData {
	static constexpr const char* kName = "BuffSkillEffect";

	std::string buffName;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.buffName, "buffName");
	}
};

using SkillEffectData = ExpandedVariant<
	DamageSkillEffectData,
	BlockSkillEffectData,
	HeroHealSkillEffectData,
	ManaRestoreSkillEffectData,
	SlowSkillEffectData,
	BuffSkillEffectData
>;

struct SkillData {
	std::string name;
	int castTime = 1;
	int manaCost = 10;

	std::vector<SkillEffectData> effects;

	template<typename Self, typename Visitor>
	static void ApplyVisitor(Self& self, Visitor& visitor) {
		visitor.Visit(self.name, "name");
		visitor.Visit(self.castTime, "castTime");
		visitor.Visit(self.manaCost, "manaCost");
		visitor.Visit(self.effects, "effects");
	}
};

} // namespace r0
