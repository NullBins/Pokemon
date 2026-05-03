#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Pokemon.h"

// Load the Pokemon Data
PokemonData* loadPkmData(int* count) {
    int pkmType;
    FILE* fp = fopen("pokemon.txt", "r");
    if (fp == NULL) { puts(RED "pokemon.txt 읽기 실패!" RESET); exit(1); };
    fscanf(fp, "%d", count);
    PokemonData* pkmData = (PokemonData*)malloc(sizeof(PokemonData) * (*count));
    for(int i = 0; i < (*count); i++) {
        pkmData[i].id = i + 1;
        fscanf(fp, "%s %d %d %d", pkmData[i].name, &pkmType, &pkmData[i].minHealth, &pkmData[i].minAttack);
        pkmData[i].pkmType = pkmType;
    }
    fclose(fp);
    return pkmData;
}

// Load the Player Data
Player* loadPlayerData(Player* pt, PokemonData* pkmData) {
    char buf[256];
    int idIndex = 0;
    FILE* fp = fopen("savefile.txt", "r");
    if (fp == NULL) { puts(RED "savefile.txt 읽기 실패!" RESET); exit(1); };
    if (fgets(buf, sizeof(buf), fp) != NULL) {
        sscanf(buf, "포켓몬 개수: %d, 돈: %d, 몬스터볼: %d, 회복아이템: %d\n",
            &pt->pkmCount, &pt->money, &pt->pkmBalls, &pt->healItems);
    }
    for(int i = 0; i < pt->pkmCount; i++) {
        if (fgets(buf, sizeof(buf), fp) != NULL) {
            pt->party[i] = (Pokemon*)malloc(sizeof(Pokemon));
            sscanf(buf, "ID: %d, 닉네임: %[^,], 체력: %d, 최대체력: %d, 공격력: %d\n",
            &idIndex, pt->party[i]->nickname, &pt->party[i]->health,
            &pt->party[i]->maxHealth, &pt->party[i]->attack);
            pt->party[i]->pkmData = &pkmData[idIndex - 1];
        }
    }
    fclose(fp);
    return pt;
}

// Save the Play Data
void saveData(Player* pt) {
    FILE *fp = fopen("savefile.txt", "w");
    if (fp == NULL) { puts(RED "게임 저장 실패!" RESET); exit(1); }
    fprintf(fp, "포켓몬 개수: %d, 돈: %d, 몬스터볼: %d, 회복아이템: %d\n",
        pt->pkmCount, pt->money, pt->pkmBalls, pt->healItems);
    for(int i = 0; i < pt->pkmCount; i++) {
        fprintf(fp, "ID: %d, 닉네임: %s, 체력: %d, 최대체력: %d, 공격력: %d\n",
            pt->party[i]->pkmData->id, pt->party[i]->nickname, pt->party[i]->health,
            pt->party[i]->maxHealth, pt->party[i]->attack);
    }
    puts(GREEN "게임이 저장되었습니다!" RESET);
    fclose(fp);
}

// Player Initialization
Player* initPlayer() {
    Player* pt = (Player*)malloc(sizeof(Player));
    pt->party = (Pokemon**)malloc(sizeof(Pokemon*) * MAX_PARTY);
    // 최대 6마리 저장을 위해 2중 포인터 사용
    pt->money = 10000;
    pt->pkmCount = 0;
    pt->pkmBalls = 0;
    pt->healItems = 0;
    return pt;
}

// Add Pokemon
Pokemon* addPokemon(PokemonData* pkmData, const char* nickname) {
    Pokemon* pkm = (Pokemon*)malloc(sizeof(Pokemon));
    pkm->pkmData = pkmData;
    pkm->attack = pkmData->minAttack + (rand() % 101); // 최소 공격력 + 0 ~ 100
    pkm->maxHealth = pkmData->minHealth + (rand() % 151); // 최소 체력 + 0 ~ 150
    pkm->health = pkm->maxHealth;
    strcpy(pkm->nickname, nickname);
    return pkm;
}

