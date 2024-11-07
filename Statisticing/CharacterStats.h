#pragma once
#include <cstdio>
#include <cstdint>
#include <utility>

struct DamageRange {
	uint16_t min;
	uint16_t max;
};

struct CharacterStats {
	uint16_t level = 2;
	uint16_t mainStat = 18;
	uint16_t subStat = 4;
	uint8_t weaponDamage = 87;
	uint16_t accuracy = 13;
	DamageRange normal; //swing for pole arms
	DamageRange weak; //stab for polearms
	double averageDamage;
	double skillPercentage = 100.0;

	const double stabRatio = 0.4;
	const double swingRatio = 0.6;
	double mastery = 0.1;
	double statMulti = 5.0;
	double statMultiLow = 3.0;

	void RangeCalculation() {
		normal.min = 336;
		normal.max = 691;

	}

};

struct MonsterStats {
	uint8_t level = 1;
	uint16_t hp = 1;
	uint8_t avoid = 0;
	uint16_t defense = 0;
	uint16_t exp = 3;
};