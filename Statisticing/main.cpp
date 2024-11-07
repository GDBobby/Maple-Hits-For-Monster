

#include <chrono>

#include "CharacterStats.h"
#include "HPTables.h"

#include <chrono>

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

int main() {
    CharacterStats characterStats{};
    characterStats.level = 62;
    characterStats.RangeCalculation();

    MonsterStats monStats{};
    monStats.hp = 5500;
    monStats.exp = 10;
    monStats.level = 62;
    monStats.defense = 0;

    auto startingTime = std::chrono::high_resolution_clock::now();
    HPTables::AddTable(monStats, characterStats);
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startingTime).count();
    printf("microseconds duration : %l \n", duration);


    //MapData::calculateMapEfficiency(dataCollected);
}