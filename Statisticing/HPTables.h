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

class HPTables {
public:
    //double chanceToKill;
    //FOR SPEEDUP
    //need chanceToKillInThisStep, nextStep, nextStep, nextStep;
    std::vector<double> chanceToKill;

    static std::unordered_map<uint16_t, HPTables> hpTables;

    //only works bottom to top
    static void addTable(uint16_t startingHP, CharacterStats& charStats) {
        //printf("starting hp : %d \n", startingHP);
        //this only works if going from bottom to top

        HPTables hpTable;

        // printf("beginning table construction : %d \n", startingHP);
        hpTable.chanceToKill.resize(std::ceil(static_cast<double>(startingHP) / static_cast<double>(charStats.stab.first)), 0.0);
        //amount of damage vlaues that will kill
        int16_t totalStabDamageValues = charStats.stab.second - charStats.stab.first + 1;
        int16_t stabKillValueCount = (charStats.stab.second - startingHP + 1);
        if (stabKillValueCount < 0) {
            stabKillValueCount = 0;
        }
        //amount of damage values that can be survived
        int16_t stabSurviveValueCount = totalStabDamageValues - stabKillValueCount;

        //if enemy still alive
        int16_t maxRemainingHPFromStab = startingHP - charStats.stab.first;
        int16_t minRemainingHPFromStab = startingHP - charStats.stab.second;
        //stab
        if (stabKillValueCount > 0) {
            hpTable.chanceToKill[0] += 0.4 * static_cast<double>(stabKillValueCount) / static_cast<double>(totalStabDamageValues);
            minRemainingHPFromStab = 1;
        }

        //amount of damage vlaues that will kill
        if (charStats.swing.first >= startingHP) {
            hpTable.chanceToKill[0] += 0.6;
            //printf("swing kills 100% \n");

            //only calculate stabs that dont kill here
            //already checked if stab 100% kills, so it does NOT here, for sure
            double chanceForEachStabNotKilling = 1.0 / static_cast<double>(stabSurviveValueCount) * (1.0 - hpTable.chanceToKill[0]) * .4;
            for (uint16_t dmg = charStats.stab.first; dmg < startingHP; dmg++) {
                //printf("ending hp : %d \n", startingHP - i);
                auto& childTable = hpTables.at(startingHP - dmg);
                for (int i = 0; i < childTable.chanceToKill.size(); i++) {
                    hpTable.chanceToKill[i + 1] += childTable.chanceToKill[i] * chanceForEachStabNotKilling;
                }
                //resultingHPs.emplace_back(startingHP - dmg, chanceForEachStabNotKilling);
            }

        }
        else {
            int16_t totalSwingDamageValues = charStats.swing.second - charStats.swing.first + 1;
            int16_t swingKillValueCount = (charStats.swing.second - startingHP + 1);
            if (swingKillValueCount < 0) {
                swingKillValueCount = 0;
            }
            //amount of damage values that can be survived
            int16_t swingSurviveValueCount = totalSwingDamageValues - swingKillValueCount;

            //if enemy still alive
            int16_t maxRemainingHPFromSwing = startingHP - charStats.swing.first;
            int16_t minRemainingHPFromSwing = startingHP - charStats.swing.second;
            //swing
            if (swingKillValueCount > 0) {
                hpTable.chanceToKill[0] += 0.6 * static_cast<double>(swingKillValueCount) / static_cast<double>(totalSwingDamageValues);
                minRemainingHPFromSwing = 1;
            }

            //calculate chance to get a damage value from from stabMaxHP to swingMaxHP, this will be only swings
            //calculate from swingMaxHP to stabMinHP, this will be from both swings and stabs
            //if(stabMinHP > 1, aka stab can kill, or leave at 1 hp){
                //calculate chance to hit damage that can ONLY be reached by swings, without killing
            //}
            /*
            step 1: stab min to swing to min, 0.4 multiplier
            step 2: stab AND swing values, these have a 1.0 multiplier
            step 3: swing only values that don't kill, 0.6 multiplier
            */


            double chanceForEachStabNotKilling = (1.0 - hpTable.chanceToKill[0]) * .4 / static_cast<double>(stabSurviveValueCount);
            double chanceForEachSwingNotKilling = (1.0 - hpTable.chanceToKill[0]) * .6 / static_cast<double>(swingSurviveValueCount);

            for (int remainingHP = maxRemainingHPFromStab; remainingHP > maxRemainingHPFromSwing; remainingHP--) {
                auto& childTable = hpTables.at(remainingHP);
                for (int i = 0; i < childTable.chanceToKill.size(); i++) {
                    hpTable.chanceToKill[i + 1] += childTable.chanceToKill[i] * chanceForEachStabNotKilling;
                }
                //resultingHPs.emplace_back(remainingHP, chanceForEachStabNotKilling);
            }
            for (int remainingHP = maxRemainingHPFromSwing; remainingHP >= minRemainingHPFromStab; remainingHP--) {
                auto& childTable = hpTables.at(remainingHP);
                for (int i = 0; i < childTable.chanceToKill.size(); i++) {
                    hpTable.chanceToKill[i + 1] += childTable.chanceToKill[i] * (chanceForEachStabNotKilling + chanceForEachSwingNotKilling);
                }
                //resultingHPs.emplace_back(remainingHP, chanceForEachStabNotKilling + chanceForEachSwingNotKilling);
            }
            for (int remainingHP = minRemainingHPFromStab - 1; remainingHP >= minRemainingHPFromSwing; remainingHP--) {
                auto& childTable = hpTables.at(remainingHP);
                for (int i = 0; i < childTable.chanceToKill.size(); i++) {
                    hpTable.chanceToKill[i + 1] += childTable.chanceToKill[i] * chanceForEachSwingNotKilling;
                }
                //resultingHPs.emplace_back(remainingHP, chanceForEachSwingNotKilling);
            }


            //printf("breakpoint pls \n");
        }
        hpTables.emplace(startingHP, hpTable);
    }

    HPTables() {}
    void print(uint16_t startingHP) {
        printf("chance to kill at startingHP : %d \n", startingHP);
        
        for (int i = 0; i < chanceToKill.size(); i++) {
            printf("\t %d:%.7f  \n", i, chanceToKill[i]);
        }
        //printf("\t %.7f chanceToKill \n", chanceToKill);
        //printf("\t ");
        //printf("\n");
    }
};
