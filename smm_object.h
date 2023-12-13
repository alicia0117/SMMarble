//
//  smm_object.h
//  SMMarble object
//
//  Created by professor on 2023/11/05.
//

#ifndef smm_object_h
#define smm_object_h

#define MAX_NODETYPE    7
#define MAX_GRADE       9

/* node type :
    lecture,
    restaurant,
    laboratory,
    home,
    experiment,
    foodChance,
    festival
*/

typedef struct smmNode_e{
    char* name;
    int type;
    int credit;
    int energy;
} smmNode_e;

typedef struct smmFood_e{
    char* name;
    int energy;
} smmFood_e;

typedef struct smmFest_e{
    char* name;
} smmFest_e;

typedef struct smmGrade_e{
    char* courseName;
    int playerId;
    int credit;
    int grade;
} smmGrade_e;

/* grade :
    A+,
    A0,
    A-,
    B+,
    B0,
    B-,
    C+,
    C0,
    C-
*/



//object generation
void* smmObj_genNode(char* name, int type, int credit, int energy, int ListType);
// smmObj_genNode in grade, name : courseName, type : playerId, credit : credit, energy:grade
//member retrieving


//element to string



#endif /* smm_object_h */
