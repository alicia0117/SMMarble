//
//  smm_node.c
//  SMMarble
//
//  Created by professor on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODETYPE    7
#define MAX_GRADE       9

// //object generation
void* smmObj_genNode(char* name, int type, int credit, int energy, int ListType){
    if(ListType==0){ // LISTNO_NODE
        smmNode_e* NodePtr = (smmNode_e*)malloc(sizeof(smmNode_e));
        strcpy(NodePtr->name, name);
        NodePtr->type = type;
        NodePtr->credit = credit;
        NodePtr->energy = energy;
        return NodePtr;
    }
    else if(ListType==1){ // LISTNO_FOODCARD
        smmFood_e* NodePtr = (smmFood_e*)malloc(sizeof(smmFood_e));
        strcpy(NodePtr->name, name);
        NodePtr->energy = energy;
        return NodePtr;
    }
    else if(ListType==2){
        smmFest_e* NodePtr = (smmFest_e*)malloc(sizeof(smmFest_e));
        strcpy(NodePtr->name, name);
        return NodePtr;
    }
    // smmObj_genNode in grade, name : courseName, type : playerId, credit : credit, energy:grade
    else if(ListType==3){
        int playerId = type;
        int grade = energy;
        smmGrade_e* NodePtr = (smmGrade_e*)malloc(sizeof(smmGrade_e));
        strcpy(NodePtr->courseName, name);
        NodePtr->playerId = playerId;
        NodePtr->credit = credit;
        NodePtr->grade = grade;
        return NodePtr;
    }
    else return NULL;
}



//member retrieving



//element to string
// char* smmObj_getNodeName(smmNode_e type)
// {
//     return smmNodeName[type];
// }

// char* smmObj_getGradeName(smmGrade_e grade)
// {
//     return smmGradeName[grade];
// }

