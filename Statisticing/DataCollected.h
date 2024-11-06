#pragma once
#include "CharacterStats.h"
#include <vector>

struct DataCollected {
	CharacterStats charStats;

	uint16_t experiencePerMinute;
	double perfectExperiencePerMinute;
	double efficiency;

	static void populateDataCollected(std::vector<DataCollected>& dataCollected) {
		dataCollected.emplace_back();
		{
			auto& data = dataCollected.back();
			data.charStats.str = 37;
			data.charStats.dex = 5;
			data.charStats.weaponDamage = 87;
			data.experiencePerMinute = 318;
			data.perfectExperiencePerMinute = 708.45;
			data.efficiency = static_cast<double>(data.experiencePerMinute) / data.perfectExperiencePerMinute;
			printf("efficiency : %.3f \n", data.efficiency);
		}
		dataCollected.emplace_back();
		{
			auto& data = dataCollected.back();
			data.charStats.str = 47;
			data.charStats.dex = 5;
			data.charStats.weaponDamage = 87;
			data.experiencePerMinute = 345;
			data.perfectExperiencePerMinute = 785;
			data.efficiency = static_cast<double>(data.experiencePerMinute) / data.perfectExperiencePerMinute;
			printf("efficiency : %.3f \n", data.efficiency);
		}

	}
};



