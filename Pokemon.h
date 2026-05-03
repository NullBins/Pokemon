#pragma once
#include "PokeTool.h"

// Max Pokemon Number
#define MAX_PARTY 6

// Pokemon Types
enum type { FIRE = 0, GRASS = 1, WATER = 2 };

// Pokemon Data Struct
typedef struct {
    int id;
    char name[30];
    enum type pkmType;
    int minHealth;
    int minAttack;
} PokemonData;

// Pokemon Object Struct
typedef struct {
    PokemonData* pkmData;
    char nickname[40];
    int health;
    int maxHealth;
    int attack;
} Pokemon;

// Pokemon Trainer Struct
typedef struct {
    Pokemon** party;
    int pkmCount;
    int money;
    int pkmBalls;
    int healItems;
} Player;

// Pokemon Functions
PokemonData* loadPkmData(int* count);
Player* loadPlayerData(Player* pt, PokemonData* pkmData);
void saveData(Player* pt);
Player* initPlayer();
Pokemon* addPokemon(PokemonData* pkmData, const char* nickname);
Pokemon* catchPkmName(Pokemon* catchPkm);
Pokemon* changePokemon(Player* pt, Pokemon* battlePkm);
Player* selectPokemon(Player* pt, PokemonData* pkmData);
void addDamage(Pokemon* attackPkm, Pokemon* defensePkm);
int alivePkmParty(Player* pt);
int takePkm(Pokemon* wildPkm);
int runAway(Pokemon* wildPkm);
char* pkmTypeString(int type);
void selectAdvAttack(Pokemon* battlePkm, Pokemon* wildPkm);
int selectAdvBag(Player* pt, Pokemon* battlePkm, Pokemon* wildPkm);
void playAdventure(Player* pt, PokemonData* pkmData, int pkmCount);
void goToShop(Player* pt);
void goToPkmCenter(Player* pt);
void openPokedex(PokemonData* pkmData, int* pkmCount);
int pkmMasterLicense(Player* pt);
void freeMemory(Player* pt);
void mainTheme();
void pkmThemeAsciiArt();