Pokemon* catchPkmName(Pokemon* catchPkm) {
    char nickname[20];
    char answer;
    while (1) {
        getLine(DEF_LINE_NUM);
        printGrad("별명을 정하시겠습니까? (Y/N)\n>>"); scanf(" %c", &answer);
        while(getchar() != '\n');
        if (answer == 'Y' || answer == 'y') {
            getLine(DEF_LINE_NUM);
            printf("%s의 별명을 정해주세요!\n", catchPkm->nickname);
            printGrad(">>");
            fgets(nickname, sizeof(nickname), stdin);
            nickname[strcspn(nickname, "\n")] = 0;
            strcpy(catchPkm->nickname, nickname);
            return catchPkm;
        } else if (answer == 'N' || answer == 'n') return catchPkm;
        else { getLine(DEF_LINE_NUM); puts(RED "(Y/N) 중 하나를 선택해주세요." RESET); continue; }
    }
}

Pokemon* changePokemon(Player* pt, Pokemon* battlePkm) {
    int aliveCond;
    if (battlePkm->health > 0) return battlePkm;
    while (1) {
        aliveCond = alivePkmParty(pt);
        if (aliveCond == -1) return NULL;
        if (aliveCond == -2) {
            getLine(DEF_LINE_NUM); puts(RED "더 이상 싸울 수 있는 포켓몬이 없습니다 ..." RESET);
            puts(GRAY "눈 앞이 깜깜해졌다 ..." RESET);
            if (pt->money > 0 && pt->money < 1000) {
                printf("(%d원을 잃었습니다)\n", pt->money);
                pt->money = 0;
                return NULL;
            } else if (pt->money == 0) {
                return NULL;
            }
            puts(GRAY "(1000원을 잃었습니다)" RESET);
            pt->money -= 1000;
            return NULL;
        }
        if (pt->party[aliveCond]->health != 0) {
            battlePkm = pt->party[aliveCond];
            return battlePkm;
        }
    }
    return NULL;
}

// Select the Pokemon
Player* selectPokemon(Player* pt, PokemonData* pkmData) {
    int pkmChoice = 0;
    while (1) {
        getLine(DEF_LINE_NUM);
        printGrad("어느 포켓몬을 선택하시겠습니까?\n");
        printGrad("\t1. 파이리 2. 이상해씨 3. 꼬부기\n>>");
        if (onlyNumber(&pkmChoice)) continue;
        if (pkmChoice == 1 || pkmChoice == 2 || pkmChoice == 3) {
            pt->party[pt->pkmCount++] = addPokemon(&pkmData[pkmChoice - 1], pkmData[pkmChoice - 1].name);
            break;
        } else { getLine(DEF_LINE_NUM); puts(RED "포켓몬 1 ~ 3 중 하나를 선택해주세요." RESET); }
    }
    return pt;
}

// Attack to Pokemon
void addDamage(Pokemon* attackPkm, Pokemon* defensePkm) {
    double atkDamage = attackPkm->attack;
    int atkPkmType = attackPkm->pkmData->pkmType;
    int dfsPkmType = defensePkm->pkmData->pkmType;
    if ((atkPkmType + 1) % 3 == dfsPkmType) {
        puts(GREEN "효과가 굉장했다!" RESET);
        atkDamage *= 1.5;
    } else if ((dfsPkmType + 1) % 3 == atkPkmType) {
        puts(GRAY "효과가 별로인 듯 하다." RESET);
        atkDamage *= 0.5;
    }
    if ((rand() % 101) <= 20) {
        puts(RED "급소에 맞았다!" RESET);
        atkDamage *= 1.5;
    }
    defensePkm->health -= atkDamage;
    printf("%s는 %d의 데미지를 입었다.\n", defensePkm->nickname, (int)(atkDamage));
    if (defensePkm->health < 0) {
        defensePkm->health = 0;
        printf("%s는 쓰러졌다.\n", defensePkm->nickname);
    }
}

