

#include <chrono>

#include "CharacterStats.h"
#include "HPTables.h"

#include <chrono>
#include <random>

/*
* ayumilove
1 DEX = 0.8 accuracy and 1 LUK = 0.5 accuracy
Accuracy = (DEX*0.8) + (LUK*0.5) + accuracy from mastery and equipment
Acc Needed
100% hit accuracy = (55+2*lvl difference)*mob avoid/15
*/

/* MrSoupMan, github.com/mrsoupman
    function doSomeMath(){
        int monLevel = parseInt(document.getElementById('mobLevel').value);
        var monAvoid = parseInt(document.getElementById('mobAvoid').value);
        var charLevel = parseInt(document.getElementById('level').value);
        var charMainStat = parseInt(document.getElementById('mainstat').value);
        var charLuk = parseInt(document.getElementById('luk').value);
        var diff = monLevel - charLevel;
        var acc100 = 0;
        var acc1 = 0;
        var AccRatio = 0;
        if(diff < 0)
            diff = 0;
        if(document.querySelector('input[name="type"]:checked').id == 'physical') {
            acc100 = (55.2 + 2.15 * diff) * (monAvoid/15.0)
            acc1 = acc100 * 0.5 + 1;
            AccRatio = 100 * ((charMainStat - (acc100 * 0.5) ) / (acc100 * 0.5));
        }
        else {
            var curAcc=( Math.floor(charMainStat/10) + Math.floor(charLuk/10) );
            acc100 = Math.floor((monAvoid + 1.0)*(1.0 + (0.04 * diff)));
            acc1 = Math.round(0.41*acc100);
            AccPart = (curAcc-acc1+1)/(acc100-acc1+1);
            AccRatio = ((-0.7011618132 * Math.pow(AccPart,2)) + (1.702139835 * AccPart)) * 100;
        }
        if(AccRatio > 100)
            AccRatio = 100;
            else if(AccRatio < 0)
            AccRatio = 0;
        document.getElementById('mob1acc').value = acc1.toPrecision(3);
        document.getElementById('mob100acc').value = acc100.toPrecision(3);
        document.getElementById('mobRate').value = AccRatio.toPrecision(3) + "%";
    }
*/

void PopulateTablesSwingStab(CharacterStats& charStats, uint16_t startingHP) {
    //std::unordered_map<uint16_t, std::vector<double>> hpTables{};


    //for (int i = 1; i < startingHP - charStats.weak.min + 1; i++) {
    //    hpTables(hpTables, i, charStats);

    //}
    //HPTables::AddTable(hpTables, startingHP, charStats);
}


int32_t max_pl(int32_t a, int32_t b) {
    if (a > b) {
        return a;
    }
    return b;
}

double Standard_Normal_CDF(double z) {
    return 0.5 * (1.0 + std::erf(z / std::sqrt(2.0)));
}

void ProbabilityAlgorithm(CharacterStats& charStats, MonsterStats& monStats) {

    const double levelDiff = static_cast<double>(max_pl(static_cast<int>(monStats.level) - static_cast<int>(charStats.level), 0));

    charStats.normal.max = (static_cast<double>(charStats.normal.max) * charStats.skillPercentage / 100.0) * (1.0 - 0.01 * levelDiff) - static_cast<double>(monStats.defense) * 0.5;
    charStats.normal.min = (static_cast<double>(charStats.normal.min) * charStats.skillPercentage / 100.0) * (1.0 - 0.01 * levelDiff) - static_cast<double>(monStats.defense) * 0.6;
    printf("defense range - %d:%d\n", charStats.normal.min, charStats.normal.max);

    const uint32_t avgDmg = (charStats.normal.max + charStats.normal.min) / 2;

    const int maxHitsToKill = std::ceil(static_cast<double>(monStats.hp) / static_cast<double>(charStats.normal.min));
    const int minHitsToKill = std::ceil(static_cast<double>(monStats.hp) / static_cast<double>(charStats.normal.max));
    const double expectedHits = static_cast<double>(monStats.hp) / static_cast<double>(avgDmg);

    std::vector<double> probabilities{};
    probabilities.resize(maxHitsToKill - minHitsToKill + 1, 0.0);
    for (int i = minHitsToKill; i <= maxHitsToKill; i++) {
        const double meanDmg = static_cast<double>(avgDmg * i);
        const double hitVariance = static_cast<double>(i) * static_cast<double>(std::pow(charStats.normal.max - charStats.normal.min, 2)) / 12.0;
        const double hitStdDev = std::sqrt(hitVariance);
        probabilities[i - minHitsToKill] = 1.0 - Standard_Normal_CDF((static_cast<double>(monStats.hp) - meanDmg) / hitStdDev);
    }
    for (int i = 0; i < probabilities.size(); i++) {
        if (i != 0) {
            printf("probability[%d] : %.3f\n", i + minHitsToKill, (probabilities[i] - probabilities[i - 1]) * 100.0);
        }
        else {
            printf("probability[%d] : %.3f\n", i + minHitsToKill, probabilities[i] * 100.0);
        }
    }
}

int main() {
    CharacterStats characterStats{};
    characterStats.level = 60;
    characterStats.mainStat = 251;
    characterStats.subStat = 105;
    characterStats.skillPercentage = 110.0;
    characterStats.weaponDamage = 81 + 20;
    characterStats.CalculateRange();

    MonsterStats monStats{};
    monStats.hp = 3700;
    monStats.exp = 10;
    monStats.level = 59;
    monStats.defense = 180;

    auto startingTime = std::chrono::high_resolution_clock::now();
    HPTables::AddTable(monStats, characterStats);
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startingTime).count();
    printf("microseconds duration : %zu \n", duration);

    //MonsterStats skeleStats{};
    //skeleStats.hp = 4500;
    //skeleStats.level = 57;
    //skeleStats.defense = 160;
    //startingTime = std::chrono::high_resolution_clock::now();
    //HPTables::AddTable(skeleStats, characterStats);
    //duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startingTime).count();
    //printf("microseconds duration : %zu \n", duration);

    characterStats.CalculateRange();
    startingTime = std::chrono::high_resolution_clock::now();
    ProbabilityAlgorithm(characterStats, monStats);
    duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startingTime).count();
    printf("microseconds duration : %zu \n", duration);

    characterStats.ForceRange(359, 722);


    //MapData::calculateMapEfficiency(dataCollected);
}