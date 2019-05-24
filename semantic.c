#include "wordunit.h"
#include "synax.h"
#include "symbolTable.h"
#include "sem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define NODE(x) (node->next[(x)])
#define NODE_SEM(x) ((node->next[(x)])->sem)
#define FNODE_SEM (node->sem)
#define EXP(x) ((x) + EXPOFFSET)
#define RET(x) ((x) - EXPOFFSET)

extern void printStcInfo(Sentence* node);
extern void showAST(Sentence* node, int lv);
extern void lex_part(FILE* file);
extern void syn_part();

extern void sem(Sentence* node, int rulenum, int stepnum);

extern Sentence* pgm;

symtable table[ROWMAX];
int tptr;
int tptrStk[10000];
int tptrStkTop;
int onameCnt;

char *sem_type[] = {"int", "float", "char", "func", "int_array", "float_array", "char_array"};

int level = 0;
int offset = DX;
void showTable();

int getSize(int type){
    if(type == SEM_INT || type == SEM_INT_ARRAY) return INT_SIZE;
    if(type == SEM_FLOAT || type == SEM_FLOAT_ARRAY) return FLOAT_SIZE;
    if(type == SEM_CHAR || type == SEM_CHAR_ARRAY) return CHAR_SIZE;
}

void insertTable(Sentence* id, int opt){
    table[++tptr].name = id->tval.name;
    table[tptr].type = id->sem.type;
    table[tptr].level = level;
    if(table[tptr].type != SEM_FUNC){
        table[tptr].offset = offset;
        offset += (getSize(table[tptr].type) * id->sem.arraynum);
    }else{
        table[tptr].offset = offset;
        table[tptr].types = id->sem.argtype;
        table[tptr].argnum = &(id->sem.argnum);
    }
    if(opt)
        sprintf(table[tptr].oname, "$tmp%d", ++onameCnt);
    showTable();
}

int searchTable(Sentence* id){
    int cnt = 0;
    for(int i = tptr; i > 0; i--){
        if(strcmp(id->tval.name, table[i].name) == 0 && id->sem.type == table[i].type){
            if(id->sem.type != SEM_FUNC){
                if(level == table[i].level) return FOUND_IN_SAME_LEVEL;
                else return FOUND_IN_DIFF_LEVEL;
            }else{
                if(cnt == 1){
                    cnt++;
                    if(id->sem.argnum != *(table[i].argnum)) return FUNC_OVERLOAD;
                    for(int j = 1; j <= id->sem.argnum; j++)
                        if(id->sem.argtype[j] != table[i].types[j]) return FUNC_OVERLOAD;
                    return FUNC_CONFLICT;
                }
            }
        }
    }
    return NOT_FOUND;
}

int searchReference(Sentence* id, int isFunc){
    int funcmatch = 0;
    for(int i = tptr; i > 0; i--){
        if(strcmp(id->tval.name, table[i].name) == 0){
            if(isFunc && table[i].type == SEM_FUNC){
                funcmatch = 1;
                if(id->sem.argnum != *(table[i].argnum)) continue;
                int fd = 1;
                for(int j = 2; j <= id->sem.argnum; j++){
                    if(id->sem.argtype[j] != table[i].types[j]) {
                        fd = 0;
                        break;
                    }
                }
                if(fd)
                    return i;
            }
            if(!isFunc && table[i].type != SEM_FUNC)
                return i;
        }
    }
    return (funcmatch) ? -2 : -1;
}

void showTable(){
    printf("table:\n");
    printf("%7s %7s %7s %9s %7s %7s\n", "name", "type", "level", "offset", "funarg", "oname");
    for(int i = 1; i <= tptr; i++){
        if(table[i].type != SEM_FUNC){
            printf("%7s %7s %7d %7d        ", table[i].name, sem_type[table[i].type - SEM_INT], table[i].level, table[i].offset);
            if(table[i].oname[0] == '$')
                printf("%7s", table[i].oname);
        }else{
            printf("%7s %7s %7d           ", table[i].name, sem_type[table[i].type - SEM_INT], table[i].level);
            printf("%s(", sem_type[table[i].types[1] - SEM_INT]);
            for(int j = 2; j <= *(table[i].argnum); j++){
                printf("%s, ", sem_type[table[i].types[j] - SEM_INT]);
            }
            printf(")");
        }
        printf("\n");
    }
    printf("\n");
}

void newZone(int opt){
    level++;
    tptrStk[++tptrStkTop] = tptr;
    if(opt)
        offset = DX;
}