// Replace a Healthy Pokemon
int alivePkmParty(Player* pt) {
    int allHpSum = 0;
    int pkmChoice = 0;
    for(int i = 0; i < pt->pkmCount; i++) {
        allHpSum += pt->party[i]->health;
    }
    if (allHpSum == 0) return -2;
    while(1) {
        getLine(DEF_LINE_NUM);
        printGrad("어느 포켓몬을 내보낼까?\n");
        for(int i = 0; i < pt->pkmCount; i++) {
            if (pt->party[i]->health > 0) {
                printf("%d. %s %s %d/%d\n", i+1, pt->party[i]->nickname,
                pkmTypeString(pt->party[i]->pkmData->pkmType),
                pt->party[i]->health, pt->party[i]->maxHealth);
            }
        }
        printGrad("0. 도망가기\n>>");
        if (onlyNumber(&pkmChoice)) continue;
        if (pkmChoice < 0 || pkmChoice > pt->pkmCount) {
            getLine(DEF_LINE_NUM); puts(RED "사용 가능한 포켓몬 중 하나를 선택해주세요." RESET);
            continue;
        } else if (pkmChoice == 0) {
            return -1;
        } else if (pt->party[pkmChoice - 1]->health == 0) {
            getLine(DEF_LINE_NUM); puts(RED "해당 포켓몬은 HP가 0입니다 .." RESET);
            continue;
        } else {
            return pkmChoice - 1; // 인덱스 -1반환시 배틀 종료
        }
    }
}

// Taking Pokemon
int takePkm(Pokemon* wildPkm) {
    double healthPercent = ((double)wildPkm->health / wildPkm->maxHealth) * 100.0;
    int takeChange = 100 - (int)(healthPercent);
    int takePercent = (rand() % 100) + 1;
    if (takePercent < takeChange) return 1;
    return 0;
}

// Run Away
int runAway(Pokemon* wildPkm) {
    double healthPercent = ((double)wildPkm->health / wildPkm->maxHealth) * 100.0;
    int runPercent = (rand() % 100) + 1;
    if (healthPercent == 100.0 && runPercent <= 10) return 1;
    if ((healthPercent >= 60.0 && healthPercent < 100.0) && runPercent <= 40) return 1;
    if ((healthPercent >= 40.0 && healthPercent < 60.0) && runPercent <= 60) return 1;
    if (healthPercent < 40.0 && runPercent <= 90) return 1;
    return 0;
}

// Pokemon Type Constant Match
char* pkmTypeString(int type) {
    if (type == 0) return "불";
    else if (type == 1) return "풀";
    else if (type == 2) return "물";
    else return "?";
}

// [Adventure] Select Attack(1)
void selectAdvAttack(Pokemon* battlePkm, Pokemon* wildPkm) {
    int turnChance = (rand() % 2 == 1); // 50% 확률
    if (turnChance) {
        getLine(DEF_LINE_NUM);
        printf("%s의 공격!\n", battlePkm->nickname);
        addDamage(battlePkm, wildPkm);
        if (wildPkm->health > 0) {
            getLine(DEF_LINE_NUM);
            printf("%s의 공격!\n", wildPkm->nickname);
            addDamage(wildPkm, battlePkm);
        }
    } else {
        getLine(DEF_LINE_NUM);
        printf("%s의 공격!\n", wildPkm->nickname);
        addDamage(wildPkm, battlePkm);
        if (battlePkm->health > 0) {
            getLine(DEF_LINE_NUM);
            printf("%s의 공격!\n", battlePkm->nickname);
            addDamage(battlePkm, wildPkm);
        }
    }
}

