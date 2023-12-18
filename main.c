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
int init_energy;
int whereLab;

const char* smmNodeName[MAX_NODETYPE] = {
    "Lecture", "Restaurant", "Laboratory", "Home", "Experiment", "FoodChance", "Festival"
};

const char* smmGradeName[MAX_GRADE] = {
    "A+", "A0", "A-", "B+", "B0", "B-", "C+", "C0", "C-"
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
    int isExperience;
} Player;

Player PlayerList[MAX_PLAYER];

int endflag = 0;
void generatePlayers(int n); //generate a new player
//int isGraduated(int n); //check if any player is graduated
void printPlayerStatus(int n); //print all player status at the beginning of each turn
int rolldie(int player);
void printGrades(int player); //print all the grade history of the player
void goForward(int player, int step); //make player go "step" steps on the board (check if player is graduated)
int isGraded(int player, char* lectureName); // return is player Graded lecture?


//action code when a player stays at a node
void actionNode(int player)
{
    smmNode_e *nodeInfo = smmdb_getData(LISTNO_NODE, PlayerList[player].nodeNum);
    switch(nodeInfo->type)
    {
        case 0:
            if(PlayerList[player].energy < nodeInfo->energy) printf("need more Energy T.T\n\n");
            else{
                if(!isGraded(player, nodeInfo->name)){
                    char c;
                    printf("Here Name(Lecture) is %s\n", nodeInfo->name);
                    printf("It takes as much energy as %d here. Do you want to take the course? : (Y/N)", nodeInfo->energy);
                    scanf(" %c", &c);
                    if(c == 'Y'){
                        int randGrade = rand()%MAX_GRADE;
                        smmGrade_e* gradeInfo= smmObj_genNode(nodeInfo->name, player, nodeInfo->credit, randGrade, 3);
                        printf("You got %s!!\n\n", smmGradeName[randGrade]);
                        PlayerList[player].energy -= nodeInfo->energy;
                        PlayerList[player].credit += nodeInfo->credit;
                        smmdb_addTail(LISTNO_OFFSET_GRADE, gradeInfo);
                    }
                }
            }
            break;
        case 1:
        {
            smmNode_e* nodeInfo = smmdb_getData(0, PlayerList[player].nodeNum);
            printf("Congrats! You are in Restaurant, You will get %d energy\n", nodeInfo->energy);
            PlayerList[player].energy += nodeInfo->energy;
            break;
        }
        case 2:
            printf("Now You are in Lab\n");
            if(PlayerList[player].isExperience){
                /* 
                이게 실행될 가능성이 있는 지에 관한 의문. 
                어차피 실험 블럭으로 가버리면, 강제로 실험실로 이동되는데 그 부분은 밑에 있는 case 4에서 실행될 것임.
                */
                printf("Good Luck~!\n");
            }
            else{ // 이 부분도 마찬가지로 else가 아니라, 사실상 무조건 아닌가
                printf("But your experience stat is False. So don't worry~!\n");
            }
            break;
        case 3:
            // 딱 집에 도착하는 경우 (goForward에서 energy를 얻는 경우는, 집을 지나쳐가는 경우임.)
            printf("Now, here is house. You will get %d energy\n", init_energy);
            PlayerList[player].energy += init_energy;
            break;
        case 4:
            printf("Oh no... we have to go lab...\n");
            printf("Now your experience stat will be True. GoodLuck!\n");  
            PlayerList[player].isExperience = 1;
            PlayerList[player].nodeNum = whereLab;
            break;
        case 5:
        {// 변수 선언하기 위해 중괄호로 묶기
            int randomFood = rand() % food_nr;
            smmFood_e* randfood = smmdb_getData(LISTNO_FOODCARD, randomFood);
            printf("Oh, You got a FoodChance!\n");
            printf("There will be a random Food, and you will get or lose Energy\n");
            printf("You got food %s, and you will get energy : %d\n", randfood->name, randfood->energy);
            PlayerList[player].energy += randfood->energy;
            break;
        }
        case 6:
        {// 변수 선언하기 위해 중괄호로 묶기
            int randomFest = rand()%festival_nr;
            smmFest_e* randfest = smmdb_getData(LISTNO_FESTCARD, randomFest);
            printf("Oh, You got a FestivalChance!\n");
            printf("You have to join a festival.\n");
            printf("Your Festival Name is, %s Must do it!\n", randfest->name);
        }
        default:
            break;
        printf("\n\n\n");
    }
}



