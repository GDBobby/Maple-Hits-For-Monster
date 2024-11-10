#include "HPTables.h"

#ifdef _DEBUG
#include <cassert>
#endif

int32_t min_pls(int32_t a, int32_t b) {
    if (a < b) {
        return a;
    }
    return b;
}

int32_t max_pls(int32_t a, int32_t b) {
    if (a > b) {
        return a;
    }
    return b;
}


namespace HPTables {
    void FillHitChances(std::vector<std::vector<double>>& hpTables, uint32_t currentHP, DamageRange const& damageRange) {

        const int16_t damageRangeCount = damageRange.max - damageRange.min + 1;

        const uint32_t tableIndex = min_pls((currentHP - 1), hpTables.size() - 1);
        auto& hitKillChance = hpTables[tableIndex];
#ifdef _DEBUG
        assert(hitKillChance.size() == 0);
#endif
        hitKillChance.resize(std::ceil(static_cast<double>(currentHP) / static_cast<double>(damageRange.min)), 0.0);

        const int16_t killValueCountTemp = damageRange.max - currentHP + 1;
        const int16_t killValueCount = killValueCountTemp * (killValueCountTemp > 0);
        const int16_t survivalCount = damageRangeCount - killValueCount;

        const int16_t maxRemainingHP = currentHP - damageRange.min;
        int16_t minRemainingHP = currentHP - damageRange.max;
        if (killValueCount > 0) {
            hitKillChance[0] += static_cast<double>(killValueCount) / static_cast<double>(damageRangeCount);
            minRemainingHP = 1;
        }
        const double chanceForEachHitNotKilling = (1.0 - hitKillChance[0]) / static_cast<double>(survivalCount);
        for (int remainingHP = minRemainingHP; remainingHP <= maxRemainingHP; remainingHP++) {

            auto& remainingHPChance = hpTables[remainingHP - 1]; //this should never be less than 1
            for (int hitCount = 0; hitCount < remainingHPChance.size(); hitCount++) {
                hitKillChance[hitCount + 1] += remainingHPChance[hitCount] * chanceForEachHitNotKilling;
            }

        }
    }

    std::vector<double> AddTable(MonsterStats& monsterStats, CharacterStats& charStats) {
        //ignorign the original hp
        //we only need to consider hits from the remaining hp after one hit
        // and the hp possible that isn't a 1 shot (below minimum damage)
        //this is all possible hp values between 1 shot and hit once
        double levelDiff = static_cast<double>(max_pls(static_cast<int>(monsterStats.level) - static_cast<int>(charStats.level), 0));

        charStats.normal.max = (charStats.normal.max * charStats.skillPercentage / 100.0) * (1.0 - 0.01 * levelDiff) - static_cast<double>(monsterStats.defense) * 0.5;
        charStats.normal.min = (charStats.normal.min * charStats.skillPercentage / 100.0) * (1.0 - 0.01 * levelDiff) - static_cast<double>(monsterStats.defense) * 0.6;


        std::vector<std::vector<double>> hpTables((monsterStats.hp - charStats.normal.min + 1), std::vector<double>{});

        //chance for each amount of hits to kill
        

        for (int currentHP = 1; currentHP <= (monsterStats.hp - charStats.normal.min); currentHP++) { //change later
            FillHitChances(hpTables, currentHP, charStats.normal);
        }
        FillHitChances(hpTables, monsterStats.hp, charStats.normal);
#ifdef _DEBUG
        double expPerHit = 0.0;
        for (int i = 0; i < hpTables.back().size(); i++) {
            if (hpTables.back()[i] > 0.000001) {
                printf("\t %d:%.4f  \n", i + 1, hpTables.back()[i] * 100.0);
            }
            expPerHit += static_cast<double>(monsterStats.exp) * hpTables.back()[i] / static_cast<double>(i + 1);
        }
        printf("exp per hit : %.5f\n", expPerHit);
#endif
        return hpTables.back();
    }

    void AddTableSwingStab(std::unordered_map<uint16_t, std::vector<double>>& hpTables, uint16_t startingHP, CharacterStats& charStats) {

    }
} //namespace HPTables


extern "C" __declspec(dllexport) double* AddTable_C(MonsterStats monsterStats, CharacterStats charStats, int* outSize) {
    auto result = HPTables::AddTable(monsterStats, charStats);

    *outSize = static_cast<int>(result.size());
    double* output = new double[*outSize];

    memcpy(output, result.data(), *outSize);

    return output;
}