// [Adventure] Select Bag(2)
int selectAdvBag(Player* pt, Pokemon* battlePkm, Pokemon* wildPkm) {
    int itemSelect = 0;
    int addHp = 0;
    while (1) {
        if (battlePkm->health <= 0) return 1;
        // === 야생/플레이어 포켓몬 정보 화면 ===
        getLine(DEF_LINE_NUM);
        printf("\t\t\t%s\n\t\t\t(HP %d/%d)\n",
            wildPkm->nickname, wildPkm->health, wildPkm->maxHealth);
        printf("%s\n(HP %d/%d)\n",
            battlePkm->nickname,
            battlePkm->health,
            battlePkm->maxHealth);
        // === 아이템 정보 화면 ===
        getLine(DEF_LINE_NUM);
        printf("1. 몬스터볼 x %d\n2. 회복약 x %d\n3. 뒤로 가기\n", pt->pkmBalls, pt->healItems);
        printGrad(">>");
        if (onlyNumber(&itemSelect)) continue;
        // === 몬스터볼 선택 ===
        if (itemSelect == 1) {
            if (pt->pkmBalls == 0) { getLine(DEF_LINE_NUM); puts(RED "몬스터볼이 없습니다." RESET); continue; }
            pt->pkmBalls -= 1;
            // === 포켓몬 포획 시도 화면 ===
            getLine(DEF_LINE_NUM);
            printf("몬스터볼을 %s에게 던졌다!\n", wildPkm->nickname);
            for (int i = 1; i <= 3; i++) {
                printf(GRAY "\r%s" RESET, (i == 1) ? "." : ((i == 2) ? ". ." : ". . ."));
                fflush(stdout);
                sleep(1);
            }
            printf("\n");
            // === 포켓몬 포획 결과 화면 ===
            if (takePkm(wildPkm)) {
                printf("%s를(을) 잡았다!\n", wildPkm->nickname);
                pt->party[pt->pkmCount++] = catchPkmName(wildPkm);
                break;
            } else {
                printf("%s(이)가 몬스터볼에서 빠져나왔다.\n", wildPkm->nickname);
                getLine(DEF_LINE_NUM);
                printf("%s의 공격!\n", wildPkm->nickname);
                addDamage(wildPkm, battlePkm);
                continue;
            }
        // === 회복약 선택 ===
        } else if (itemSelect == 2) {
            if (pt->healItems == 0) { getLine(DEF_LINE_NUM); puts(RED "회복약이 없습니다." RESET); continue; }
            if (battlePkm->health == battlePkm->maxHealth) { getLine(DEF_LINE_NUM); puts(RED "회복할 필요가 없습니다" RESET); continue; }
            pt->healItems -= 1;
            addHp = battlePkm->maxHealth * 0.3;
            if ((battlePkm->health + addHp) > battlePkm->maxHealth) {
                addHp = battlePkm->maxHealth - battlePkm->health;
                battlePkm->health = battlePkm->maxHealth;
            } else battlePkm->health += addHp;
            getLine(DEF_LINE_NUM);
            printf(GREEN "%s는 %d의 체력을 회복하였다.\n" RESET, battlePkm->nickname, addHp);
            getLine(DEF_LINE_NUM);
            printf("%s의 공격!\n", wildPkm->nickname);
            addDamage(wildPkm, battlePkm);
            return 1;
        } else if (itemSelect == 3) {
            return 1; // 나가기
        } else { getLine(DEF_LINE_NUM); puts(RED "1 ~ 3 중 하나를 선택해주세요." RESET); }
    }
    return 0;
}

// Play Adventure
void playAdventure(Player* pt, PokemonData* pkmData, int pkmCount) {
    // === 변수 ===
    int randSec = (rand() % 5) + 1; // 1~5초 기다리기
    int randPkmIndex = rand() % pkmCount; // 랜덤 야생 포켓몬 인덱스
    int select = 0; // 공격, 가방열기, 도망치기
    int addMoney = 0;
    int battlePkmIndex = -1; // 플레이어 선두 포켓몬 인덱스 (첫 인덱스는 없음 = -1)
    int battle = 1; // 1이면 배틀 중, 0이면 배틀 끝
    // === 야생 포켓몬 메모리 할당 ===
    Pokemon* wildPkm = addPokemon(&pkmData[randPkmIndex], pkmData[randPkmIndex].name);
    for(int i = 0; i < pt->pkmCount; i++) {
        if (pt->party[i] != NULL && pt->party[i]->health > 0) { battlePkmIndex = i; break; }
    }
    if (battlePkmIndex == -1) {
        getLine(DEF_LINE_NUM); puts(RED "현재 전투 가능한 포켓몬이 없습니다." RESET);
        return;
    }
    // === 포켓몬 탐색 화면 ===
    getLine(DEF_LINE_NUM);
    for (int i = 1; i <= randSec; i++) {
        printf(GREEN "\r포켓몬을 탐색하는중 . . . (%d/%d)" RESET, i, randSec);
        fflush(stdout);
        sleep(1);
    }
    printf("\n");
    // === 포켓몬 탐색 완료 화면 ===
    getLine(DEF_LINE_NUM);
    printf("앗! 야생의 %s(이)가 나타났다!\n", wildPkm->nickname);
    Pokemon* battlePkm = pt->party[battlePkmIndex];
    // === 배틀 시작 ===
    while (battle) {
        // === 야생/플레이어 포켓몬 정보 화면 ===
        getLine(DEF_LINE_NUM);
        printf("\t\t\t%s\n\t\t\t(HP %d/%d)\n",
            wildPkm->nickname, wildPkm->health, wildPkm->maxHealth);
        printf("%s\n(HP %d/%d)\n",
            battlePkm->nickname, battlePkm->health, battlePkm->maxHealth);
        // === 배틀 선택 화면 ===
        getLine(DEF_LINE_NUM);
        printGrad("무엇을 해야할까?\n");
        printGrad("1. 공격 2. 가방열기 3. 도망치기\n>>");
        // === 숫자 입력할 때까지 반복 ===
        if (onlyNumber(&select)) continue;
        // === 공격 선택 ===
        if (select == 1) {
            selectAdvAttack(battlePkm, wildPkm);
            battlePkm = changePokemon(pt, battlePkm);
            if (battlePkm == NULL) battle = 0;
            if (wildPkm->health == 0) {
                addMoney = 300 + (rand() % 201);
                pt->money += addMoney;
                printf("%s(을)를 쓰러트려 %d원을 얻었습니다.\n", wildPkm->nickname, addMoney);
            }
            if (wildPkm->health == 0) battle = 0;
        // === 가방열기 선택 ===
        } else if (select == 2) {
            if (selectAdvBag(pt, battlePkm, wildPkm) != 1) battle = 0;
        } else if (select == 3) {
            if (runAway(wildPkm)) {
                getLine(DEF_LINE_NUM); puts(GREEN "무사히 도망쳤습니다." RESET);
                battle = 0;
            } else {
                getLine(DEF_LINE_NUM); puts(RED "도망칠 수 없습니다." RESET);
                continue;
            }
        } else { getLine(DEF_LINE_NUM); puts(RED "1 ~ 3 중 하나를 선택해주세요." RESET); }
    }
}