int main(int argc, const char * argv[]) {
    int i=0;
    int turn=0;
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
        void* NodePtr = smmObj_genNode(name, type, credit, energy, 0);
        if(type == 3) init_energy = energy;
        if(type == 2) whereLab = board_nr;
        smmdb_addTail(0, NodePtr);
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
        void* NodePtr = smmObj_genNode(name, 0, 0, energy, 1);
        smmdb_addTail(1, NodePtr);
        printf(" => %d. %s, charge:%d\n", food_nr, name, energy);
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
        void* NodePtr = smmObj_genNode(name, 0, 0, 0, 2);
        smmdb_addTail(2, NodePtr);
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
    for(i=0; i<players; i++){
        printf("Player%d Name : ", i);
        scanf("%s", PlayerList[i].playerName);
    }
    generatePlayers(players);
    
    // 3. SM Marble game starts ---------------------------------------------------------------------------------
    while (1) //is anybody graduated?
    {
        turn = turn % players;
        int die_result;
        
        //4-1. initial printing
        printPlayerStatus(players);
        
        //4-2. die rolling (if not in experiment)
        
        die_result = rolldie(turn);
        printf("\nOh, Your dice number is %d!!\n\n", die_result);
        //4-3. go forward
        goForward(turn, die_result);
        if(endflag){
            printf("Congrats!! The player%d Win!!!!\n\n",turn);
            printGrades(turn);
            printf("\n\nPress Any Key To Exit\n");
            getch();
            break;
        }
		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        //4-5. next turn
        turn++;
        // if(turn==1) break;
    }
    return 0;
}

void generatePlayers(int n){
    int i=0;
    for(i=0; i<n; i++){
        PlayerList[i].energy = init_energy;
        PlayerList[i].credit = 0;
        PlayerList[i].nodeNum = 0;
        PlayerList[i].isExperience = 0;
    }
}

// int isGraduated(int n){
//     for(int i=0; i<n; i++){
//         if(PlayerList[i].credit >= GRADUATE_CREDIT){
//             return 1;
//         }
//     }
//     return 0;
// }

void printPlayerStatus(int n){
    int i=0;
    for(i=0; i<n; i++){
        smmNode_e *nodeInfo = smmdb_getData(LISTNO_NODE, PlayerList[i].nodeNum);
        printf("\n\nPlayer%d Name : %s\n", i, PlayerList[i].playerName);
        printf("Player%d is here : %d(%s)\n", i, PlayerList[i].nodeNum, nodeInfo->name);
        printf("Player%d Experiement Status : %d\n", i, PlayerList[i].isExperience);
        printf("Player%d credit : %d\n", i, PlayerList[i].credit);
        printf("Player%d energy : %d\n", i, PlayerList[i].energy);
    }
    printf("\n\n\n");
}

int rolldie(int player)
{
    char c;
    printf("Hello, Player%d, Press any key to roll a die (press g to see grade): ", player);
    scanf(" %c", &c);
    
    if (c == 'g')
        printGrades(player);
    
    return (rand()%MAX_DIE + 1);
}

void printGrades(int player){
    int i=0;
    int gradeLen = smmdb_len(LISTNO_OFFSET_GRADE);
    for(i=0; i<gradeLen; i++){
        smmGrade_e* checkGrade = smmdb_getData(LISTNO_OFFSET_GRADE, i);
        if(checkGrade->playerId == player){
            printf("%s %s %d\n", checkGrade->courseName, smmGradeName[checkGrade->grade], checkGrade ->credit);
        }
    }
    printf("\n");
}

void goForward(int player, int step){
    int nextNode = PlayerList[player].nodeNum + step;
    // printf("[TEST] NEXT NODE : %d\n", nextNode);
    if(PlayerList[player].isExperience){
        int randEscapeValue = rand()%MAX_DIE + 1;
        printf("Experience Escape Value : %d\n", randEscapeValue);
        if(step != randEscapeValue){
            printf("try one more haha!\n");
            return ;
        }
        PlayerList[player].isExperience = 0;
    }
    if(nextNode >= board_nr){
        int i;
        nextNode %= board_nr;
        if(nextNode > board_nr){
            printf("You go through house! You will get %d energy\n\n", init_energy);
            PlayerList[player].energy += init_energy; // go through house
        }
        for(i=PlayerList[player].nodeNum+1; i<board_nr; i++){
            smmNode_e* nodeInfo = smmdb_getData(0, i);
            printf("%s\n", nodeInfo->name);
        }
        for(i=0; i <= nextNode; i++){
            smmNode_e* nodeInfo = smmdb_getData(0, i);
            printf("%s\n", nodeInfo->name);            
        }
        if(PlayerList[player].credit >= GRADUATE_CREDIT){
            endflag = 1;
        }
    }
    else{
        int i;
        for(i=PlayerList[player].nodeNum+1; i <= nextNode; i++){
            smmNode_e* nodeInfo = smmdb_getData(0, i);
            printf("%s\n", nodeInfo->name);   
        }
    }
    PlayerList[player].nodeNum = nextNode;
}

int isGraded(int player, char* lectureName){
    int i;
    int gradeLen = smmdb_len(LISTNO_OFFSET_GRADE);
    for(i=0; i<gradeLen; i++){
        smmGrade_e *checkGrade = smmdb_getData(LISTNO_OFFSET_GRADE, i);
        if(checkGrade->playerId == player){
            if(!strcmp(lectureName, checkGrade->courseName)){
                printf("Already Graded!!\n\n");
                return 1;
            }
        }
    }
    return 0;
}
