#include "wordunit.h"
#include "synax.h"
#include "symbolTable.h"
#include "sem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))
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
int labelCnt = 0;
char tmpcode[50];

char *sem_type[] = {"int", "float", "char", "func", "int_array", "float_array", "char_array"};
char *branch[] = {"blt", "ble", "bgt", "bge", "beq", "bne"};
char *arith[] = {"add", "sub", "mul", "div", "div"};

int level = 0;
int offset = DX;
int argoffset = ARGDX;
int minsize = DX;
void showTable();

void list_init(list_t* list){
    list->next = list->pre = list;
}

void list_del(list_t* le){
    list_t* pre = le->pre; list_t* next = le->next;
    pre->next = next;
    next->pre = pre;
    list_init(le);
}

void list_add(list_t* pre, list_t* le, list_t* next){
    next->pre = pre->next = le;
    le->next = next;
    le->pre = pre;
}

void list_share(list_t* a, list_t* b){
    a->pre = b->pre;
    a->next = b->next;
    b->pre->next = a;
    b->next->pre = a;
}

void list_cat(list_t* a, list_t* b){
    a->pre->next = b->next;
    a->pre = b->pre;
    b->next->pre = a->pre;
    b->pre->next = a;
}

code* genCode(){
    code* c = (code *) malloc(sizeof(code));
    list_init(&(c->link));
    strcpy(c->assembly, tmpcode);
    return c;
}

char* newlabel(){
    char* lb = (char *) malloc(sizeof(char) * 20);
    sprintf(lb, "label%d", ++labelCnt);
    return lb;
}

void showCode(list_t* hd){
    list_t* p = hd->next;
    while(p != hd){
        code* tmp = link2type(code, p);
        printf("%s", tmp->assembly);
        p = p->next;
    }
}

void addCodePre(list_t* hd, code* c){
    list_add(hd, &(c->link), hd->next);
}

void addCodeBack(list_t* hd, code* c){
    list_add(hd->pre, &(c->link), hd);
}

int getSize(int type){
    if(type == SEM_INT || type == SEM_INT_ARRAY) return INT_SIZE;
    if(type == SEM_FLOAT || type == SEM_FLOAT_ARRAY) return FLOAT_SIZE;
    if(type == SEM_CHAR || type == SEM_CHAR_ARRAY) return CHAR_SIZE;
}

void insertTable(Sentence* id, int opt, int argopt){
    table[++tptr].name = id->tval.name;
    table[tptr].type = id->sem.type;
    table[tptr].level = level;
    if(table[tptr].type != SEM_FUNC){
        table[tptr].size = (getSize(table[tptr].type) * id->sem.arraynum);
        if(argopt){
            table[tptr].offset = argoffset;
            argoffset += table[tptr].size;
        }else{
            offset -= table[tptr].size;
            table[tptr].offset = offset;
            minsize = MIN(offset, minsize);
        }
        id->sem.stkPos = table[tptr].offset;
    }else{
        table[tptr].offset = offset;
        table[tptr].types = id->sem.argtype;
        table[tptr].argnum = &(id->sem.argnum);
    }
    if(opt){
        table[tptr].namedup = 1;
        sprintf(table[tptr].oname, "$tmp%d", ++onameCnt);
    }else{
        table[tptr].namedup = 0;
    }
    showTable();
}

void expTmpInsertTable(int type, Sentence* node){
    table[++tptr].name = (char *) malloc(sizeof(char) * 20);
    sprintf(table[tptr].name, "$tmp%d", ++onameCnt);
    table[tptr].type = type;
    table[tptr].level = level;
    offset -= (getSize(type));
    table[tptr].offset = offset;
    node->sem.stkPos = table[tptr].offset;
    minsize = MIN(offset, minsize);
    table[tptr].namedup = 0;
    table[tptr].size = getSize(type);
}

