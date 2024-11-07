#pragma once

#include <cstdint>
#include <utility>
#include <cstdio>
#include <vector>
#include <cmath>
#include <map>
#include <iostream>
#include <unordered_map>

#include "CharacterStats.h"

extern "C" __declspec(dllexport) double* AddTable_C(MonsterStats monsterStats, CharacterStats charStats, int* outSize);

namespace HPTables {

    std::vector<double> AddTable(MonsterStats& monsterStats, CharacterStats& charStats);
    void AddTableSwingStab(std::unordered_map<uint16_t, std::vector<double>>& hpTables, uint16_t startingHP, CharacterStats& charStats);
}// namespace HPTables

