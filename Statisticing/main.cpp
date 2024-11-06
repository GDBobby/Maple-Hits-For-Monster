

#include <chrono>

#include "CharacterStats.h"
#include "HPTables.h"
#include "DataCollected.h"

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




struct MonsterStats {
	uint8_t level;
	uint16_t hp;
	uint8_t avoid;
    uint8_t defense;
    uint16_t exp;

    float chanceToHit = 0.0f;
    void getChanceToHit(CharacterStats& charStats) {
        int levelDiff = level - charStats.level;
        double acc100 = 0;
        double acc1 = 0;
        double AccRatio = 0;
        if (levelDiff < 0) {
            levelDiff = 0;
        }
        acc100 = (55.2 + 2.15 * static_cast<double>(levelDiff)) * (static_cast<double>(avoid) / 15.0);
        acc1 = acc100 * 0.5 + 1.0;
        AccRatio = 100 * ((static_cast<double>(charStats.accuracy) - (acc100 * 0.5)) / (acc100 * 0.5));
        
        if (AccRatio > 100.0) {
            AccRatio = 100.0;
        }
        else if (AccRatio < 0.0) {
            AccRatio = 0.0;
        }
        chanceToHit = AccRatio;
    }
    void populateTables(CharacterStats& charStats, uint16_t startingHP) {

        for (int i = 1; i < startingHP - charStats.stab.first + 1; i++) {
            HPTables::addTable( i, charStats);
        }
        HPTables::addTable(startingHP, charStats);
        HPTables::hpTables.at(startingHP).print(startingHP);
    }

    std::vector<double> getHitsToKill(CharacterStats& charStats) {
        std::vector<double> chanceToKillInThisHit;

        uint16_t maximumHits = std::ceil(static_cast<double>(hp) / static_cast<double>(charStats.stab.first));
        printf("maximum hits :%d \n", maximumHits);
        for (uint16_t i = 0; i < maximumHits; i++) {
            chanceToKillInThisHit.push_back(0.0);
        }


        std::unordered_map<uint16_t, HPTables> hpResultsLookup;
        hpResultsLookup.reserve(hp);
        //printf("map size : %d \n", hp);
        double chanceToBeDead = 0.0;

        //calculateNextHit(hpResultsLookup, chanceToKillInThisHit, hp, charStats, 0, 1.0, chanceToBeDead);
        
        return chanceToKillInThisHit;
    }

    float getPerfectEfficiencyEPM(CharacterStats& charStats) {
        //hits to kill
        auto startingTime = std::chrono::high_resolution_clock::now();
        /*
        auto hitsToKill = getHitsToKill(charStats);
        for (int i = 0; i < hitsToKill.size(); i++) {
            printf("chance to kill in hit[%d] : %.3f \n", i + 1, hitsToKill[i]);
        }*/
        populateTables(charStats, hp);

        HPTables& hpTable = HPTables::hpTables.at(hp);
        double experiencePerSecond = 0.0;
        for (int i = 0; i < hpTable.chanceToKill.size(); i++) {
            if (hpTable.chanceToKill[i] > 0.001) {
                experiencePerSecond += static_cast<double>(exp) / (charStats.attackSpeed * (i + 1)) * hpTable.chanceToKill[i];
            }
        }

        //HPTables::hpTables.clear();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startingTime).count();
        printf("microseconds duration : %d \n", duration);

        return experiencePerSecond * 60.0;
    }
    float getAveragePerfectEfficiency(CharacterStats& charStats) {
        double experiencePerSecond = 0.0;
        double timeToDefeatOne = (hp / charStats.averageDamage) / charStats.attackSpeed;
        experiencePerSecond = exp / timeToDefeatOne;

        return experiencePerSecond * 60.0;
    }
};


struct MapData {
    double mapEfficiency;
};

int main() {
    CharacterStats characterStats;
    characterStats.level = 25;
    characterStats.str = 172;
    characterStats.dex = 33;
    characterStats.weaponDamage = 149;
    characterStats.rangeCalculation();
    printf("range : %d ~ %d \n", characterStats.stab.first, characterStats.swing.second);
    printf("full range : %d ~ %d : %d ~ %d \n", characterStats.stab.first, characterStats.stab.second, characterStats.swing.first, characterStats.swing.second);
    printf("average damage : %.1f \n", characterStats.averageDamage);


    MonsterStats orangeMushroom;
    orangeMushroom.level = 3;
    orangeMushroom.hp = 80;
    orangeMushroom.avoid = 0;
    orangeMushroom.exp = 15;

    MonsterStats greenSlime;
    greenSlime.exp = 26;
    greenSlime.hp = 240;

    MonsterStats blueMushroom;
    blueMushroom.level = 20;
    blueMushroom.exp = 35;
    blueMushroom.hp = 350;

    MonsterStats boar;
    boar.level = 25;
    boar.exp = 42;
    boar.hp = 550;

    MonsterStats darkStump;
    darkStump.level = 10;
    darkStump.exp = 18;
    darkStump.hp = 250;

    MonsterStats fireBoar;
    fireBoar.level = 32;
    fireBoar.exp = 60;
    fireBoar.hp = 1000;

    std::map<std::string, MonsterStats*> monsterTable;
    monsterTable.emplace("orangeMush", &orangeMushroom);
    monsterTable.emplace("greenSlime", &greenSlime);
    monsterTable.emplace("blueMush", &blueMushroom);
    monsterTable.emplace("boar", &boar);
    monsterTable.emplace("darkStump", &darkStump);
    monsterTable.emplace("fireBoar", &fireBoar);


    for (auto iter = monsterTable.begin(); iter != monsterTable.end(); iter++) {
		printf("%s perfect efficiency : %.2f \n", iter->first.c_str(), iter->second->getPerfectEfficiencyEPM(characterStats));
		//printf("%s perfect efficiency with average damage : %.2f \n", iter->first.c_str(), iter->second->getAveragePerfectEfficiency(characterStats));
		HPTables::hpTables.clear();
	}

    //orangeMushroom.getPerfectEfficiencyEPM(characterStats, orangeMushroom);
   // orangeMushroom.getPerfectEfficiencyEPM(characterStats);
    //orangeMushroom.getPerfectEfficiencyEPM(characterStats);
    //orangeMushroom.getPerfectEfficiencyEPM(characterStats);
    //orangeMushroom.getPerfectEfficiencyEPM(characterStats);

    MapData smallForestTrail;
    MapData nextMap;
    std::vector<DataCollected> dataCollected;
    DataCollected::populateDataCollected(dataCollected);

    //MapData::calculateMapEfficiency(dataCollected);
}