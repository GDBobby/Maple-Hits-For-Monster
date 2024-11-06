#pragma once
#include <cstdio>
#include <cstdint>
#include <utility>

struct CharacterStats {
	uint16_t level = 2;
	uint16_t str = 18;
	uint16_t dex = 4;
	uint8_t weaponDamage = 87;
	uint16_t accuracy = 13;
	std::pair<uint16_t, uint16_t> swing;
	std::pair<uint16_t, uint16_t> stab;
	double averageDamage;

	const double stabRatio = 0.4;
	const double swingRatio = 0.6;
	const double attackSpeed = 0.9;
	const double mastery = 0.09;

	void rangeCalculation() {
		swing.first = ((((str * 5)) * mastery) + dex) * weaponDamage / 100; //min
		swing.second = (str * 5 + dex) * weaponDamage / 100; //max
		stab.first = ((((str * 3)) * mastery) + dex) * weaponDamage / 100; //min
		stab.second = (str * 3 + dex) * weaponDamage / 100; //min
		//averageDamage = (static_cast<double>(str) * 5.0 * (mastery + 1.0) / 2.0 + static_cast<double>(dex)) * static_cast<double>(weaponDamage) / 100.0 * 0.6;
		//averageDamage += (static_cast<double>(str) * 3.0 * (mastery + 1.0) / 2.0 + static_cast<double>(dex)) * static_cast<double>(weaponDamage) / 100.0 * 0.4;
		//printf("averageDamage : %.1f \n", averageDamage);

		averageDamage = (static_cast<double>(str) * 2.289 + static_cast<double>(dex)) * static_cast<double>(weaponDamage) / 100.0;
		printf("averageDamage : %.1f \n", averageDamage);

	}
};