void goToShop(Player* pt) {
    int itemChoice, itemCount;
    while (1) {
        getLine(DEF_LINE_NUM);
        printf("상점\t\t지갑 : %d\n", pt->money);
        printGrad("1. 포켓몬볼 1000원\n");
        printGrad("2. 상처약 500원\n");
        getLine(DEF_LINE_NUM);
        printGrad("무엇을 구매할까? (나가기 0)\n>>");
        if (onlyNumber(&itemChoice)) continue;
        if (itemChoice == 0) break;
        if (itemChoice == 1) {
            getLine(DEF_LINE_NUM); printGrad("포켓몬볼을 몇개 구매할까? (취소 -1)\n>>");
            if (onlyNumber(&itemCount)) continue;
            if (itemCount <= 0) continue;
            if (pt->money < 1000 * itemCount) { getLine(DEF_LINE_NUM); puts(RED "돈이 부족합니다!" RESET); continue; }
            pt->money -= 1000 * itemCount;
            pt->pkmBalls += itemCount;
            getLine(DEF_LINE_NUM); printf("성공적으로 구매하였다! (잔액 %d원)\n", pt->money);
            break;
        } else if (itemChoice == 2) {
            getLine(DEF_LINE_NUM); printGrad("상처약을 몇개 구매할까? (취소 -1)\n>>");
            if (onlyNumber(&itemCount)) continue;
            if (itemCount <= 0) continue;
            if (pt->money < 500 * itemCount) { getLine(DEF_LINE_NUM); puts(RED "돈이 부족합니다!" RESET); continue; }
            pt->money -= 500 * itemCount;
            pt->healItems += itemCount;
            getLine(DEF_LINE_NUM); printf("성공적으로 구매하였다! (잔액 %d원)\n", pt->money);
            break;
        } else { getLine(DEF_LINE_NUM); puts(RED "1 ~ 2 중 하나를 선택해주세요." RESET); }
    }
}

void goToPkmCenter(Player* pt) {
    getLine(DEF_LINE_NUM);
    for (int i = 1; i <= 3; i++) {
        printf(GREEN "\r포켓몬을 치료하고 있습니다 %s" RESET, (i == 1) ? "." : ((i == 2) ? ". ." : ". . ."));
        fflush(stdout);
        sleep(1);
    }
    printGrad("\n치료가 완료되었습니다!\n");
    getLine(DEF_LINE_NUM);
    for(int i = 0; i < pt->pkmCount; i++) {
        pt->party[i]->health = pt->party[i]->maxHealth;
        printf("%s %s (HP %d/%d)\n", pt->party[i]->nickname, pkmTypeString(pt->party[i]->pkmData->pkmType),
            pt->party[i]->health, pt->party[i]->maxHealth);
    }
    printGrad("\n(press enter)"); getchar(); getchar();
}