int searchTable(Sentence* id){
    int cnt = 0, rename = 0;
    for(int i = tptr; i > 0; i--){
        if(strcmp(id->tval.name, table[i].name) == 0){
            rename = 1;
            if(id->sem.type == table[i].type){
                if(id->sem.type != SEM_FUNC){
                    if(level == table[i].level) return FOUND_IN_SAME_LEVEL;
                    else return FOUND_IN_DIFF_LEVEL;
                }else{
                    if(cnt >= 1){
                        cnt++;
                        if(id->sem.argnum != *(table[i].argnum)) continue;
                        int fd = 1;
                        for(int j = 1; j <= id->sem.argnum; j++)
                            if(id->sem.argtype[j] != table[i].types[j]){
                                fd = 0;
                                break;
                            }
                        if(fd) return FUNC_CONFLICT;
                    }
                }
            }
        }
    }
    return rename ? SYMBOL_RENAME : NOT_FOUND;
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

int checkRename(char *name){
    for(int i = tptr; i > 0; i--){
        if(strcmp(name, table[i].name) == 0){
            return 1;
        }
    }
    return 0;
}

void showTable(){
    printf("table:\n");
    printf("%7s %7s %7s %9s %7s %7s\n", "name", "type", "level", "offset", "funarg", "oname");
    for(int i = 1; i <= tptr; i++){
        if(table[i].type != SEM_FUNC){
            printf("%7s %7s %7d %7d        ", table[i].name, sem_type[table[i].type - SEM_INT], table[i].level, table[i].offset);
            if(table[i].namedup)
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

void quitZone(){
    level--;
    tptr = tptrStk[tptrStkTop--];
    offset = table[tptr].offset;
}

void popOne(){
    tptr--;
    offset = table[tptr].offset;
}

void dfs(Sentence* node){
    list_init(&(node->link));
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
                quitZone();
                showTable();
                list_share(&(node->link), &(NODE(1)->link));
            }
            return;
        case 1:
            if(stepnum == 1){
                NODE_SEM(2).type = NODE_SEM(1).type;
                NODE_SEM(2).arraynum = NODE_SEM(1).arraynum;
            }else if(stepnum == 3){
                list_cat(&(NODE(2)->link), &(NODE(3)->link));
                list_share(&(node->link), &(NODE(2)->link));
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
                insertTable(NODE(1), 0, 0);
                NODE_SEM(2).type = FNODE_SEM.type;
                NODE_SEM(2).arraynum = FNODE_SEM.arraynum;
            }else if(stepnum == 2){
                if(NODE_SEM(1).type < SEM_INT_ARRAY)
                    sprintf(tmpcode, "%s: .word 0\n", NODE(1)->tval.name);
                else
                    sprintf(tmpcode, "%s: .space %d\n", NODE(1)->tval.name
                        , getSize(NODE_SEM(1).type) * NODE_SEM(1).arraynum);
                addCodePre(&(NODE(2)->link), genCode());
                list_share(&(node->link), &(NODE(2)->link));
            }
            return;
        case 10:
            if(stepnum == 0){
                NODE_SEM(1).type = FNODE_SEM.type;
                NODE_SEM(1).arraynum = FNODE_SEM.arraynum;
            }else if(stepnum == 1){
                if(searchTable(NODE(1)) != NOT_FOUND)
                    sem_error(NODE(1), "external redefine var");
                insertTable(NODE(1), 0, 0);
                if(NODE_SEM(1).type < SEM_INT_ARRAY)
                    sprintf(tmpcode, "%s: .word 0\n", NODE(1)->tval.name);
                else
                    sprintf(tmpcode, "%s: .space %d\n", NODE(1)->tval.name
                        , getSize(NODE_SEM(1).type) * NODE_SEM(1).arraynum);
                addCodePre(&(node->link), genCode());
            }
            return;
        case 11:
            if(stepnum == 0){
                NODE_SEM(1).type = FNODE_SEM.type;
                NODE_SEM(1).arraynum = FNODE_SEM.arraynum;
            }else if(stepnum == 1){
                sprintf(tmpcode, ".data\n");
                addCodePre(&(NODE(1)->link), genCode());
                list_share(&(node->link), &(NODE(1)->link));
            }
            return;
        case 12:
            if(stepnum == 0)
                NODE_SEM(1).type = FNODE_SEM.type;
            else if(stepnum == 1){
                NODE_SEM(2).cannotnew = 1;
                minsize = DX;
                NODE(2)->jlb.retlabel = newlabel();
            }else if(stepnum == 2){
                if(NODE_SEM(2).retype != FNODE_SEM.type){
                    sem_error(NODE(2), "function return type conflict");
                }
                sprintf(tmpcode, "addiu $fp, $sp, %d\n", -minsize);
                addCodePre(&(NODE(1)->link), genCode());
                sprintf(tmpcode, "sw $fp, %d($sp)\n", -(minsize + 8));
                addCodePre(&(NODE(1)->link), genCode());
                sprintf(tmpcode, "sw $ra, %d($sp)\n", -(minsize + 4));
                addCodePre(&(NODE(1)->link), genCode());
                sprintf(tmpcode, "addiu $sp, $sp, %d\n", minsize);
                addCodePre(&(NODE(1)->link), genCode());
                sprintf(tmpcode, "%s:\n", NODE(1)->next[1]->tval.name);
                addCodePre(&(NODE(1)->link), genCode());
                sprintf(tmpcode, "\n.text\n");
                addCodePre(&(NODE(1)->link), genCode());

                list_cat(&(NODE(1)->link), &(NODE(2)->link));

                sprintf(tmpcode, "%s:\n", NODE(2)->jlb.retlabel);
                addCodeBack(&(NODE(1)->link), genCode());
                sprintf(tmpcode, "lw $fp, %d($sp)\n", -(minsize + 8));
                addCodeBack(&(NODE(1)->link), genCode());
                sprintf(tmpcode, "lw $ra, %d($sp)\n", -(minsize + 4));
                addCodeBack(&(NODE(1)->link), genCode());
                sprintf(tmpcode, "addiu $sp, $sp, %d\n", -minsize);
                addCodeBack(&(NODE(1)->link), genCode());
                // if(strcmp(NODE(1)->next[1]->tval.name, "main") != 0)
                sprintf(tmpcode, "jr $ra\n");
                // else
                    // sprintf(tmpcode, "syscall");
                addCodeBack(&(NODE(1)->link), genCode());

                list_share(&(node->link), &(NODE(1)->link));
            }
            return;
        case 13:
            if(stepnum == 0){
                NODE_SEM(1).type = SEM_FUNC;
                NODE_SEM(1).argtype = (int *) malloc(MAXARG * sizeof(int));
                NODE_SEM(1).argtype[++(NODE_SEM(1).argnum)] = FNODE_SEM.type;
                NODE_SEM(2).argtype = NODE_SEM(1).argtype;
                NODE_SEM(2).argnum = NODE_SEM(1).argnum;
                if(checkRename(NODE(1)->tval.name))
                    sem_error(NODE(1), "function name conflicts with global var or function");
                insertTable(NODE(1), 0, 0);
                newZone(1);
                argoffset = ARGDX;
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
                if(checkRename(NODE(1)->tval.name))
                    sem_error(NODE(1), "function name conflicts with global var or function");
                insertTable(NODE(1), 0, 0);
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
                int sret = searchTable(NODE(2));
                if(sret == FOUND_IN_SAME_LEVEL)
                    sem_error(NODE(2), "function args redefine");
                if(sret != NOT_FOUND)
                    insertTable(NODE(2), 1, 1);
                else
                    insertTable(NODE(2), 0, 1);
                FNODE_SEM.argtype[++FNODE_SEM.argnum] = NODE_SEM(2).type;
            }
            return;
        case 18:
            if(stepnum == 0){
                if(FNODE_SEM.cannotnew == 0)
                    newZone(0);
                NODE(2)->jlb.nextlabel = newlabel();
                NODE(2)->jlb.retlabel = node->jlb.retlabel;
            }else if(stepnum == 2){
                // if(FNODE_SEM.cannotnew == 0)
                //     quitZone(0);
                // else
                //     quitZone(1);
                quitZone();
                showTable();
                FNODE_SEM.retype = NODE_SEM(2).retype;
                list_cat(&(NODE(1)->link), &(NODE(2)->link));
                list_share(&(node->link), &(NODE(1)->link));
            }
            return;
        case 19:
            if(stepnum == 2){
                list_cat(&(NODE(1)->link), &(NODE(2)->link));
                list_share(&(node->link), &(NODE(1)->link));
            }
            return;
        case 21:
            if(stepnum == 1){
                NODE_SEM(2).type = NODE_SEM(1).type;
                NODE_SEM(2).arraynum = NODE_SEM(1).arraynum;
            }else if(stepnum == 2){
                list_share(&(node->link), &(NODE(2)->link));
            }
            return;
        case 22:
            if(stepnum == 0){
                NODE_SEM(1).type = FNODE_SEM.type;
                NODE_SEM(1).arraynum = FNODE_SEM.arraynum;
            }else if(stepnum == 1){
                NODE_SEM(2).type = FNODE_SEM.type;
                NODE_SEM(2).arraynum = FNODE_SEM.arraynum;
            }else if(stepnum == 2){
                list_cat(&(NODE(1)->link), &(NODE(2)->link));
                list_share(&(node->link), &(NODE(1)->link));
            }
            return;
        case 23:
            if(stepnum == 0){
                NODE_SEM(1).type = FNODE_SEM.type;
                NODE_SEM(1).arraynum = FNODE_SEM.arraynum;
            }else if(stepnum == 1){
                list_share(&(node->link), &(NODE(1)->link));
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
                    insertTable(NODE(1), 1, 0);
                else if(sret == NOT_FOUND)
                    insertTable(NODE(1), 0, 0);
            }else if(stepnum == 3){
                if(EXP(NODE_SEM(1).type) != NODE_SEM(3).type)
                    sem_error(NODE(1), "type match failed");
                if(NODE_SEM(3).isTmp && NODE_SEM(3).tmpPos == tptr){
                    popOne();
                    printf("after pop\n");
                    showTable();
                }
                if(NODE_SEM(3).isGlobal){
                    sprintf(tmpcode, "lw $t0, %d($fp)\n", NODE_SEM(3).stkPos);
                    addCodeBack(&(NODE(3)->link), genCode());
                    sprintf(tmpcode, "lw $t1, 0($t0)\n");
                    addCodeBack(&(NODE(3)->link), genCode());
                    sprintf(tmpcode, "sw $t1, %d($fp)\n", NODE_SEM(1).stkPos);
                    addCodeBack(&(NODE(3)->link), genCode());
                }else{
                    sprintf(tmpcode, "lw $t0, %d($fp)\n", NODE_SEM(3).stkPos);
                    addCodeBack(&(NODE(3)->link), genCode());
                    sprintf(tmpcode, "sw $t0, %d($fp)\n", NODE_SEM(1).stkPos);
                    addCodeBack(&(NODE(3)->link), genCode());
                }
                list_share(&(node->link), &(NODE(3)->link));
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
                    insertTable(NODE(1), 1, 0);
                else if(sret == NOT_FOUND)
                    insertTable(NODE(1), 0, 0);
            }
            return;
        case 26:
            if(stepnum == 0){
                NODE(1)->jlb.nextlabel = newlabel();
                NODE(1)->jlb.retlabel = node->jlb.retlabel;
                NODE(2)->jlb.retlabel = node->jlb.retlabel;
            }else if(stepnum == 1){
                NODE(2)->jlb.nextlabel = node->jlb.nextlabel;
            }else if(stepnum == 2){
                if(NODE_SEM(1).retype != SEM_VOID && NODE_SEM(2).retype != SEM_VOID
                    && NODE_SEM(1).retype != NODE_SEM(2).retype)
                    sem_error(node, "statments return type conflict");
                if(NODE_SEM(1).retype != SEM_VOID)
                    FNODE_SEM.retype = NODE_SEM(1).retype;
                else
                    FNODE_SEM.retype = NODE_SEM(2).retype;
                sprintf(tmpcode, "%s:\n", NODE(1)->jlb.nextlabel);
                addCodeBack(&(NODE(1)->link), genCode());
                list_cat(&(NODE(1)->link), &(NODE(2)->link));
                list_share(&(node->link), &(NODE(1)->link));
            }
            return;
        case 27:
            if(stepnum == 0)
                FNODE_SEM.retype = SEM_VOID;
            return;
        case 28:
            if(stepnum == 0){
                NODE(1)->jlb.nextlabel = node->jlb.nextlabel;
                NODE(1)->jlb.retlabel = node->jlb.retlabel;
            }else if(stepnum == 1){
                FNODE_SEM.retype = NODE_SEM(1).retype;
                list_share(&(node->link), &(NODE(1)->link));
            }
            return;
        case 29:
            if(stepnum == 1){
                FNODE_SEM.retype = SEM_VOID;
                list_share(&(node->link), &(NODE(1)->link));
            }
            return;
        case 30:
            if(stepnum == 2){
                FNODE_SEM.retype = RET(NODE_SEM(2).type);
                sprintf(tmpcode, "lw $v1, %d($fp)\n", NODE_SEM(2).stkPos);
                addCodeBack(&(NODE(2)->link), genCode());
                sprintf(tmpcode, "j %s\n", node->jlb.retlabel);
                addCodeBack(&(NODE(2)->link), genCode());
                list_share(&(node->link), &(NODE(2)->link));
            }
            return;
        case 31:
            if(stepnum == 0){
                NODE(2)->jlb.truelabel = newlabel();
                NODE(2)->jlb.falselabel = node->jlb.nextlabel;
                NODE(3)->jlb.retlabel = node->jlb.retlabel;
            }else if(stepnum == 2){
                if(NODE_SEM(2).type != SEM_BOOL_EXP)
                    sem_error(NODE(2), "exp should be a BOOL EXP in if clause");
                NODE(3)->jlb.nextlabel = node->jlb.nextlabel;
            }else if(stepnum == 3){
                FNODE_SEM.retype = NODE_SEM(3).retype;
                sprintf(tmpcode, "%s:\n", NODE(2)->jlb.truelabel);
                addCodeBack(&(NODE(2)->link), genCode());
                list_cat(&(NODE(2)->link), &(NODE(3)->link));
                list_share(&(node->link), &(NODE(2)->link));
            }
            return;
        case 32:
            if(stepnum == 0){
                NODE(2)->jlb.truelabel = newlabel();
                NODE(2)->jlb.falselabel = newlabel();
                NODE(3)->jlb.retlabel = node->jlb.retlabel;
                NODE(5)->jlb.retlabel = node->jlb.retlabel;
            }else if(stepnum == 2){
                if(NODE_SEM(2).type != SEM_BOOL_EXP)
                    sem_error(NODE(2), "exp should be a BOOL EXP in if clause");
                NODE(3)->jlb.nextlabel = node->jlb.nextlabel;
            }else if(stepnum == 4){
                NODE(3)->jlb.nextlabel = node->jlb.nextlabel;
            }else if(stepnum == 5){
                if(NODE_SEM(3).retype != SEM_VOID && NODE_SEM(5).retype != SEM_VOID
                    && NODE_SEM(3).retype != NODE_SEM(5).retype)
                    sem_error(node, "statments return type conflict");
                if(NODE_SEM(3).retype != SEM_VOID)
                    FNODE_SEM.retype = NODE_SEM(3).retype;
                else
                    FNODE_SEM.retype = NODE_SEM(5).retype;
                sprintf(tmpcode, "%s:\n", NODE(2)->jlb.truelabel);
                addCodeBack(&(NODE(2)->link), genCode());
                list_cat(&(NODE(2)->link), &(NODE(3)->link));
                sprintf(tmpcode, "j %s\n", node->jlb.nextlabel);
                addCodeBack(&(NODE(2)->link), genCode());
                sprintf(tmpcode, "%s:\n", NODE(2)->jlb.falselabel);
                addCodeBack(&(NODE(2)->link), genCode());
                list_cat(&(NODE(2)->link), &(NODE(5)->link));
                list_share(&(node->link), &(NODE(2)->link));
            }
            return;
        case 33:
            if(stepnum == 0){
                NODE(2)->jlb.truelabel = newlabel();
                NODE(2)->jlb.falselabel = node->jlb.nextlabel;
                NODE(3)->jlb.retlabel = node->jlb.retlabel;
            }else if(stepnum == 2){
                if(NODE_SEM(2).type != SEM_BOOL_EXP)
                    sem_error(NODE(2), "exp should be a BOOL EXP in while clause");
                NODE(3)->jlb.nextlabel = newlabel();
            }else if(stepnum == 3){
                FNODE_SEM.retype = NODE_SEM(3).retype;
                sprintf(tmpcode, "%s:\n", NODE(3)->jlb.nextlabel);
                addCodePre(&(NODE(2)->link), genCode());
                sprintf(tmpcode, "%s:\n", NODE(2)->jlb.truelabel);
                addCodeBack(&(NODE(2)->link), genCode());
                list_cat(&(NODE(2)->link), &(NODE(3)->link));
                sprintf(tmpcode, "j %s\n", NODE(3)->jlb.nextlabel);
                addCodeBack(&(NODE(2)->link), genCode());
                list_share(&(node->link), &(NODE(2)->link));
            }
            return;
        case 34:
            if(stepnum == 0){
                NODE(3)->jlb.truelabel = newlabel();
                NODE(3)->jlb.falselabel = node->jlb.nextlabel;
                NODE(5)->jlb.retlabel = node->jlb.retlabel;
            }else if(stepnum == 3){
                if(NODE_SEM(3).type != SEM_BOOL_EXP)
                    sem_error(NODE(3), "exp should be a BOOL EXP in for clause");
                NODE(4)->jlb.nextlabel = newlabel();
            }else if(stepnum == 5){
                FNODE_SEM.retype = NODE_SEM(5).retype;
                sprintf(tmpcode, "%s:\n", NODE(4)->jlb.nextlabel);
                addCodeBack(&(NODE(2)->link), genCode());
                list_cat(&(NODE(2)->link), &(NODE(3)->link));
                sprintf(tmpcode, "%s:\n", NODE(3)->jlb.truelabel);
                addCodeBack(&(NODE(2)->link), genCode());
                list_cat(&(NODE(2)->link), &(NODE(5)->link));
                list_cat(&(NODE(2)->link), &(NODE(4)->link));
                sprintf(tmpcode, "j %s\n", NODE(4)->jlb.nextlabel);
                addCodeBack(&(NODE(2)->link), genCode());
                list_share(&(node->link), &(NODE(2)->link));
            }
            return;
        case 35:
            if(stepnum == 2){
                list_cat(&(NODE(1)->link), &(NODE(2)->link));
                list_share(&(node->link), &(NODE(1)->link));
            }
            return;
        case 36:
            if(stepnum == 1){
                list_share(&(node->link), &(NODE(1)->link));
            }
            return;
        case 37:
            if(stepnum == 3){
                int sret = searchReference(NODE(1), 0);
                if(sret == -1)
                    sem_error(NODE(1), "the var isn't defined");
                if(EXP(table[sret].type) != NODE_SEM(3).type)
                    sem_error(NODE(1), "type match failed");
                sprintf(tmpcode, "lw $t0, %d($fp)\n", NODE_SEM(3).stkPos);
                addCodeBack(&(node->link), genCode());
                if(table[sret].level == 0){
                    sprintf(tmpcode, "sw $t0, %s\n", table[sret].name);
                    addCodeBack(&(node->link), genCode());
                }else{
                    sprintf(tmpcode, "sw $t0, %d($fp)\n", table[sret].offset);
                    addCodeBack(&(node->link), genCode());
                }
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
                sprintf(tmpcode, "lw $t0, %d($fp)\n", NODE_SEM(3).stkPos);
                addCodeBack(&(node->link), genCode());
                if(table[sret].level == 0){
                    sprintf(tmpcode, "lw $t1, %s\n", table[sret].name);
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "add $t0, $t1, $t0\n");
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "sw $t0, %s\n", table[sret].name);
                    addCodeBack(&(node->link), genCode());
                }else{
                    sprintf(tmpcode, "lw $t1, %d($fp)\n", table[sret].offset);
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "add $t0, $t1, $t0\n");
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "sw $t0, %d($fp)\n", table[sret].offset);
                    addCodeBack(&(node->link), genCode());
                }
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
                sprintf(tmpcode, "lw $t0, %d($fp)\n", NODE_SEM(3).stkPos);
                addCodeBack(&(node->link), genCode());
                if(table[sret].level == 0){
                    sprintf(tmpcode, "lw $t1, %s\n", table[sret].name);
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "sub $t0, $t1, $t0\n");
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "sw $t0, %s\n", table[sret].name);
                    addCodeBack(&(node->link), genCode());
                }else{
                    sprintf(tmpcode, "lw $t1, %d($fp)\n", table[sret].offset);
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "sub $t0, $t1, $t0\n");
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "sw $t0, %d($fp)\n", table[sret].offset);
                    addCodeBack(&(node->link), genCode());
                }
            }
            return;
        case 40:
            if(stepnum == 2){
                list_cat(&(NODE(1)->link), &(NODE(2)->link));
                list_share(&(node->link), &(NODE(1)->link));
            }
            return;
        case 41:
            if(stepnum == 1){
                list_share(&(node->link), &(NODE(1)->link));
            }
            return;
        case 42:
            if(stepnum == 2){
                int sret = searchReference(NODE(2), 0);
                if(sret == -1)
                    sem_error(NODE(2), "the var isn't defined");
                if(table[sret].type != SEM_INT)
                    sem_error(NODE(2), "++ operator only supports INT");
                if(table[sret].level == 0){
                    sprintf(tmpcode, "lw $t0, %s\n", table[sret].name);
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "addiu $t0, $t0, 1\n");
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "sw $t0, %s\n", table[sret].name);
                    addCodeBack(&(node->link), genCode());
                }else{
                    sprintf(tmpcode, "lw $t0, %d($fp)\n", table[sret].offset);
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "addiu $t0, $t0, 1\n");
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "sw $t0, %d($fp)\n", table[sret].offset);
                    addCodeBack(&(node->link), genCode());
                }
            }
            return;
        case 43:
            if(stepnum == 1){
                int sret = searchReference(NODE(1), 0);
                if(sret == -1)
                    sem_error(NODE(1), "the var isn't defined");
                if(table[sret].type != SEM_INT)
                    sem_error(NODE(1), "++ operator only supports INT");
                if(table[sret].level == 0){
                    sprintf(tmpcode, "lw $t0, %s\n", table[sret].name);
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "addiu $t0, $t0, 1\n");
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "sw $t0, %s\n", table[sret].name);
                    addCodeBack(&(node->link), genCode());
                }else{
                    sprintf(tmpcode, "lw $t0, %d($fp)\n", table[sret].offset);
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "addiu $t0, $t0, 1\n");
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "sw $t0, %d($fp)\n", table[sret].offset);
                    addCodeBack(&(node->link), genCode());
                }
            }
            return;
        case 44:
            if(stepnum == 2){
                int sret = searchReference(NODE(2), 0);
                if(sret == -1)
                    sem_error(NODE(2), "the var isn't defined");
                if(table[sret].type != SEM_INT)
                    sem_error(NODE(2), "-- operator only supports INT");
                if(table[sret].level == 0){
                    sprintf(tmpcode, "lw $t0, %s\n", table[sret].name);
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "subiu $t0, $t0, 1\n");
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "sw $t0, %s\n", table[sret].name);
                    addCodeBack(&(node->link), genCode());
                }else{
                    sprintf(tmpcode, "lw $t0, %d($fp)\n", table[sret].offset);
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "subiu $t0, $t0, 1\n");
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "sw $t0, %d($fp)\n", table[sret].offset);
                    addCodeBack(&(node->link), genCode());
                }
            }
            return;
        case 45:
            if(stepnum == 1){
                int sret = searchReference(NODE(1), 0);
                if(sret == -1)
                    sem_error(NODE(1), "the var isn't defined");
                if(table[sret].type != SEM_INT)
                    sem_error(NODE(1), "-- operator only supports INT");
                if(table[sret].level == 0){
                    sprintf(tmpcode, "lw $t0, %s\n", table[sret].name);
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "subiu $t0, $t0, 1\n");
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "sw $t0, %s\n", table[sret].name);
                    addCodeBack(&(node->link), genCode());
                }else{
                    sprintf(tmpcode, "lw $t0, %d($fp)\n", table[sret].offset);
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "subiu $t0, $t0, 1\n");
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "sw $t0, %d($fp)\n", table[sret].offset);
                    addCodeBack(&(node->link), genCode());
                }
            }
            return;
        case 46:
            if(stepnum == 3){
                int sret = searchReference(NODE(1), 0);
                if(sret == -1)
                    sem_error(NODE(1), "the var isn't defined");
                if(EXP(table[sret].type) != NODE_SEM(3).type)
                    sem_error(NODE(1), "type match failed");
                sprintf(tmpcode, "lw $t0, %d($fp)\n", NODE_SEM(3).stkPos);
                addCodeBack(&(node->link), genCode());
                if(table[sret].level == 0){
                    sprintf(tmpcode, "sw $t0, %s\n", table[sret].name);
                    addCodeBack(&(node->link), genCode());
                }else{
                    sprintf(tmpcode, "sw $t0, %d($fp)\n", table[sret].offset);
                    addCodeBack(&(node->link), genCode());
                }
            }
            return;
        case 47:
        case 48:
            if(stepnum == 0){
                if(rulenum == 47){
                    NODE(1)->jlb.truelabel = newlabel();
                    NODE(1)->jlb.falselabel = node->jlb.falselabel;
                }else{
                    NODE(1)->jlb.truelabel = node->jlb.truelabel;
                    NODE(1)->jlb.falselabel = newlabel();
                }
            }else if(stepnum == 1){
                if(NODE_SEM(1).type != SEM_BOOL_EXP)
                    sem_error(NODE(1), "bool operator only supports BOOL EXP");
            }else if(stepnum == 2){
                NODE(3)->jlb.truelabel = node->jlb.truelabel;
                NODE(3)->jlb.falselabel = node->jlb.falselabel;
            }else if(stepnum == 3){
                if(NODE_SEM(3).type != SEM_BOOL_EXP)
                    sem_error(NODE(3), "bool operator only supports BOOL EXP");
                FNODE_SEM.type = SEM_BOOL_EXP;
                if(FNODE_SEM.argtype != NULL){
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(FNODE_SEM.type);
                }
                if(rulenum == 47){
                    sprintf(tmpcode, "%s:\n", NODE(1)->jlb.truelabel);
                    addCodeBack(&(NODE(1)->link), genCode());
                    list_cat(&(NODE(1)->link), &(NODE(3)->link));
                    list_share(&(node->link), &(NODE(1)->link));
                }else{
                    sprintf(tmpcode, "%s:\n", NODE(1)->jlb.falselabel);
                    addCodeBack(&(NODE(1)->link), genCode());
                    list_cat(&(NODE(1)->link), &(NODE(3)->link));
                    list_share(&(node->link), &(NODE(1)->link));
                }
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
                if(FNODE_SEM.argtype != NULL){
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(FNODE_SEM.type);
                }

                list_cat(&(NODE(1)->link), &(NODE(3)->link));
                sprintf(tmpcode, "lw $t0, %d($fp)\n", NODE_SEM(1).stkPos);
                addCodeBack(&(NODE(1)->link), genCode());
                if(NODE_SEM(1).isGlobal){
                    sprintf(tmpcode, "lw $t0, 0($t0)\n");
                    addCodeBack(&(NODE(1)->link), genCode());
                }
                sprintf(tmpcode, "lw $t1, %d($fp)\n", NODE_SEM(3).stkPos);
                addCodeBack(&(NODE(1)->link), genCode());
                if(NODE_SEM(3).isGlobal){
                    sprintf(tmpcode, "lw $t1, 0($t1)\n");
                    addCodeBack(&(NODE(1)->link), genCode());
                }
                sprintf(tmpcode, "%s $t0, $t1, %s\n", branch[rulenum - 49], node->jlb.truelabel);
                addCodeBack(&(NODE(1)->link), genCode());
                sprintf(tmpcode, "j %s\n", node->jlb.falselabel);
                addCodeBack(&(NODE(1)->link), genCode());
                list_share(&(node->link), &(NODE(1)->link));
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
                if(FNODE_SEM.argtype != NULL){
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(FNODE_SEM.type);
                    node->arg[FNODE_SEM.argnum] = node;
                }
                if(NODE_SEM(3).isTmp && NODE_SEM(3).tmpPos == tptr){
                    popOne();
                    printf("after pop\n");
                    showTable();
                }
                if(NODE_SEM(1).isTmp && NODE_SEM(1).tmpPos == tptr){
                    popOne();
                    printf("after pop\n");
                    showTable();
                }
                expTmpInsertTable(RET(FNODE_SEM.type), node);
                FNODE_SEM.isTmp = 1;
                FNODE_SEM.tmpPos = tptr;
                showTable();

                list_cat(&(NODE(1)->link), &(NODE(3)->link));
                sprintf(tmpcode, "lw $t0, %d($fp)\n", NODE_SEM(1).stkPos);
                addCodeBack(&(NODE(1)->link), genCode());
                if(NODE_SEM(1).isGlobal){
                    sprintf(tmpcode, "lw $t0, 0($t0)\n");
                    addCodeBack(&(NODE(1)->link), genCode());
                }
                sprintf(tmpcode, "lw $t1, %d($fp)\n", NODE_SEM(3).stkPos);
                addCodeBack(&(NODE(1)->link), genCode());
                if(NODE_SEM(3).isGlobal){
                    sprintf(tmpcode, "lw $t1, 0($t1)\n");
                    addCodeBack(&(NODE(1)->link), genCode());
                }
                sprintf(tmpcode, "%s $t0, $t0, $t1\n", arith[rulenum - 55]);
                addCodeBack(&(NODE(1)->link), genCode());
                sprintf(tmpcode, "sw $t0, %d($fp)\n", FNODE_SEM.stkPos);
                addCodeBack(&(NODE(1)->link), genCode());
                list_share(&(node->link), &(NODE(1)->link));
            }
            return;
        case 60:
            if(stepnum == 1){
                FNODE_SEM.type = SEM_INT_EXP;
                if(FNODE_SEM.argtype != NULL){
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(FNODE_SEM.type);
                    node->arg[FNODE_SEM.argnum] = node;
                }
                expTmpInsertTable(RET(FNODE_SEM.type), node);
                FNODE_SEM.isTmp = 1;
                FNODE_SEM.tmpPos = tptr;
                showTable();

                sprintf(tmpcode, "addiu $t0, $0, %d\n", NODE(1)->tval.ivalue);
                addCodeBack(&(node->link), genCode());
                sprintf(tmpcode, "sw $t0, %d($fp)\n", FNODE_SEM.stkPos);
                addCodeBack(&(node->link), genCode());
            }
            return;
        case 61:
            if(stepnum == 1){
                FNODE_SEM.type = SEM_FLOAT_EXP;
                if(FNODE_SEM.argtype != NULL){
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(FNODE_SEM.type);
                    node->arg[FNODE_SEM.argnum] = node;
                }
                expTmpInsertTable(RET(FNODE_SEM.type), node);
                FNODE_SEM.isTmp = 1;
                FNODE_SEM.tmpPos = tptr;
                showTable();

                sprintf(tmpcode, "addiu $t0, $0, %d\n", (int)NODE(1)->tval.fvalue);
                addCodeBack(&(node->link), genCode());
                sprintf(tmpcode, "sw $t0, %d($fp)\n", FNODE_SEM.stkPos);
                addCodeBack(&(node->link), genCode());
            }
            return;
        case 62:
            if(stepnum == 1){
                int sret = searchReference(NODE(1), 0);
                if(sret == -1)
                    sem_error(NODE(1), "the var isn't defined");
                FNODE_SEM.type = EXP(table[sret].type);
                FNODE_SEM.canLeft = 1;
                if(FNODE_SEM.argtype != NULL){
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(FNODE_SEM.type);
                    node->arg[FNODE_SEM.argnum] = node;
                }
                if(table[sret].level == 0){
                    // use address to acess global var
                    expTmpInsertTable(SEM_INT, node);
                    showTable();
                    sprintf(tmpcode, "la $t0, %s\n", NODE(1)->tval.name);
                    addCodeBack(&(node->link), genCode());
                    sprintf(tmpcode, "sw $t0, %d($fp)\n", FNODE_SEM.stkPos);
                    addCodeBack(&(node->link), genCode());
                    FNODE_SEM.isGlobal = 1;
                }else{
                    FNODE_SEM.stkPos = table[sret].offset;
                }
            }
            return;
        case 63:
            if(stepnum == 1){
                FNODE_SEM.type = NODE_SEM(1).type;
                FNODE_SEM.canLeft = NODE_SEM(1).canLeft;
                if(FNODE_SEM.argtype != NULL){
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(FNODE_SEM.type);
                    node->arg[FNODE_SEM.argnum] = node;
                }
                FNODE_SEM.isTmp = NODE_SEM(1).isTmp;
                FNODE_SEM.tmpPos = NODE_SEM(1).tmpPos;
                FNODE_SEM.stkPos = NODE_SEM(1).stkPos;
                FNODE_SEM.isGlobal = NODE_SEM(1).isGlobal;
                list_share(&(node->link), &(NODE(1)->link));
            }
            return;
        case 64:
            if(stepnum == 0){
                NODE(2)->jlb.truelabel = node->jlb.falselabel;
                NODE(2)->jlb.falselabel = node->jlb.truelabel;
            }else if(stepnum == 2){
                if(NODE_SEM(2).type != SEM_BOOL_EXP)
                    sem_error(NODE(2), "bool operator only supports BOOL EXP");
                FNODE_SEM.type = SEM_BOOL_EXP;
                if(FNODE_SEM.argtype != NULL)
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(FNODE_SEM.type);
                list_share(&(node->link), &(NODE(2)->link));
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
                if(FNODE_SEM.argtype != NULL)
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(FNODE_SEM.type);
                if(NODE_SEM(3).isTmp && NODE_SEM(3).tmpPos == tptr){
                    popOne();
                    printf("after pop\n");
                    showTable();
                }
                if(NODE_SEM(1).isTmp && NODE_SEM(1).tmpPos == tptr){
                    popOne();
                    printf("after pop\n");
                    showTable();
                }
                expTmpInsertTable(RET(FNODE_SEM.type), node);
                FNODE_SEM.isTmp = 1;
                FNODE_SEM.tmpPos = tptr;
                showTable();

                // logic arith not support yet;
            }
            return;
        case 69:
            if(stepnum == 0){
                NODE_SEM(1).argtype = FNODE_SEM.argtype;
                NODE(1)->arg = node->arg;
                NODE_SEM(1).argnum = FNODE_SEM.argnum;
            }else if(stepnum == 2){
                FNODE_SEM.argnum = NODE_SEM(1).argnum;
                FNODE_SEM.type = SEM_UNDEF_EXP;
                if(FNODE_SEM.argtype != NULL){
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(NODE_SEM(2).type);
                    node->arg[FNODE_SEM.argnum] = node;
                }

                list_cat(&(NODE(1)->link), &(NODE(2)->link));
                list_share(&(node->link), &(NODE(1)->link));
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
                FNODE_SEM.type = EXP(table[sret].type - SEM_INT_ARRAY + SEM_INT);
                FNODE_SEM.canLeft = 1;
                if(FNODE_SEM.argtype != NULL){
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(FNODE_SEM.type);
                    node->arg[FNODE_SEM.argnum] = node;
                }
                
                expTmpInsertTable(SEM_INT, node);
                showTable();
                if(table[sret].level == 0){
                    // use address to acess global array
                    sprintf(tmpcode, "la $t0, %s\n", NODE(1)->tval.name);
                    addCodeBack(&(NODE(2)->link), genCode());
                    sprintf(tmpcode, "lw $t1, %d($fp)\n", NODE_SEM(2).stkPos);
                    addCodeBack(&(NODE(2)->link), genCode());
                    if(NODE_SEM(2).isGlobal){
                        sprintf(tmpcode, "lw $t1, 0($t1)\n");
                        addCodeBack(&(NODE(2)->link), genCode());
                    }
                    sprintf(tmpcode, "sll $t1, $t1, 2\n");
                    addCodeBack(&(NODE(2)->link), genCode());
                    sprintf(tmpcode, "add $t0, $t0, $t1\n");
                    addCodeBack(&(NODE(2)->link), genCode());
                    sprintf(tmpcode, "sw $t0, %d($fp)\n", FNODE_SEM.stkPos);
                    addCodeBack(&(NODE(2)->link), genCode());
                }else{
                    sprintf(tmpcode, "lw $t0, %d($fp)\n", NODE_SEM(2).stkPos);
                    addCodeBack(&(NODE(2)->link), genCode());
                    if(NODE_SEM(2).isGlobal){
                        sprintf(tmpcode, "lw $t0, 0($t0)\n");
                        addCodeBack(&(NODE(2)->link), genCode());
                    }
                    sprintf(tmpcode, "sll $t0, $t0, 2\n");
                    addCodeBack(&(NODE(2)->link), genCode());
                    sprintf(tmpcode, "sub $t0, $t0, %d\n", table[sret].offset);
                    addCodeBack(&(NODE(2)->link), genCode());
                    sprintf(tmpcode, "sub $t0, $fp, $t0\n");
                    addCodeBack(&(NODE(2)->link), genCode());
                    sprintf(tmpcode, "sw $t0, %d($fp)\n", FNODE_SEM.stkPos);
                }
                list_share(&(node->link), &(NODE(2)->link));
                FNODE_SEM.isGlobal = 1;
            }
            return;
        case 71:
            if(stepnum == 1){
                if(NODE_SEM(1).canLeft != 1)
                    sem_error(NODE(1), "exp1 should be a left value");
            }else if(stepnum == 3){
                if(NODE_SEM(1).type != NODE_SEM(3).type)
                    sem_error(NODE(1), "\'=\' type match failed");
                FNODE_SEM.type = NODE_SEM(1).type;
                // FNODE_SEM.stkPos , FNODE_SEM.isGlobal
                if(FNODE_SEM.argtype != NULL){
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(FNODE_SEM.type);
                    node->arg[FNODE_SEM.argnum] = node;
                }
                if(NODE_SEM(3).isTmp && NODE_SEM(3).tmpPos == tptr){
                    popOne();
                    printf("after pop\n");
                    showTable();
                }

                sprintf(tmpcode, "lw $t0, %d($fp)\n", NODE_SEM(3).stkPos);
                addCodeBack(&(NODE(3)->link), genCode());
                if(NODE_SEM(3).isGlobal){
                    sprintf(tmpcode, "lw $t0, 0($t0)\n");
                    addCodeBack(&(NODE(3)->link), genCode());
                }
                if(NODE_SEM(1).isGlobal){
                    sprintf(tmpcode, "lw $t1, %d($fp)\n", NODE_SEM(1).stkPos);
                    addCodeBack(&(NODE(3)->link), genCode());
                    sprintf(tmpcode, "sw $t0, 0($t1)\n");
                    addCodeBack(&(NODE(3)->link), genCode());
                }else{
                    sprintf(tmpcode, "sw $t0, %d($fp)\n", NODE_SEM(1).stkPos);
                    addCodeBack(&(NODE(3)->link), genCode());
                }
                list_cat(&(NODE(1)->link), &(NODE(3)->link));
                list_share(&(node->link), &(NODE(1)->link));
                FNODE_SEM.isGlobal = NODE_SEM(1).isGlobal;
                FNODE_SEM.stkPos = NODE_SEM(1).stkPos;
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
                // FNODE_SEM.stkPos , FNODE_SEM.isGlobal
                if(FNODE_SEM.argtype != NULL){
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(FNODE_SEM.type);
                    node->arg[FNODE_SEM.argnum] = node;
                }
                if(NODE_SEM(3).isTmp && NODE_SEM(3).tmpPos == tptr){
                    popOne();
                    printf("after pop\n");
                    showTable();
                }

                sprintf(tmpcode, "lw $t0, %d($fp)\n", NODE_SEM(3).stkPos);
                addCodeBack(&(NODE(3)->link), genCode());
                if(NODE_SEM(3).isGlobal){
                    sprintf(tmpcode, "lw $t0, 0($t0)\n");
                    addCodeBack(&(NODE(3)->link), genCode());
                }
                if(NODE_SEM(1).isGlobal){
                    sprintf(tmpcode, "lw $t1, %d($fp)\n", NODE_SEM(1).stkPos);
                    addCodeBack(&(NODE(3)->link), genCode());
                    sprintf(tmpcode, "lw $t2, 0($t1)\n");
                    addCodeBack(&(NODE(3)->link), genCode());
                    if(rulenum == 72)
                        sprintf(tmpcode, "add $t0, $t2, $t0\n");
                    else
                        sprintf(tmpcode, "sub $t0, $t2, $t0\n");
                    addCodeBack(&(NODE(3)->link), genCode());
                    sprintf(tmpcode, "sw $t0, 0($t1)\n");
                    addCodeBack(&(NODE(3)->link), genCode());
                }else{
                    sprintf(tmpcode, "lw $t1, %d($fp)\n", NODE_SEM(1).stkPos);
                    addCodeBack(&(NODE(3)->link), genCode());
                    if(rulenum == 72)
                        sprintf(tmpcode, "add $t0, $t1, $t0\n");
                    else
                        sprintf(tmpcode, "sub $t0, $t1, $t0\n");
                    addCodeBack(&(NODE(3)->link), genCode());
                    sprintf(tmpcode, "sw $t0, %d($fp)\n", NODE_SEM(1).stkPos);
                    addCodeBack(&(NODE(3)->link), genCode());
                }
                list_cat(&(NODE(1)->link), &(NODE(3)->link));
                list_share(&(node->link), &(NODE(1)->link));
                FNODE_SEM.isGlobal = NODE_SEM(1).isGlobal;
                FNODE_SEM.stkPos = NODE_SEM(1).stkPos;
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
                // FNODE_SEM.stkPos , FNODE_SEM.isGlobal
                if(FNODE_SEM.argtype != NULL){
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(FNODE_SEM.type);
                    node->arg[FNODE_SEM.argnum] = node;
                }

                sprintf(tmpcode, "lw $t0, %d($fp)\n", NODE_SEM(1).stkPos);
                addCodeBack(&(NODE(1)->link), genCode());
                if(NODE_SEM(1).isGlobal){
                    sprintf(tmpcode, "lw $t1, 0($t0)\n");
                    addCodeBack(&(NODE(1)->link), genCode());
                    if(rulenum == 74)
                        sprintf(tmpcode, "addiu $t1, $t1, 1\n");
                    else 
                        sprintf(tmpcode, "subiu $t1, $t1, 1\n");
                    addCodeBack(&(NODE(1)->link), genCode());
                    sprintf(tmpcode, "sw $t1, 0($t0)\n");
                    addCodeBack(&(NODE(1)->link), genCode());
                }else{
                    if(rulenum == 74)
                        sprintf(tmpcode, "addiu $t0, $t0, 1\n");
                    else 
                        sprintf(tmpcode, "subiu $t0, $t0, 1\n");
                    addCodeBack(&(NODE(1)->link), genCode());
                    sprintf(tmpcode, "sw $t0, %d($fp)\n", NODE_SEM(1).stkPos);
                    addCodeBack(&(NODE(1)->link), genCode());
                }
                list_share(&(node->link), &(NODE(1)->link));
                FNODE_SEM.isGlobal = NODE_SEM(1).isGlobal;
                FNODE_SEM.stkPos = NODE_SEM(1).stkPos;
            }
            return;
        case 76:
        case 77:
            if(stepnum == 2){
                if(NODE_SEM(2).canLeft != 1 || NODE_SEM(2).type != SEM_INT_EXP)
                    sem_error(NODE(2), "++/-- only supports an INT left value");
                FNODE_SEM.type = SEM_INT_EXP;
                FNODE_SEM.canLeft = 1;
                if(FNODE_SEM.argtype != NULL){
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(FNODE_SEM.type);
                    node->arg[FNODE_SEM.argnum] = node;
                }

                sprintf(tmpcode, "lw $t0, %d($fp)\n", NODE_SEM(2).stkPos);
                    addCodeBack(&(NODE(2)->link), genCode());
                    if(NODE_SEM(2).isGlobal){
                        sprintf(tmpcode, "lw $t1, 0($t0)\n");
                        addCodeBack(&(NODE(2)->link), genCode());
                        if(rulenum == 76)
                            sprintf(tmpcode, "addiu $t1, $t1, 1\n");
                        else 
                            sprintf(tmpcode, "subiu $t1, $t1, 1\n");
                        addCodeBack(&(NODE(2)->link), genCode());
                        sprintf(tmpcode, "sw $t1, 0($t0)\n");
                        addCodeBack(&(NODE(2)->link), genCode());
                    }else{
                        if(rulenum == 76)
                            sprintf(tmpcode, "addiu $t0, $t0, 1\n");
                        else 
                            sprintf(tmpcode, "subiu $t0, $t0, 1\n");
                        addCodeBack(&(NODE(2)->link), genCode());
                        sprintf(tmpcode, "sw $t0, %d($fp)\n", NODE_SEM(2).stkPos);
                        addCodeBack(&(NODE(2)->link), genCode());
                    }
                    list_share(&(node->link), &(NODE(2)->link));
                    FNODE_SEM.isGlobal = NODE_SEM(2).isGlobal;
                    FNODE_SEM.stkPos = NODE_SEM(2).stkPos;
            }

            return;
        case 78:
            if(stepnum == 1){
                FNODE_SEM.type = SEM_INT_EXP;
                if(FNODE_SEM.argtype != NULL){
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(FNODE_SEM.type);
                    node->arg[FNODE_SEM.argnum] = node;
                }
                expTmpInsertTable(RET(FNODE_SEM.type), node);
                FNODE_SEM.isTmp = 1;
                FNODE_SEM.tmpPos = tptr;
                showTable();

                sprintf(tmpcode, "addiu $t0, $0, %d\n", (int)NODE(1)->tval.cvalue);
                addCodeBack(&(node->link), genCode());
                sprintf(tmpcode, "sw $t0, %d($fp)\n", FNODE_SEM.stkPos);
                addCodeBack(&(node->link), genCode());
            }
            return;
        case 79:
            if(stepnum == 1){
                NODE_SEM(1).argtype = (int *) malloc(MAXARG * sizeof(int));
                NODE(1)->arg = (Sentence **) malloc(MAXARG * sizeof(Sentence *));
                NODE_SEM(1).argnum = 1;
                NODE_SEM(2).argtype = NODE_SEM(1).argtype;
                NODE(2)->arg = NODE(1)->arg;
                NODE_SEM(2).argnum = NODE_SEM(1).argnum;
            }else if(stepnum == 2){
                NODE_SEM(1).argnum = NODE_SEM(2).argnum;
                // printf("%d\n", NODE_SEM(1).argnum);
                // for(int j = 2; j <= NODE_SEM(1).argnum; j++)
                int sret = searchReference(NODE(1), 1);
                if(sret == -1)
                    sem_error(NODE(1), "function not defined");
                if(sret == -2)
                    sem_error(NODE(1), "function args not match");
                FNODE_SEM.type = EXP(table[sret].types[1]);
                if(FNODE_SEM.argtype != NULL){
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(FNODE_SEM.type);
                    node->arg[FNODE_SEM.argnum] = node;
                }
                expTmpInsertTable(RET(FNODE_SEM.type), node);
                FNODE_SEM.isTmp = 1;
                FNODE_SEM.tmpPos = tptr;
                showTable();

                sprintf(tmpcode, "sw $t2, -4($sp)\n");
                addCodeBack(&(NODE(2)->link), genCode());
                sprintf(tmpcode, "sw $t1, -8($sp)\n");
                addCodeBack(&(NODE(2)->link), genCode());
                sprintf(tmpcode, "sw $t0, -12($sp)\n");
                addCodeBack(&(NODE(2)->link), genCode());
                int tmpoffset = -16;
                for(int i = NODE_SEM(1).argnum; i >= 2; i--){
                    sprintf(tmpcode, "lw $t0, %d($fp)\n", NODE(1)->arg[i]->sem.stkPos);
                    addCodeBack(&(NODE(2)->link), genCode());
                    if(NODE(1)->arg[i]->sem.isGlobal){
                        sprintf(tmpcode, "lw $t0, 0($t0)\n");
                        addCodeBack(&(NODE(2)->link), genCode());
                    }
                    sprintf(tmpcode, "sw $t0, %d($sp)\n", tmpoffset);
                    addCodeBack(&(NODE(2)->link), genCode());
                    tmpoffset -= 4;
                }
                tmpoffset += 4;
                sprintf(tmpcode, "addiu $sp, $sp, %d\n", tmpoffset);
                addCodeBack(&(NODE(2)->link), genCode());
                sprintf(tmpcode, "jal %s\n", NODE(1)->tval.name);
                addCodeBack(&(NODE(2)->link), genCode());
                sprintf(tmpcode, "sw $v1, %d($fp)\n", FNODE_SEM.stkPos);
                addCodeBack(&(NODE(2)->link), genCode());
                sprintf(tmpcode, "addiu $sp, $sp, %d\n", -tmpoffset);
                addCodeBack(&(NODE(2)->link), genCode());
                sprintf(tmpcode, "lw $t0, -12($sp)\n");
                addCodeBack(&(NODE(2)->link), genCode());
                sprintf(tmpcode, "lw $t1, -8($sp)\n");
                addCodeBack(&(NODE(2)->link), genCode());
                sprintf(tmpcode, "lw $t2, -4($sp)\n");
                addCodeBack(&(NODE(2)->link), genCode());
                list_share(&(node->link), &(NODE(2)->link));
            }
            return;
        case 80:
            if(stepnum == 1){
                NODE_SEM(1).argtype = (int *) malloc(MAXARG * sizeof(int));
                NODE(1)->arg = (Sentence **) malloc(MAXARG * sizeof(Sentence *));
                NODE_SEM(1).argnum = 1;
                int sret = searchReference(NODE(1), 1);
                if(sret == -1)
                    sem_error(NODE(1), "function not defined");
                if(sret == -2)
                    sem_error(NODE(1), "function args not match");
                FNODE_SEM.type = EXP(table[sret].types[1]);
                if(FNODE_SEM.argtype != NULL){
                    FNODE_SEM.argtype[++(FNODE_SEM.argnum)] = RET(FNODE_SEM.type);
                    node->arg[FNODE_SEM.argnum] = node;
                }
                expTmpInsertTable(RET(FNODE_SEM.type), node);
                FNODE_SEM.isTmp = 1;
                FNODE_SEM.tmpPos = tptr;
                showTable();

                sprintf(tmpcode, "sw $t2, -4($sp)\n");
                addCodeBack(&(node->link), genCode());
                sprintf(tmpcode, "sw $t1, -8($sp)\n");
                addCodeBack(&(node->link), genCode());
                sprintf(tmpcode, "sw $t0, -12($sp)\n");
                addCodeBack(&(node->link), genCode());

                sprintf(tmpcode, "addiu $sp, $sp, -12\n");
                addCodeBack(&(node->link), genCode());
                sprintf(tmpcode, "jal %s\n", NODE(1)->tval.name);
                addCodeBack(&(node->link), genCode());
                sprintf(tmpcode, "sw $v1, %d($fp)\n", FNODE_SEM.stkPos);
                addCodeBack(&(node->link), genCode());
                sprintf(tmpcode, "addiu $sp, $sp, 12\n");
                addCodeBack(&(node->link), genCode());
                sprintf(tmpcode, "lw $t0, -12($sp)\n");
                addCodeBack(&(node->link), genCode());
                sprintf(tmpcode, "lw $t1, -8($sp)\n");
                addCodeBack(&(node->link), genCode());
                sprintf(tmpcode, "lw $t2, -4($sp)\n");
                addCodeBack(&(node->link), genCode());
            }
            return;
        default:
            return;
    }
}

int main()
{
    // FILE* f = fopen("./Test/SEM_TEST1.txt", "r");
    // freopen("./TestRes/SEMTEST1_RES.txt", "w", stdout);
    // FILE* f = fopen("./sem_test.c", "r");
    // freopen("./TestRes/SEM_TEST.txt", "w", stdout);
    FILE* f = fopen("./Test/final_test3.txt", "r");
    freopen("./TestRes/final_res3.txt", "w", stdout);
    printf("lex:\n");
    lex_part(f);
    printf("\n\nsyntax:\n");
    syn_part();
    // printf("%d\n", fin);
    showAST(pgm, 0);
    printf("\n\nsemantic:\n");
    dfs(pgm);
    printf("\nsemantic analyse passed\n");
    printf("\n\ncode:\n");
    showCode(&(pgm->link));
    fclose(f);
    return 0;
}