void quitZone(int opt){
    level--;
    tptr = tptrStk[tptrStkTop--];
    if(opt)
        offset = table[tptr].offset;
}

void dfs(Sentence* node){
    if(node->type != SYNAXELE) return;
    sem(node, node->irule, 0);
    for(int i = 1; ;i++){
        if(node->next[i] == NULL) {
            break;
        }
        dfs(node->next[i]);
        sem(node, node->irule, i);
    }
}

void sem_error(Sentence* stc, const char* reason){
    printf("semantic_err: can't deal with "); 
    printStcInfo(stc);
    printf("reason: ");
    printf("%s", reason);
    exit(0);
}

// typedef struct seminfo
// {
//     int type;
//     int retype;
//     int canLeft;
//     int argtype[MAXARG];
//     int argnum;
//     int arraynum;
// } seminfo;

void sem(Sentence* node, int rulenum, int stepnum){
    switch(rulenum){
        case 0:
            if(stepnum == 1){
                quitZone(1);
                showTable();
            }
            return;
        case 1:
            if(stepnum == 1){
                NODE_SEM(2).type = NODE_SEM(1).type;
                NODE_SEM(2).arraynum = NODE_SEM(1).arraynum;
            }
            return;
        case 3:
            if(stepnum == 1){
                FNODE_SEM.type = SEM_INT;
                FNODE_SEM.arraynum = 1;
            }
            return;
        case 4:
            if(stepnum == 1){
                FNODE_SEM.type = SEM_FLOAT;
                FNODE_SEM.arraynum = 1;
            }
            return;
        case 5:
            if(stepnum == 1){
                FNODE_SEM.type = SEM_CHAR;
                FNODE_SEM.arraynum = 1;
            }
            return;
        case 6:
            if(stepnum == 2){
                FNODE_SEM.type = SEM_INT_ARRAY;
                FNODE_SEM.arraynum = NODE(2)->tval.ivalue;
            }
            return;
        case 7:
            if(stepnum == 2){
                FNODE_SEM.type = SEM_FLOAT_ARRAY;
                FNODE_SEM.arraynum = NODE(2)->tval.ivalue;
            }
            return;
        case 8:
            if(stepnum == 2){
                FNODE_SEM.type = SEM_CHAR_ARRAY;
                FNODE_SEM.arraynum = NODE(2)->tval.ivalue;
            }
            return;
        case 9:
            if(stepnum == 0){
                NODE_SEM(1).type = FNODE_SEM.type;
                NODE_SEM(1).arraynum = FNODE_SEM.arraynum;
            }else if(stepnum == 1){
                if(searchTable(NODE(1)) != NOT_FOUND)
                    sem_error(NODE(1), "external redefine var");
                insertTable(NODE(1), 0);
                NODE_SEM(2).type = FNODE_SEM.type;
                NODE_SEM(2).arraynum = FNODE_SEM.arraynum;
            }
            return;
        case 10:
            if(stepnum == 0){
                NODE_SEM(1).type = FNODE_SEM.type;
                NODE_SEM(1).arraynum = FNODE_SEM.arraynum;
            }else if(stepnum == 1){
                if(searchTable(NODE(1)) != NOT_FOUND)
                    sem_error(NODE(1), "external redefine var");
                insertTable(NODE(1), 0);
            }
            return;
        case 11:
            if(stepnum == 0){
                NODE_SEM(1).type = FNODE_SEM.type;
                NODE_SEM(1).arraynum = FNODE_SEM.arraynum;
            }
            return;
        case 12:
            if(stepnum == 0)
                NODE_SEM(1).type = FNODE_SEM.type;
            else if(stepnum == 1)
                NODE_SEM(2).cannotnew = 1;
            else if(stepnum == 2){
                if(NODE_SEM(2).retype != FNODE_SEM.type){
                    sem_error(NODE(2), "function return type conflict");
                }
            }
            return;
        case 13:
            if(stepnum == 0){
                NODE_SEM(1).type = SEM_FUNC;
                NODE_SEM(1).argtype = (int *) malloc(MAXARG * sizeof(int));
                NODE_SEM(1).argtype[++(NODE_SEM(1).argnum)] = FNODE_SEM.type;
                NODE_SEM(2).argtype = NODE_SEM(1).argtype;
                NODE_SEM(2).argnum = NODE_SEM(1).argnum;
                insertTable(NODE(1), 0);
                newZone(1);
            }else if(stepnum == 2){
                NODE_SEM(1).argnum = NODE_SEM(2).argnum;
                if(searchTable(NODE(1)) == FUNC_CONFLICT){
                    sem_error(NODE(1), "function redefine");
                }
                showTable();
            }
            return;
        case 14:
            if(stepnum == 0){
                NODE_SEM(1).type = SEM_FUNC;
                NODE_SEM(1).argtype = (int *) malloc(MAXARG * sizeof(int));
                NODE_SEM(1).argtype[++(NODE_SEM(1).argnum)] = FNODE_SEM.type;
                insertTable(NODE(1), 0);
                newZone(1);
            }else if(stepnum == 1){
                if(searchTable(NODE(1)) == FUNC_CONFLICT){
                    sem_error(NODE(1), "function redefine");
                }
            }
            return;
        case 15:
            if(stepnum == 0){
                NODE_SEM(1).argtype = FNODE_SEM.argtype;
                NODE_SEM(1).argnum = FNODE_SEM.argnum;
            }else if(stepnum == 1){
                NODE_SEM(2).argnum = NODE_SEM(1).argnum;
                NODE_SEM(2).argtype = FNODE_SEM.argtype;
            }else if(stepnum == 2){
                FNODE_SEM.argnum = NODE_SEM(2).argnum;
            }
            return;
        case 16:
            if(stepnum == 0){
                NODE_SEM(1).argtype = FNODE_SEM.argtype;
                NODE_SEM(1).argnum = FNODE_SEM.argnum;
            }else if(stepnum == 1){
                FNODE_SEM.argnum = NODE_SEM(1).argnum;
            }
            return;
        case 17:
            if(stepnum == 1){
                NODE_SEM(2).type = NODE_SEM(1).type;
                NODE_SEM(2).arraynum = NODE_SEM(1).arraynum;
            }else if(stepnum == 2){
                if(searchTable(NODE(2)) == FOUND_IN_SAME_LEVEL)
                    sem_error(NODE(2), "function args redefine");
                insertTable(NODE(2), 0);
                FNODE_SEM.argtype[++FNODE_SEM.argnum] = NODE_SEM(2).type;
            }
            return;
        case 18:
            if(stepnum == 0){
                if(FNODE_SEM.cannotnew == 0)
                    newZone(0);
            }else if(stepnum == 2){
                if(FNODE_SEM.cannotnew == 0)
                    quitZone(0);
                else
                    quitZone(1);
                showTable();
                FNODE_SEM.retype = NODE_SEM(2).retype;
            }
            return;
        case 21:
            if(stepnum == 1){
                NODE_SEM(2).type = NODE_SEM(1).type;
                NODE_SEM(2).arraynum = NODE_SEM(1).arraynum;
            }
            return;
        case 22:
            if(stepnum == 0){
                NODE_SEM(1).type = FNODE_SEM.type;
                NODE_SEM(1).arraynum = FNODE_SEM.arraynum;
            }else if(stepnum == 1){
                NODE_SEM(2).type = FNODE_SEM.type;
                NODE_SEM(2).arraynum = FNODE_SEM.arraynum;
            }
        case 23:
            if(stepnum == 0){
                NODE_SEM(1).type = FNODE_SEM.type;
                NODE_SEM(1).arraynum = FNODE_SEM.arraynum;
            }
            return;
        case 24:
            if(stepnum == 0){
                NODE_SEM(1).type = FNODE_SEM.type;
                NODE_SEM(1).arraynum = FNODE_SEM.arraynum;
            }else if(stepnum == 1){
                int sret = searchTable(NODE(1));
                if(sret == FOUND_IN_SAME_LEVEL)
                    sem_error(NODE(1), "inner redefine var 24");
                else if(sret == FOUND_IN_DIFF_LEVEL)
                    insertTable(NODE(1), 1);
                else if(sret == NOT_FOUND)
                    insertTable(NODE(1), 0);
            }else if(stepnum == 3){
                if(EXP(NODE_SEM(1).type) != NODE_SEM(3).type)
                    sem_error(NODE(1), "type match failed");
            }
            return;
        case 25:
            if(stepnum == 0){
                NODE_SEM(1).type = FNODE_SEM.type;
                NODE_SEM(1).arraynum = FNODE_SEM.arraynum;
            }else if(stepnum == 1){
                int sret = searchTable(NODE(1));
                if(sret == FOUND_IN_SAME_LEVEL)
                    sem_error(NODE(1), "inner redefine var 25");
                else if(sret == FOUND_IN_DIFF_LEVEL)
                    insertTable(NODE(1), 1);
                else if(sret == NOT_FOUND)
                    insertTable(NODE(1), 0);
            }
            return;
        case 26:
            if(stepnum == 2){
                if(NODE_SEM(1).retype != SEM_VOID && NODE_SEM(2).retype != SEM_VOID
                    && NODE_SEM(1).retype != NODE_SEM(2).retype)
                    sem_error(node, "statments return type conflict");
                if(NODE_SEM(1).retype != SEM_VOID)
                    FNODE_SEM.retype = NODE_SEM(1).retype;
                else
                    FNODE_SEM.retype = NODE_SEM(2).retype;
            }
            return;
        case 27:
            if(stepnum == 0)
                FNODE_SEM.retype = SEM_VOID;
            return;
        case 28:
            if(stepnum == 1)
                FNODE_SEM.retype = NODE_SEM(1).retype;
            return;
        case 29:
            if(stepnum == 1)
                FNODE_SEM.retype = SEM_VOID;
            return;
        case 30:
            if(stepnum == 2)
                FNODE_SEM.retype = RET(NODE_SEM(2).type);
            return;
        case 31:
            if(stepnum == 2){
                if(NODE_SEM(2).type != SEM_BOOL_EXP)
                    sem_error(NODE(2), "exp should be a BOOL EXP in if clause");
            }else if(stepnum == 3){
                FNODE_SEM.retype = NODE_SEM(3).retype;
            }
            return;
        case 32:
            if(stepnum == 2){
                if(NODE_SEM(2).type != SEM_BOOL_EXP)
                    sem_error(NODE(2), "exp should be a BOOL EXP in if clause");
            }else if(stepnum == 5){
                if(NODE_SEM(3).retype != SEM_VOID && NODE_SEM(5).retype != SEM_VOID
                    && NODE_SEM(3).retype != NODE_SEM(5).retype)
                    sem_error(node, "statments return type conflict");
                if(NODE_SEM(3).retype != SEM_VOID)
                    FNODE_SEM.retype = NODE_SEM(3).retype;
                else
                    FNODE_SEM.retype = NODE_SEM(5).retype;
            }
            return;
        case 33:
            if(stepnum == 2){
                if(NODE_SEM(2).type != SEM_BOOL_EXP)
                    sem_error(NODE(2), "exp should be a BOOL EXP in while clause");
            }else if(stepnum == 3){
                FNODE_SEM.retype = NODE_SEM(3).retype;
            }
            return;
        case 34:
            if(stepnum == 3){
                if(NODE_SEM(3).type != SEM_BOOL_EXP)
                    sem_error(NODE(3), "exp should be a BOOL EXP in for clause");
            }else if(stepnum == 5){
                FNODE_SEM.retype = NODE_SEM(5).retype;
            }
            return;
        case 37:
            if(stepnum == 3){
                int sret = searchReference(NODE(1), 0);
                if(sret == -1)
                    sem_error(NODE(1), "the var isn't defined");
                if(EXP(table[sret].type) != NODE_SEM(3).type)
                    sem_error(NODE(1), "type match failed");
            }
            return;
        case 38:
            if(stepnum == 3){
                int sret = searchReference(NODE(1), 0);
                if(sret == -1)
                    sem_error(NODE(1), "the var isn't defined");
                if(EXP(table[sret].type) != NODE_SEM(3).type)
                    sem_error(NODE(1), "type match failed");
                if(table[sret].type != SEM_INT && table[sret].type != SEM_FLOAT)
                    sem_error(NODE(1), "+= operator only supports INT and FLOAT");
            }
            return;
        case 39:
            if(stepnum == 3){
                int sret = searchReference(NODE(1), 0);
                if(sret == -1)
                    sem_error(NODE(1), "the var isn't defined");
                if(EXP(table[sret].type) != NODE_SEM(3).type)
                    sem_error(NODE(1), "type match failed");
                if(table[sret].type != SEM_INT && table[sret].type != SEM_FLOAT)
                    sem_error(NODE(1), "+= operator only supports INT and FLOAT");
            }
            return;
        case 42:
            if(stepnum == 2){
                int sret = searchReference(NODE(2), 0);
                if(sret == -1)
                    sem_error(NODE(2), "the var isn't defined");
                if(table[sret].type != SEM_INT)
                    sem_error(NODE(2), "++ operator only supports INT");
            }
            return;
        case 43:
            if(stepnum == 1){
                int sret = searchReference(NODE(1), 0);
                if(sret == -1)
                    sem_error(NODE(1), "the var isn't defined");
                if(table[sret].type != SEM_INT)
                    sem_error(NODE(1), "++ operator only supports INT");
            }
            return;
        case 44:
            if(stepnum == 2){
                int sret = searchReference(NODE(2), 0);
                if(sret == -1)
                    sem_error(NODE(2), "the var isn't defined");
                if(table[sret].type != SEM_INT)
                    sem_error(NODE(2), "-- operator only supports INT");
            }
            return;
        case 45:
            if(stepnum == 1){
                int sret = searchReference(NODE(1), 0);
                if(sret == -1)
                    sem_error(NODE(1), "the var isn't defined");
                if(table[sret].type != SEM_INT)
                    sem_error(NODE(1), "-- operator only supports INT");
            }
            return;
        case 46:
            if(stepnum == 3){
                int sret = searchReference(NODE(1), 0);
                if(sret == -1)
                    sem_error(NODE(1), "the var isn't defined");
                if(EXP(table[sret].type) != NODE_SEM(3).type)
                    sem_error(NODE(1), "type match failed");
            }
            return;
        case 47:
        case 48:
            if(stepnum == 1){
                if(NODE_SEM(1).type != SEM_BOOL_EXP)
                    sem_error(NODE(1), "bool operator only supports BOOL EXP");
            }else if(stepnum == 3){
                if(NODE_SEM(3).type != SEM_BOOL_EXP)
                    sem_error(NODE(3), "bool operator only supports BOOL EXP");
                FNODE_SEM.type = SEM_BOOL_EXP;
            }
            return;
        case 49:
        case 50:
        case 51:
        case 52:
        case 53:
        case 54:
            if(stepnum == 3){
                if(NODE_SEM(1).type != NODE_SEM(3).type || (NODE_SEM(1).type != SEM_INT_EXP
                    && NODE_SEM(1).type != SEM_FLOAT_EXP))
                    sem_error(NODE(1), "cmp operator type match failed");
                FNODE_SEM.type = SEM_BOOL_EXP;
            }
            return;
        case 55:
        case 56:
        case 57:
        case 58:
        case 59:
            if(stepnum == 1){
                if(NODE_SEM(1).type != SEM_INT_EXP && NODE_SEM(1).type != SEM_FLOAT_EXP)
                    sem_error(NODE(1), "arith operator only supports INT or FLOAT EXP");
            }else if(stepnum == 3){
                if(NODE_SEM(3).type != NODE_SEM(1).type)
                    sem_error(NODE(3), "type match failed");
                FNODE_SEM.type = NODE_SEM(3).type;
            }
            return;
        case 60:
            if(stepnum == 1)
                FNODE_SEM.type = SEM_INT_EXP;
            return;
        case 61:
            if(stepnum == 1)
                FNODE_SEM.type = SEM_FLOAT_EXP;
            return;
        case 62:
            if(stepnum == 1){
                int sret = searchReference(NODE(1), 0);
                if(sret == -1)
                    sem_error(NODE(1), "the var isn't defined");
                FNODE_SEM.type = EXP(table[sret].type);
                FNODE_SEM.canLeft = 1;
            }
            return;
        case 63:
            if(stepnum == 1){
                FNODE_SEM.type = NODE_SEM(1).type;
                FNODE_SEM.canLeft = NODE_SEM(1).canLeft;
            }
            return;
        case 64:
            if(stepnum == 2){
                if(NODE_SEM(2).type != SEM_BOOL_EXP)
                    sem_error(NODE(2), "bool operator only supports BOOL EXP");
                FNODE_SEM.type = SEM_BOOL_EXP;
            }
            return;
        case 65:
        case 66:
        case 67:
        case 68:
            if(stepnum == 1){
                if(NODE_SEM(1).type != SEM_INT_EXP)
                    sem_error(NODE(1), "bit operator only supports INT EXP");
            }else if(stepnum == 3){
                if(NODE_SEM(3).type != SEM_INT_EXP)
                    sem_error(NODE(1), "bit operator only supports INT EXP");
                FNODE_SEM.type = SEM_INT_EXP;
            }
            return;
        case 69:
            if(stepnum == 0){
                NODE_SEM(1).argtype = FNODE_SEM.argtype;
                NODE_SEM(1).argnum = FNODE_SEM.argnum;
            }else if(stepnum == 2){
                FNODE_SEM.argnum = NODE_SEM(1).argnum;
                if(FNODE_SEM.argtype != NULL)
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(NODE_SEM(2).type);
                FNODE_SEM.type = SEM_UNDEF_EXP;
            }
            return;
        case 70:
            if(stepnum == 1){
                int sret = searchReference(NODE(1), 0);
                if(sret == -1)
                    sem_error(NODE(1), "the var isn't defined");
                if(table[sret].type < SEM_INT_ARRAY)
                    sem_error(NODE(1), "the var isn't an array");
            }else if(stepnum == 2){
                int sret = searchReference(NODE(1), 0);
                if(NODE_SEM(2).type != SEM_INT_EXP)
                    sem_error(NODE(2), "array index should be an INT EXP");
                FNODE_SEM.type = table[sret].type - SEM_INT_ARRAY;
                FNODE_SEM.canLeft = 1;
            }
            return;
        case 71:
            if(stepnum == 1){
                if(NODE_SEM(1).canLeft != 1)
                    sem_error(NODE(1), "exp1 should be a left value");
            }else if(stepnum == 3){
                if(NODE_SEM(1).type != NODE_SEM(3).type)
                    sem_error(NODE(1), "type match failed");
                FNODE_SEM.type = NODE_SEM(1).type;
            }
            return;
        case 72:
        case 73:
            if(stepnum == 1){
                if(NODE_SEM(1).canLeft != 1)
                    sem_error(NODE(1), "exp1 should be a left value");
            }else if(stepnum == 3){
                if(NODE_SEM(1).type != NODE_SEM(3).type)
                    sem_error(NODE(1), "type match failed");
                if(NODE_SEM(1).type != SEM_INT_EXP && NODE_SEM(1).type != SEM_FLOAT_EXP)
                    sem_error(NODE(1), "+=/-= operator only supports INT or FLOAT EXP");
                FNODE_SEM.type = NODE_SEM(1).type;
            }
            return;
        case 74:
        case 75:
            if(stepnum == 1){
                if(NODE_SEM(1).canLeft != 1 || NODE_SEM(1).type != SEM_INT_EXP)
                    sem_error(NODE(1), "++/-- only supports an INT left value");
            }else if(stepnum == 2){
                FNODE_SEM.type = SEM_INT_EXP;
                FNODE_SEM.canLeft = 1;
            }
            return;
        case 76:
        case 77:
            if(stepnum == 2){
                if(NODE_SEM(2).canLeft != 1 || NODE_SEM(2).type != SEM_INT_EXP)
                    sem_error(NODE(2), "++/-- only supports an INT left value");
                FNODE_SEM.type = SEM_INT_EXP;
                FNODE_SEM.canLeft = 1;
            }
            return;
        case 78:
            if(stepnum == 1)
                FNODE_SEM.type = SEM_INT_EXP;
            return;
        case 79:
            if(stepnum == 1){
                NODE_SEM(1).argtype = (int *) malloc(MAXARG * sizeof(int));
                NODE_SEM(1).argnum = 1;
                NODE_SEM(2).argtype = NODE_SEM(1).argtype;
                NODE_SEM(2).argnum = NODE_SEM(1).argnum;
            }else if(stepnum == 2){
                int sret = searchReference(NODE(1), 1);
                if(sret == -1)
                    sem_error(NODE(1), "function not defined");
                if(sret == -2)
                    sem_error(NODE(1), "function args not match");
                FNODE_SEM.type = table[sret].types[1];
            }
            return;
        case 80:
            if(stepnum == 1){
                NODE_SEM(1).argtype = (int *) malloc(MAXARG * sizeof(int));
                NODE_SEM(1).argnum = 1;
                int sret = searchReference(NODE(1), 1);
                if(sret == -1)
                    sem_error(NODE(1), "function not defined");
                if(sret == -2)
                    sem_error(NODE(1), "function args not match");
                FNODE_SEM.type = table[sret].types[1];
            }
            return;
        default:
            return;
    }
}

int main()
{
    FILE* f = fopen("./Test/SYNTAX_TEST8.txt", "r");
    freopen("./TestRes/TEST8_RES.txt", "w", stdout);
    printf("lex:\n");
    lex_part(f);
    printf("\n\nsyntax:\n");
    syn_part();
    // printf("%d\n", fin);
    showAST(pgm, 0);
    printf("\n\nsemantic:\n");
    dfs(pgm);
    fclose(f);
    return 0;
}