void openPokedex(PokemonData* pkmData, int* pkmCount) {
    getLine(DEF_LINE_NUM);
    printGrad("< 포켓몬 도감 >\n");
    printGrad("| No | 이름 | 속성 | 최소 HP | 최소 공격력 |\n");
    for(int i = 0; i < *pkmCount; i++) {
        printf("%d. %s %s %d %d\n", pkmData[i].id, pkmData[i].name,
            pkmTypeString(pkmData[i].pkmType), pkmData[i].minHealth, pkmData[i].minAttack);
    }
    printGrad("\n(press enter)"); getchar(); getchar();
}

int pkmMasterLicense(Player* pt) {
    if (pt->pkmCount >= 6) {
        getLine(DEF_LINE_NUM);
        printGrad("포켓몬 마스터가 되었다!\n\n");
        for(int i = 0; i < pt->pkmCount; i++) {
            printf(GREEN "%s (HP %d/%d)\n" RESET, pt->party[i]->nickname, pt->party[i]->health,
            pt->party[i]->maxHealth);
        }
        printf(YELLOW "\n포켓몬볼 x %d\n" RESET, pt->pkmBalls);
        printf(YELLOW "상처약 x %d\n" RESET, pt->healItems);
        getLine(DEF_LINE_NUM);
        return 1;
    }
    return 0;
}

void freeMemory(Player* pt) {
    for(int i = 0; i < pt->pkmCount; i++) {
        free(pt->party[i]);
    }
    free(pt->party);
    free(pt);
}

// Pokemon Game Main Theme
void mainTheme() {
    pkmThemeAsciiArt();
    printGrad("==========( 17기 최동현 )===========\n");
    printGrad("\n\tK.Knock Pokemon Game\n");
    printGrad("\tpress enter to start\n\n");
    getLine(DEF_LINE_NUM);
    getchar(); // press enter
}

// Pokemon Theme ASCII Art
void pkmThemeAsciiArt() {
    puts(BLUE "                                                      %*                                            ");
    puts(YELLOW "                                                    #****                                           ");
    puts(BLUE "                                                  #**--==**                                         ");
    puts(YELLOW "                                     ** #*       ***--***           ******                          ");
    puts(BLUE "        *************          ********#****   %%******     #********+===*       %*                 ");
    puts(YELLOW "    ****==--------==***     ##**-----***--==***#****====***##**---=**---=**     #%*+*******         ");
    puts(BLUE " %**=----------------=**     #****---=*-----==**---------==***-----=*---=**     ##***---=**#********");
    puts(YELLOW " %%**-----------**----=**    ##%**---=------***---*++**--*****-----=-----**  *******-----=***----+* ");
    puts(BLUE "  ##**--------=**+**--=***********--------** *---****--*******-----------=***=--===**----=**----=** ");
    puts(YELLOW "   ##****------=* **--***=--*--==**-----*******--=*--**=--=***----------=**-=*-----=**----=*----**  ");
    puts(BLUE "    ####**-----=**---***--**---*-=**-------==**+-----------***---*-----=**--******--=*----=+---=*   ");
    puts(YELLOW "       ##**--------****--=******--**--**-----==***-----******---=**--*-=*----====---**-=------=**   ");
    puts(BLUE "        %%**-----=**#**----==----=**--******----==+****#%%%*----=******=**---------**--*------**    ");
    puts(YELLOW "         %%**----=**#**---------=**---**%###****---=*    %%*******%****-=***-----**---=*-----=**    ");
    puts(BLUE "         %%%**----=*%%***-----*****---**    %%%%*****             %%%**---==****%**---=*-----**     ");
    puts(YELLOW "          %%**=---=** %%%*****% #*******        %%%%*               %%%%******  %*******----**      ");
    puts(BLUE "           %%**----=*                                                                %%*---=**      ");
    puts(YELLOW "            %%*******                                                                %%%*****       ");
    puts(BLUE "             %%%                                                                          %         " RESET);
    printf("\n");
}