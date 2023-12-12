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
        NodePtr->name = name;
        NodePtr->type = type;
        NodePtr->credit = credit;
        NodePtr->energy = energy;
        return NodePtr;
    }
    else if(ListType==1){ // LISTNO_FOODCARD
        smmFood_e* NodePtr = (smmFood_e*)malloc(sizeof(smmFood_e));
        NodePtr->name = name;
        NodePtr->energy = energy;
        return NodePtr;
    }
    else if(ListType==2){
        smmFest_e* NodePtr = (smmFest_e*)malloc(sizeof(smmFest_e));
        NodePtr->name = name;
        return NodePtr;
    }
    else return 0;
}




const char* smmGradeName[MAX_GRADE] = {
    "A+", "A0", "A-", "B+", "B0", "B-", "C+", "C0", "C-"
};


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
