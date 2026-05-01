#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Pokemon.h"

int main() {
    srand(time(NULL));
    int pkmCount = 0;
    int startChoice = 0;
    int menuChoice = 0;
    int exitCond = 1;
    FILE* fp = fopen("savefile.txt", "r");
    PokemonData* pkmData = loadPkmData(&pkmCount); // 포켓몬 도감 (pokemon.txt 데이터)
    Player* pt = NULL; // 포켓몬 트레이너
    pt = initPlayer();

    // Pokemon Main 테마
    mainTheme();
    if (fp != NULL) {
        while (1) {
            getLine(DEF_LINE_NUM);
            printGrad("\t1. 새로하기  2. 이어하기\n>>");
            if (onlyNumber(&startChoice)) continue;
            if (startChoice == 1) { pt = selectPokemon(pt, pkmData); break; }
            else if (startChoice == 2) { pt = loadPlayerData(pt, pkmData); break; }
            else { getLine(DEF_LINE_NUM); puts(RED "1 ~ 2 중 하나를 선택해주세요." RESET); }
        }
    } else pt = selectPokemon(pt, pkmData);

    while (exitCond) {
        if (pkmMasterLicense(pt)) break;
        getLine(DEF_LINE_NUM);
        printGrad("모험을 진행하시겠습니까?\n");
        printGrad("1. 네  2. 아니요  3. 상점  4. 포켓몬 센터  5. 포켓몬 도감\n>>");
        if (onlyNumber(&menuChoice)) continue;
        switch (menuChoice) {
        case 1: playAdventure(pt, pkmData, pkmCount); break;
        case 2: saveData(pt); exitCond = 0; break;
        case 3: goToShop(pt); break;
        case 4: goToPkmCenter(pt); break;
        case 5: openPokedex(pkmData, &pkmCount); break;
        default: getLine(DEF_LINE_NUM); puts(RED "1 ~ 5 중 하나를 선택해주세요." RESET); continue;
        }
    }

    fclose(fp);
    freeMemory(pt);
    return 0;
}