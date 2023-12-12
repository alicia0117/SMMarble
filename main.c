//
//  main.c
//  SMMarble
//
//  Created by professor on 2023/11/05.
//


#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"


//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

const char* smmNodeName[MAX_NODETYPE] = {
    "Lecture", "Restaurant", "Laboratory", "Home", "Experiment", "FoodChance", "Festival"
};

//function prototypes
#if 0



float calcAverageGrade(int player); //calculate average grade of the player
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history

#endif

typedef struct Player{
    char playerName[MAX_CHARNAME];
    int credit;
    int energy;
    int nodeNum;
} Player;

Player PlayerList[MAX_PLAYER];

void generatePlayers(int n, int initEnergy); //generate a new player
int isGraduated(int n); //check if any player is graduated
void printPlayerStatus(int n); //print all player status at the beginning of each turn
int rolldie(int player);
void printGrades(int player); //print all the grade history of the player
void goForward(int player, int step); //make player go "step" steps on the board (check if player is graduated)

//action code when a player stays at a node
// void actionNode(int player)
// {
//     switch(type)
//     {
//         //case lecture:
//         default:
//             break;
//     }
// }



int main(int argc, const char * argv[]) {
    int turn=0;
    int init_energy;
    int players;
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    
    
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    printf("Reading board component......\n");
    while (0 < fscanf(fp, "%s %d %d %d", name, &type, &credit, &energy)) //read a node parameter set
    {
        //store the parameter set
        smmNode_e* NodePtr = smmObj_genNode(name, type, credit, energy, 0);
        if(type == 3) init_energy = energy;
        smmdb_addTail(LISTNO_NODE, NodePtr);
        printf(" => %d. %s (%s), credit:%d, energy:%d\n", board_nr, name, smmNodeName[type], credit, energy);
        board_nr++;
        
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while (0 < fscanf(fp, "%s %d", name, &energy)) //read a food parameter set
    {
        //store the parameter set
        smmFood_e* NodePtr = smmObj_genNode(name, 0, 0, energy, 1);
        smmdb_addTail(LISTNO_FOODCARD, NodePtr);
        printf(" => %d. %s, charge:%d\n", board_nr, name, energy);
        food_nr++;
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", food_nr);
        
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while (0 < fscanf(fp, "%s", name)) //read a festival card string
    {
        //store the parameter set
        smmFest_e* NodePtr = smmObj_genNode(name, 0, 0, 0, 2);
        smmdb_addTail(LISTNO_FESTCARD, NodePtr);
        printf(" => %d. %s.\n", festival_nr, name);
        festival_nr++;
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
    
    printf("\n\n\n=======================================================================\n");
    printf("-----------------------------------------------------------------------\n");
    printf("Sookmyung Marble !! Let's Graduate (total credit : 30)!!\n");
    printf("-----------------------------------------------------------------------\n");
    printf("=======================================================================\n\n\n\n");
    
    //2. Player configuration ---------------------------------------------------------------------------------
    
    printf("Input No. of players (1~%d) : ", MAX_PLAYER);
    scanf("%d", &players);
    for(int i=0; i<players; i++){
        printf("Player%d Name : ", i);
        scanf("%s", PlayerList[i].playerName);
    }
    generatePlayers(players, init_energy);
    
    // 3. SM Marble game starts ---------------------------------------------------------------------------------
    while (!isGraduated(players)) //is anybody graduated?
    {
        turn = turn % players;
        int die_result;
        
        //4-1. initial printing
        printPlayerStatus(turn);
        
        //4-2. die rolling (if not in experiment)
        
        die_result = rolldie(turn);
        printf("\nOh, Your dice number is %d!!\n\n", die_result);
        //4-3. go forward
        //goForward();

		//4-4. take action at the destination node of the board
        //actionNode();
        
        //4-5. next turn
        turn++;
        if(turn==1) break;
    }
    
    return 0;
}

void generatePlayers(int n, int initEnergy){
    for(int i=0; i<n; i++){
        PlayerList[i].energy = initEnergy;
        PlayerList[i].credit = 0;
        PlayerList[i].nodeNum = 0;
    }
}

int isGraduated(int n){
    for(int i=0; i<n; i++){
        if(PlayerList[i].credit >= GRADUATE_CREDIT){
            return 1;
        }
    }
    return 0;
}

void printPlayerStatus(int n){
    smmNode_e *nodeInfo = smmdb_getData(LISTNO_NODE, PlayerList[n].nodeNum);
    printf("\n\nPlayer%d Name : %s\n", n, PlayerList[n].playerName);
    printf("Player%d credit : %d\n", n, PlayerList[n].credit);
    printf("Player%d energy : %d\n", n, PlayerList[n].energy);
    printf("Player%d is here : %d(%s)\n", n, PlayerList[n].nodeNum, nodeInfo->name);
}

int rolldie(int player)
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    scanf(" %c", &c);
    
    if (c == 'g')
        printGrades(player);
    
    return (rand()%MAX_DIE + 1);
}

void printGrades(int player){
    int gradeLen = smmdb_len(LISTNO_OFFSET_GRADE);
    for(int i=0; i<gradeLen; i++){
        smmGrade_e* checkGrade = smmdb_getData(LISTNO_OFFSET_GRADE, i);
        if(checkGrade->playerId == player){
            printf("%c ", checkGrade->grade);
        }
    }
    printf("\n");
}
