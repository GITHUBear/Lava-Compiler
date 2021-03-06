#include "wordunit.h"
#include "dfa.h"
#include "synax.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const char* keywords[MAXSIZE]; 
extern const char* words_type[MAXSIZE];
extern const char* op_type[MAXSIZE];
extern const char* sep_type[MAXSIZE];
extern const char* op_detail_type[MAXSIZE];
Word words[MAXWORDS];
int stateStk[STACKMAX];
Sentence* stcStk[STACKMAX];
int syn_analysis_state;
int wordsSize;
int stkTop;
Sentence* pgm;
int fin;

const char* synax_ele[] = {"program", "deflist", "tp", "def", "vallist", "fundef",
                              "stmtlist", "args", "param", "stmt", "exp", "initlist", "init",
                              "steplist", "step", "compstmt", "innerdeflist", "innerdef",
                              "innerdeclist", "innerdec", "end of program"};
const int grammar_length[] = {1, 3, 0, 1, 1, 1, 4, 4, 4, 3, 1, 2, 2, 4, 3, 3, 1, 2, 4, 2, 0, 3, 3, 1, 3, 1, 2, 0, 1,
                             2, 3, 5, 7, 5, 9, 3, 1, 3, 3, 3, 3, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3,
                             3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 3, 2, 3, 3, 3, 3, 3, 4, 3, 3, 3, 2, 2, 2, 2, 1, 4, 3};
const int grammar_rule[] = {program, deflist, deflist, tp, tp, tp, tp, tp, tp, vallist, vallist, def, def,
                             fundef, fundef, args, args, param, compstmt, innerdeflist, innerdeflist, innerdef,
                             innerdeclist, innerdeclist, innerdec, innerdec, stmtlist, stmtlist, stmt, 
                             stmt, stmt, stmt, stmt, stmt, stmt, initlist, initlist,
                             init, init, init, steplist, steplist, step, step, step, step, step,
                             exp, exp, exp, exp, exp, exp, exp, exp, exp, exp, exp, exp, 
                             exp, exp, exp, exp, exp, exp, exp, exp, exp, exp, exp, exp,
                             exp, exp, exp, exp, exp, exp, exp, exp, exp, exp};

int scanIdx;

void printStcInfo(Sentence* node){
    int nodetype = node->type;
    if(nodetype == SYNAXELE)
        printf("\'%s\'\n", synax_ele[node->tval.synVal - program]);
    else{
        switch(nodetype){
        case ID:
            printf("ID:  [%s]\n", node->tval.name);
            break;
        case KEYWORD:
            printf("\'%s\'\n", keywords[node->tval.keyIdx]);
            break;
        case INUM:
            printf("INTEGER:  [%d]\n", node->tval.ivalue);
            break;
        case FNUM:
            printf("FLOAT:  [%f]\n", node->tval.fvalue);
            break;
        case CNUM:
            printf("CHAR:  [\'%c\']\n", node->tval.cvalue);
            break;
        case OPERATER:
            printf("OPERATER:  (%s, %s)\n", op_type[node->tval.opType - UNARY_OP], op_detail_type[node->op - BIADD]);
            break;
        case SEPERATOR:
            printf("[%s]\n", sep_type[node->tval.sepType - COMMA]);
            break;
        }
    }
}

void syntax_err(Sentence* stc){
    printf("syntax_err: at line: %d can't deal with ", stc->line); 
    printStcInfo(stc);
    printf("near ");
    printStcInfo(words + scanIdx);
    exit(0);
}

Sentence* mkNode(int ruleNum){
    Sentence* newnode = (Sentence*) malloc(sizeof(Sentence));
    newnode->type = SYNAXELE;
    newnode->tval.synVal = grammar_rule[ruleNum];
    newnode->irule = ruleNum;
    int startpos = stkTop - grammar_length[ruleNum] + 1;
    int endpos = stkTop;
    pgm = newnode;
    for(int i = 1; startpos <= endpos; startpos++){
        if(stcStk[startpos]->type == SEPERATOR) continue;
        newnode->next[i] = stcStk[startpos];
        i++;
    }
    return newnode;
}

Sentence* shift(int nextState, Sentence* stc, int canGo){
    // printf("shift %d\n", nextState);
    if(canGo){
        scanIdx++;
    }
    stateStk[++stkTop] = nextState;
    stcStk[stkTop] = stc;
    syn_analysis_state = nextState;
    return NULL;
}

Sentence* reduce(int ruleNum){
    // printf("reduce %d\n", ruleNum);
    Sentence* newnode = mkNode(ruleNum);
    stkTop = stkTop - grammar_length[ruleNum];
    syn_analysis_state = stateStk[stkTop];
    return newnode;
}

void initSynaxDfa(){
    fin = 0;
    stkTop = 0;
    syn_analysis_state = 0;
    scanIdx = 1;
}

int getSynFromSentence(Sentence* stc){
    switch(stc->type){
        case ID:
            return ID;
        case KEYWORD:
            return stc->tval.keyIdx;
        case INUM:
            return INUM;
        case FNUM:
            return FNUM;
        case CNUM:
            return CNUM;
        case OPERATER:
            return stc->op;
        case SEPERATOR:
            return stc->tval.sepType;
        case SYNAXELE:
            return stc->tval.synVal;
    }
}

Sentence* synaxdfa(Sentence* stc, int canGo){
    int synid = getSynFromSentence(stc);
    switch(syn_analysis_state){
case 0:
    if(synid == INT){
        return shift(125, stc, canGo);
    }
    if(synid == FLOAT){
        return shift(122, stc, canGo);
    }
    if(synid == CHAR){
        return shift(119, stc, canGo);
    }
    if(synid == program){
        fin = 1;
        return NULL;
    }
    if(synid == deflist){
        return shift(126, stc, canGo);
    }
    if(synid == tp){
        return shift(11, stc, canGo);
    }
    return reduce(2);
case 1:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == LGB){
        return shift(9, stc, canGo);
    }
    if(synid == RETURN){
        return shift(28, stc, canGo);
    }
    if(synid == IF){
        return shift(110, stc, canGo);
    }
    if(synid == WHILE){
        return shift(108, stc, canGo);
    }
    if(synid == FOR){
        return shift(107, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == compstmt){
        shift(0, stc, canGo);
        return reduce(28);
    }
    if(synid == stmtlist){
        shift(0, stc, canGo);
        return reduce(26);
    }
    if(synid == exp){
        return shift(64, stc, canGo);
    }
    if(synid == stmt){
        return shift(1, stc, canGo);
    }
    return reduce(27);
case 2:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == LGB){
        return shift(9, stc, canGo);
    }
    if(synid == RETURN){
        return shift(28, stc, canGo);
    }
    if(synid == IF){
        return shift(110, stc, canGo);
    }
    if(synid == WHILE){
        return shift(108, stc, canGo);
    }
    if(synid == FOR){
        return shift(107, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == compstmt){
        shift(0, stc, canGo);
        return reduce(28);
    }
    if(synid == stmtlist){
        return shift(111, stc, canGo);
    }
    if(synid == exp){
        return shift(64, stc, canGo);
    }
    if(synid == stmt){
        return shift(1, stc, canGo);
    }
    return reduce(27);
case 3:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == LGB){
        return shift(9, stc, canGo);
    }
    if(synid == RETURN){
        return shift(28, stc, canGo);
    }
    if(synid == IF){
        return shift(110, stc, canGo);
    }
    if(synid == WHILE){
        return shift(108, stc, canGo);
    }
    if(synid == FOR){
        return shift(107, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == compstmt){
        shift(0, stc, canGo);
        return reduce(28);
    }
    if(synid == exp){
        return shift(64, stc, canGo);
    }
    if(synid == stmt){
        shift(0, stc, canGo);
        return reduce(34);
    }
    syntax_err(stc);
case 4:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == LGB){
        return shift(9, stc, canGo);
    }
    if(synid == RETURN){
        return shift(28, stc, canGo);
    }
    if(synid == IF){
        return shift(110, stc, canGo);
    }
    if(synid == WHILE){
        return shift(108, stc, canGo);
    }
    if(synid == FOR){
        return shift(107, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == compstmt){
        shift(0, stc, canGo);
        return reduce(28);
    }
    if(synid == exp){
        return shift(64, stc, canGo);
    }
    if(synid == stmt){
        shift(0, stc, canGo);
        return reduce(33);
    }
    syntax_err(stc);
case 5:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == LGB){
        return shift(9, stc, canGo);
    }
    if(synid == RETURN){
        return shift(28, stc, canGo);
    }
    if(synid == IF){
        return shift(110, stc, canGo);
    }
    if(synid == WHILE){
        return shift(108, stc, canGo);
    }
    if(synid == FOR){
        return shift(107, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == compstmt){
        shift(0, stc, canGo);
        return reduce(28);
    }
    if(synid == exp){
        return shift(64, stc, canGo);
    }
    if(synid == stmt){
        shift(0, stc, canGo);
        return reduce(32);
    }
    syntax_err(stc);
case 6:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == LGB){
        return shift(9, stc, canGo);
    }
    if(synid == RETURN){
        return shift(28, stc, canGo);
    }
    if(synid == IF){
        return shift(110, stc, canGo);
    }
    if(synid == WHILE){
        return shift(108, stc, canGo);
    }
    if(synid == FOR){
        return shift(107, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == compstmt){
        shift(0, stc, canGo);
        return reduce(28);
    }
    if(synid == exp){
        return shift(64, stc, canGo);
    }
    if(synid == stmt){
        return shift(109, stc, canGo);
    }
    syntax_err(stc);
case 7:
    if(synid == INT){
        return shift(125, stc, canGo);
    }
    if(synid == FLOAT){
        return shift(122, stc, canGo);
    }
    if(synid == CHAR){
        return shift(119, stc, canGo);
    }
    if(synid == RLB){
        shift(0, stc, canGo);
        return reduce(14);
    }
    if(synid == tp){
        return shift(113, stc, canGo);
    }
    if(synid == args){
        return shift(115, stc, canGo);
    }
    if(synid == param){
        return shift(114, stc, canGo);
    }
    syntax_err(stc);
case 8:
    if(synid == INT){
        return shift(125, stc, canGo);
    }
    if(synid == FLOAT){
        return shift(122, stc, canGo);
    }
    if(synid == CHAR){
        return shift(119, stc, canGo);
    }
    if(synid == tp){
        return shift(16, stc, canGo);
    }
    if(synid == innerdeflist){
        shift(0, stc, canGo);
        return reduce(19);
    }
    if(synid == innerdef){
        return shift(8, stc, canGo);
    }
    return reduce(20);
case 9:
    if(synid == INT){
        return shift(125, stc, canGo);
    }
    if(synid == FLOAT){
        return shift(122, stc, canGo);
    }
    if(synid == CHAR){
        return shift(119, stc, canGo);
    }
    if(synid == tp){
        return shift(16, stc, canGo);
    }
    if(synid == innerdeflist){
        return shift(2, stc, canGo);
    }
    if(synid == innerdef){
        return shift(8, stc, canGo);
    }
    return reduce(20);
case 10:
    if(synid == INT){
        return shift(125, stc, canGo);
    }
    if(synid == FLOAT){
        return shift(122, stc, canGo);
    }
    if(synid == CHAR){
        return shift(119, stc, canGo);
    }
    if(synid == tp){
        return shift(113, stc, canGo);
    }
    if(synid == args){
        shift(0, stc, canGo);
        return reduce(15);
    }
    if(synid == param){
        return shift(114, stc, canGo);
    }
    syntax_err(stc);
case 11:
    if(synid == ID){
        return shift(97, stc, canGo);
    }
    if(synid == def){
        return shift(14, stc, canGo);
    }
    if(synid == vallist){
        return shift(112, stc, canGo);
    }
    if(synid == fundef){
        return shift(55, stc, canGo);
    }
    syntax_err(stc);
case 12:
    if(synid == BIADD){
        return shift(103, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(102, stc, canGo);
    }
    if(synid == ID){
        return shift(88, stc, canGo);
    }
    if(synid == steplist){
        shift(0, stc, canGo);
        return reduce(40);
    }
    if(synid == step){
        return shift(104, stc, canGo);
    }
    syntax_err(stc);
case 13:
    if(synid == BIADD){
        return shift(103, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(102, stc, canGo);
    }
    if(synid == ID){
        return shift(88, stc, canGo);
    }
    if(synid == steplist){
        return shift(105, stc, canGo);
    }
    if(synid == step){
        return shift(104, stc, canGo);
    }
    syntax_err(stc);
case 14:
    if(synid == INT){
        return shift(125, stc, canGo);
    }
    if(synid == FLOAT){
        return shift(122, stc, canGo);
    }
    if(synid == CHAR){
        return shift(119, stc, canGo);
    }
    if(synid == deflist){
        shift(0, stc, canGo);
        return reduce(1);
    }
    if(synid == tp){
        return shift(11, stc, canGo);
    }
    return reduce(2);
case 15:
    if(synid == ID){
        return shift(98, stc, canGo);
    }
    if(synid == innerdeclist){
        shift(0, stc, canGo);
        return reduce(22);
    }
    if(synid == innerdec){
        return shift(99, stc, canGo);
    }
    syntax_err(stc);
case 16:
    if(synid == ID){
        return shift(98, stc, canGo);
    }
    if(synid == innerdeclist){
        return shift(100, stc, canGo);
    }
    if(synid == innerdec){
        return shift(99, stc, canGo);
    }
    syntax_err(stc);
case 17:
    if(synid == ID){
        return shift(87, stc, canGo);
    }
    if(synid == initlist){
        shift(0, stc, canGo);
        return reduce(35);
    }
    if(synid == init){
        return shift(101, stc, canGo);
    }
    syntax_err(stc);
case 18:
    if(synid == ID){
        return shift(87, stc, canGo);
    }
    if(synid == initlist){
        return shift(106, stc, canGo);
    }
    if(synid == init){
        return shift(101, stc, canGo);
    }
    syntax_err(stc);
case 19:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == RLB){
        shift(0, stc, canGo);
        return reduce(80);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(61, stc, canGo);
    }
    syntax_err(stc);
case 20:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(65, stc, canGo);
    }
    syntax_err(stc);
case 21:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(66, stc, canGo);
    }
    syntax_err(stc);
case 22:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(67, stc, canGo);
    }
    syntax_err(stc);
case 23:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(68, stc, canGo);
    }
    syntax_err(stc);
case 24:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(69, stc, canGo);
    }
    syntax_err(stc);
case 25:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(57, stc, canGo);
    }
    syntax_err(stc);
case 26:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(58, stc, canGo);
    }
    syntax_err(stc);
case 27:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(59, stc, canGo);
    }
    syntax_err(stc);
case 28:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(60, stc, canGo);
    }
    syntax_err(stc);
case 29:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(89, stc, canGo);
    }
    syntax_err(stc);
case 30:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(90, stc, canGo);
    }
    syntax_err(stc);
case 31:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(91, stc, canGo);
    }
    syntax_err(stc);
case 32:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(62, stc, canGo);
    }
    syntax_err(stc);
case 33:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(63, stc, canGo);
    }
    syntax_err(stc);
case 34:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(70, stc, canGo);
    }
    syntax_err(stc);
case 35:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(71, stc, canGo);
    }
    syntax_err(stc);
case 36:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(72, stc, canGo);
    }
    syntax_err(stc);
case 37:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(73, stc, canGo);
    }
    syntax_err(stc);
case 38:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(92, stc, canGo);
    }
    syntax_err(stc);
case 39:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(77, stc, canGo);
    }
    syntax_err(stc);
case 40:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(76, stc, canGo);
    }
    syntax_err(stc);
case 41:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(78, stc, canGo);
    }
    syntax_err(stc);
case 42:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(93, stc, canGo);
    }
    syntax_err(stc);
case 43:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(94, stc, canGo);
    }
    syntax_err(stc);
case 44:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(95, stc, canGo);
    }
    syntax_err(stc);
case 45:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(85, stc, canGo);
    }
    syntax_err(stc);
case 46:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(86, stc, canGo);
    }
    syntax_err(stc);
case 47:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(79, stc, canGo);
    }
    syntax_err(stc);
case 48:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(80, stc, canGo);
    }
    syntax_err(stc);
case 49:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(81, stc, canGo);
    }
    syntax_err(stc);
case 50:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(82, stc, canGo);
    }
    syntax_err(stc);
case 51:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(83, stc, canGo);
    }
    syntax_err(stc);
case 52:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(84, stc, canGo);
    }
    syntax_err(stc);
case 53:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(74, stc, canGo);
    }
    syntax_err(stc);
case 54:
    if(synid == BIADD){
        return shift(30, stc, canGo);
    }
    if(synid == BIMINUS){
        return shift(29, stc, canGo);
    }
    if(synid == NOT){
        return shift(31, stc, canGo);
    }
    if(synid == INUM){
        shift(0, stc, canGo);
        return reduce(60);
    }
    if(synid == ID){
        return shift(96, stc, canGo);
    }
    if(synid == LLB){
        return shift(32, stc, canGo);
    }
    if(synid == FNUM){
        shift(0, stc, canGo);
        return reduce(61);
    }
    if(synid == CNUM){
        shift(0, stc, canGo);
        return reduce(78);
    }
    if(synid == exp){
        return shift(75, stc, canGo);
    }
    syntax_err(stc);
case 55:
    if(synid == LGB){
        return shift(9, stc, canGo);
    }
    if(synid == compstmt){
        shift(0, stc, canGo);
        return reduce(12);
    }
    syntax_err(stc);
case 56:
    if(synid == ID){
        return shift(116, stc, canGo);
    }
    if(synid == vallist){
        shift(0, stc, canGo);
        return reduce(9);
    }
    syntax_err(stc);
case 57:
    if(synid == COMMA){
        return shift(37, stc, canGo);
    }
    if(synid == SETVAL){
        return shift(36, stc, canGo);
    }
    if(synid == ADDEQ){
        return shift(35, stc, canGo);
    }
    if(synid == MINUSEQ){
        return shift(34, stc, canGo);
    }
    if(synid == BIOR){
        return shift(53, stc, canGo);
    }
    if(synid == BIAND){
        return shift(54, stc, canGo);
    }
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    if(synid == SEMICOLON){
        return shift(13, stc, canGo);
    }
    syntax_err(stc);
case 58:
    if(synid == COMMA){
        return shift(37, stc, canGo);
    }
    if(synid == SETVAL){
        return shift(36, stc, canGo);
    }
    if(synid == ADDEQ){
        return shift(35, stc, canGo);
    }
    if(synid == MINUSEQ){
        return shift(34, stc, canGo);
    }
    if(synid == BIOR){
        return shift(53, stc, canGo);
    }
    if(synid == BIAND){
        return shift(54, stc, canGo);
    }
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    if(synid == RLB){
        return shift(4, stc, canGo);
    }
    syntax_err(stc);
case 59:
    if(synid == COMMA){
        return shift(37, stc, canGo);
    }
    if(synid == SETVAL){
        return shift(36, stc, canGo);
    }
    if(synid == ADDEQ){
        return shift(35, stc, canGo);
    }
    if(synid == MINUSEQ){
        return shift(34, stc, canGo);
    }
    if(synid == BIOR){
        return shift(53, stc, canGo);
    }
    if(synid == BIAND){
        return shift(54, stc, canGo);
    }
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    if(synid == RLB){
        return shift(6, stc, canGo);
    }
    syntax_err(stc);
case 60:
    if(synid == COMMA){
        return shift(37, stc, canGo);
    }
    if(synid == SETVAL){
        return shift(36, stc, canGo);
    }
    if(synid == ADDEQ){
        return shift(35, stc, canGo);
    }
    if(synid == MINUSEQ){
        return shift(34, stc, canGo);
    }
    if(synid == BIOR){
        return shift(53, stc, canGo);
    }
    if(synid == BIAND){
        return shift(54, stc, canGo);
    }
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    if(synid == SEMICOLON){
        shift(0, stc, canGo);
        return reduce(30);
    }
    syntax_err(stc);
case 61:
    if(synid == COMMA){
        return shift(37, stc, canGo);
    }
    if(synid == SETVAL){
        return shift(36, stc, canGo);
    }
    if(synid == ADDEQ){
        return shift(35, stc, canGo);
    }
    if(synid == MINUSEQ){
        return shift(34, stc, canGo);
    }
    if(synid == BIOR){
        return shift(53, stc, canGo);
    }
    if(synid == BIAND){
        return shift(54, stc, canGo);
    }
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    if(synid == RLB){
        shift(0, stc, canGo);
        return reduce(79);
    }
    syntax_err(stc);
case 62:
    if(synid == COMMA){
        return shift(37, stc, canGo);
    }
    if(synid == SETVAL){
        return shift(36, stc, canGo);
    }
    if(synid == ADDEQ){
        return shift(35, stc, canGo);
    }
    if(synid == MINUSEQ){
        return shift(34, stc, canGo);
    }
    if(synid == BIOR){
        return shift(53, stc, canGo);
    }
    if(synid == BIAND){
        return shift(54, stc, canGo);
    }
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    if(synid == RLB){
        shift(0, stc, canGo);
        return reduce(63);
    }
    syntax_err(stc);
case 63:
    if(synid == COMMA){
        return shift(37, stc, canGo);
    }
    if(synid == SETVAL){
        return shift(36, stc, canGo);
    }
    if(synid == ADDEQ){
        return shift(35, stc, canGo);
    }
    if(synid == MINUSEQ){
        return shift(34, stc, canGo);
    }
    if(synid == BIOR){
        return shift(53, stc, canGo);
    }
    if(synid == BIAND){
        return shift(54, stc, canGo);
    }
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    if(synid == RMB){
        shift(0, stc, canGo);
        return reduce(70);
    }
    syntax_err(stc);
case 64:
    if(synid == COMMA){
        return shift(37, stc, canGo);
    }
    if(synid == SETVAL){
        return shift(36, stc, canGo);
    }
    if(synid == ADDEQ){
        return shift(35, stc, canGo);
    }
    if(synid == MINUSEQ){
        return shift(34, stc, canGo);
    }
    if(synid == BIOR){
        return shift(53, stc, canGo);
    }
    if(synid == BIAND){
        return shift(54, stc, canGo);
    }
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    if(synid == SEMICOLON){
        shift(0, stc, canGo);
        return reduce(29);
    }
    syntax_err(stc);
case 65:
    if(synid == SETVAL){
        return shift(36, stc, canGo);
    }
    if(synid == ADDEQ){
        return shift(35, stc, canGo);
    }
    if(synid == MINUSEQ){
        return shift(34, stc, canGo);
    }
    if(synid == BIOR){
        return shift(53, stc, canGo);
    }
    if(synid == BIAND){
        return shift(54, stc, canGo);
    }
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(24);
case 66:
    if(synid == SETVAL){
        return shift(36, stc, canGo);
    }
    if(synid == ADDEQ){
        return shift(35, stc, canGo);
    }
    if(synid == MINUSEQ){
        return shift(34, stc, canGo);
    }
    if(synid == BIOR){
        return shift(53, stc, canGo);
    }
    if(synid == BIAND){
        return shift(54, stc, canGo);
    }
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(39);
case 67:
    if(synid == SETVAL){
        return shift(36, stc, canGo);
    }
    if(synid == ADDEQ){
        return shift(35, stc, canGo);
    }
    if(synid == MINUSEQ){
        return shift(34, stc, canGo);
    }
    if(synid == BIOR){
        return shift(53, stc, canGo);
    }
    if(synid == BIAND){
        return shift(54, stc, canGo);
    }
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(38);
case 68:
    if(synid == SETVAL){
        return shift(36, stc, canGo);
    }
    if(synid == ADDEQ){
        return shift(35, stc, canGo);
    }
    if(synid == MINUSEQ){
        return shift(34, stc, canGo);
    }
    if(synid == BIOR){
        return shift(53, stc, canGo);
    }
    if(synid == BIAND){
        return shift(54, stc, canGo);
    }
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(37);
case 69:
    if(synid == SETVAL){
        return shift(36, stc, canGo);
    }
    if(synid == ADDEQ){
        return shift(35, stc, canGo);
    }
    if(synid == MINUSEQ){
        return shift(34, stc, canGo);
    }
    if(synid == BIOR){
        return shift(53, stc, canGo);
    }
    if(synid == BIAND){
        return shift(54, stc, canGo);
    }
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(46);
case 70:
    if(synid == SETVAL){
        return shift(36, stc, canGo);
    }
    if(synid == ADDEQ){
        return shift(35, stc, canGo);
    }
    if(synid == MINUSEQ){
        return shift(34, stc, canGo);
    }
    if(synid == BIOR){
        return shift(53, stc, canGo);
    }
    if(synid == BIAND){
        return shift(54, stc, canGo);
    }
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(73);
case 71:
    if(synid == SETVAL){
        return shift(36, stc, canGo);
    }
    if(synid == ADDEQ){
        return shift(35, stc, canGo);
    }
    if(synid == MINUSEQ){
        return shift(34, stc, canGo);
    }
    if(synid == BIOR){
        return shift(53, stc, canGo);
    }
    if(synid == BIAND){
        return shift(54, stc, canGo);
    }
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(72);
case 72:
    if(synid == SETVAL){
        return shift(36, stc, canGo);
    }
    if(synid == ADDEQ){
        return shift(35, stc, canGo);
    }
    if(synid == MINUSEQ){
        return shift(34, stc, canGo);
    }
    if(synid == BIOR){
        return shift(53, stc, canGo);
    }
    if(synid == BIAND){
        return shift(54, stc, canGo);
    }
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(71);
case 73:
    if(synid == SETVAL){
        return shift(36, stc, canGo);
    }
    if(synid == ADDEQ){
        return shift(35, stc, canGo);
    }
    if(synid == MINUSEQ){
        return shift(34, stc, canGo);
    }
    if(synid == BIOR){
        return shift(53, stc, canGo);
    }
    if(synid == BIAND){
        return shift(54, stc, canGo);
    }
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(69);
case 74:
    if(synid == BIAND){
        return shift(54, stc, canGo);
    }
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(48);
case 75:
    if(synid == OR){
        return shift(40, stc, canGo);
    }
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(47);
case 76:
    if(synid == XOR){
        return shift(39, stc, canGo);
    }
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(66);
case 77:
    if(synid == AND){
        return shift(41, stc, canGo);
    }
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(67);
case 78:
    if(synid == EQ){
        return shift(48, stc, canGo);
    }
    if(synid == NOTEQ){
        return shift(47, stc, canGo);
    }
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(65);
case 79:
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(54);
case 80:
    if(synid == LSS){
        return shift(52, stc, canGo);
    }
    if(synid == LSSEQ){
        return shift(51, stc, canGo);
    }
    if(synid == GRT){
        return shift(50, stc, canGo);
    }
    if(synid == GRTEQ){
        return shift(49, stc, canGo);
    }
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(53);
case 81:
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(52);
case 82:
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(51);
case 83:
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(50);
case 84:
    if(synid == ADD){
        return shift(46, stc, canGo);
    }
    if(synid == MINUS){
        return shift(45, stc, canGo);
    }
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(49);
case 85:
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(56);
case 86:
    if(synid == DIV){
        return shift(43, stc, canGo);
    }
    if(synid == MOD){
        return shift(42, stc, canGo);
    }
    if(synid == MULTI){
        return shift(44, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(55);
case 87:
    if(synid == SETVAL){
        return shift(23, stc, canGo);
    }
    if(synid == ADDEQ){
        return shift(22, stc, canGo);
    }
    if(synid == MINUSEQ){
        return shift(21, stc, canGo);
    }
    syntax_err(stc);
case 88:
    if(synid == SETVAL){
        return shift(24, stc, canGo);
    }
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(43);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(45);
    }
    syntax_err(stc);
case 89:
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(77);
case 90:
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(76);
case 91:
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(64);
case 92:
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(68);
case 93:
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(59);
case 94:
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(58);
case 95:
    if(synid == BIADD){
        shift(0, stc, canGo);
        return reduce(74);
    }
    if(synid == BIMINUS){
        shift(0, stc, canGo);
        return reduce(75);
    }
    if(synid == BITNOT){
        return shift(38, stc, canGo);
    }
    return reduce(57);
case 96:
    if(synid == LMB){
        return shift(33, stc, canGo);
    }
    if(synid == LLB){
        return shift(19, stc, canGo);
    }
    return reduce(62);
case 97:
    if(synid == COMMA){
        return shift(56, stc, canGo);
    }
    if(synid == LLB){
        return shift(7, stc, canGo);
    }
    return reduce(10);
case 98:
    if(synid == SETVAL){
        return shift(20, stc, canGo);
    }
    return reduce(25);
case 99:
    if(synid == COMMA){
        return shift(15, stc, canGo);
    }
    return reduce(23);
case 100:
    if(synid == SEMICOLON){
        shift(0, stc, canGo);
        return reduce(21);
    }
    syntax_err(stc);
case 101:
    if(synid == COMMA){
        return shift(17, stc, canGo);
    }
    return reduce(36);
case 102:
    if(synid == ID){
        shift(0, stc, canGo);
        return reduce(44);
    }
    syntax_err(stc);
case 103:
    if(synid == ID){
        shift(0, stc, canGo);
        return reduce(42);
    }
    syntax_err(stc);
case 104:
    if(synid == COMMA){
        return shift(12, stc, canGo);
    }
    return reduce(41);
case 105:
    if(synid == RLB){
        return shift(3, stc, canGo);
    }
    syntax_err(stc);
case 106:
    if(synid == SEMICOLON){
        return shift(25, stc, canGo);
    }
    syntax_err(stc);
case 107:
    if(synid == LLB){
        return shift(18, stc, canGo);
    }
    syntax_err(stc);
case 108:
    if(synid == LLB){
        return shift(26, stc, canGo);
    }
    syntax_err(stc);
case 109:
    if(synid == ELSE){
        return shift(5, stc, canGo);
    }
    return reduce(31);
case 110:
    if(synid == LLB){
        return shift(27, stc, canGo);
    }
    syntax_err(stc);
case 111:
    if(synid == RGB){
        shift(0, stc, canGo);
        return reduce(18);
    }
    syntax_err(stc);
case 112:
    if(synid == SEMICOLON){
        shift(0, stc, canGo);
        return reduce(11);
    }
    syntax_err(stc);
case 113:
    if(synid == ID){
        shift(0, stc, canGo);
        return reduce(17);
    }
    syntax_err(stc);
case 114:
    if(synid == COMMA){
        return shift(10, stc, canGo);
    }
    return reduce(16);
case 115:
    if(synid == RLB){
        shift(0, stc, canGo);
        return reduce(13);
    }
    syntax_err(stc);
case 116:
    if(synid == COMMA){
        return shift(56, stc, canGo);
    }
    return reduce(10);
case 117:
    if(synid == RMB){
        shift(0, stc, canGo);
        return reduce(8);
    }
    syntax_err(stc);
case 118:
    if(synid == INUM){
        return shift(117, stc, canGo);
    }
    syntax_err(stc);
case 119:
    if(synid == LMB){
        return shift(118, stc, canGo);
    }
    return reduce(5);
case 120:
    if(synid == RMB){
        shift(0, stc, canGo);
        return reduce(7);
    }
    syntax_err(stc);
case 121:
    if(synid == INUM){
        return shift(120, stc, canGo);
    }
    syntax_err(stc);
case 122:
    if(synid == LMB){
        return shift(121, stc, canGo);
    }
    return reduce(4);
case 123:
    if(synid == RMB){
        shift(0, stc, canGo);
        return reduce(6);
    }
    syntax_err(stc);
case 124:
    if(synid == INUM){
        return shift(123, stc, canGo);
    }
    syntax_err(stc);
case 125:
    if(synid == LMB){
        return shift(124, stc, canGo);
    }
    return reduce(3);
case 126:
    if(synid == END){
        return reduce(0);
    }
    syntax_err(stc);
    }
}

void printWordInfo(Word w){
    printf("(%s):", words_type[w.type - INUM]);
    switch(w.type){
        case ID:
            printf("[%s]\n", w.tval.name);
            break;
        case KEYWORD:
            printf("[%s]\n", keywords[w.tval.keyIdx]);
            break;
        case INUM:
            printf("[%d]\n", w.tval.ivalue);
            break;
        case FNUM:
            printf("[%f]\n", w.tval.fvalue);
            break;
        case CNUM:
            printf("[\'%c\']\n", w.tval.cvalue);
            break;
        case OPERATER:
            printf("[%s, %d]\n", op_type[w.tval.opType - UNARY_OP], w.op);
            break;
        case SEPERATOR:
            printf("[%s]\n", sep_type[w.tval.sepType - COMMA]);
            break;
        case SYNAXELE:
            printf("[%s]\n", synax_ele[w.tval.synVal - program]);
            break;
        default:
            printf("\n");
            break;
    }
}

void lex_part(FILE* file){
    initTrie();
    // char cc[100] = "while Rick hits Morty break dollardollar 10 for 1024 plumbus 1.11  == 2.22 ++ ";
    // char cc[100] = "int i, j; int fun(int a, int b){int m;if(a > b) m = a; else m=b;return m;} float a,b;";
    // char cc[100] = "int main(){if(a > 0 && b > 0) m = 0;} ";
    char c;
    int colomn = 1, line = 1;
    while((c = fgetc(file)) != EOF){
        if(c == '\n'){
            colomn = 1;
            line++;
        }
        if(lex(c, line, colomn))
            lex(c, line, colomn);
        colomn++;
    }
    lex(' ', line, colomn);
    for(int i = 1; i <= wordsSize; i++){
        printWordInfo(words[i]);
    }
}

void debugsyn(){
    for(int i = 1; i <= stkTop; i++){
        printWordInfo(*(stcStk[i]));
    }
}

void syn_part(){
    initSynaxDfa();
    words[++wordsSize].type = SYNAXELE;
    words[wordsSize].tval.synVal = END;
    words[wordsSize].line = words[wordsSize - 1].line;
    for(; !fin && scanIdx <= wordsSize;){
        Sentence* res = synaxdfa(words + scanIdx, 1);
        // printf("%d state:%d:\n", scanIdx, syn_analysis_state);
        // debugsyn();
        // printf("\n");
        if(res){
            // printf("%d:(reduce) state:%d\n", scanIdx, syn_analysis_state);
            while((res = synaxdfa(res, 0)));
            // debugsyn();
            // printf("\n");
        }
    }
}

void lvprint(int lv){
    for(int i = 1; i <= lv; i++)
        printf("    ");
}

void showNodeInfo(Sentence* node, int lv){
    lvprint(lv);
    int nodetype = node->type;
    if(nodetype == SYNAXELE)
        printf("%s:\n", synax_ele[node->tval.synVal - program]);
    else{
        switch(nodetype){
        case ID:
            printf("ID:  [%s]\n", node->tval.name);
            break;
        case KEYWORD:
            printf("\'%s\':\n", keywords[node->tval.keyIdx]);
            break;
        case INUM:
            printf("INTEGER:  [%d]\n", node->tval.ivalue);
            break;
        case FNUM:
            printf("FLOAT:  [%f]\n", node->tval.fvalue);
            break;
        case CNUM:
            printf("CHAR:  [\'%c\']\n", node->tval.cvalue);
            break;
        case OPERATER:
            printf("OPERATER:  (%s, %s)\n", op_type[node->tval.opType - UNARY_OP], op_detail_type[node->op - BIADD]);
            break;
        case SEPERATOR:
            printf("[%s]\n", sep_type[node->tval.sepType - COMMA]);
            break;
        }
    }
}

void showAST(Sentence* node, int lv){
    showNodeInfo(node, lv);
    for(int i = 1; ; i++){
        if(node->next[i] == NULL)
            break;
        showAST(node->next[i], lv + 1);
    }
}

// int main()
// {
//     FILE* f = fopen("./Test/SYNTAX_TEST1.txt", "r");
//     freopen("./TestRes/TEST1_RES.txt", "w", stdout);
//     printf("lex:\n");
//     lex_part(f);
//     printf("\n\nsyntax:\n");
//     syn_part();
//     // printf("%d\n", fin);
//     showAST(pgm, 0);
//     fclose(f);
//     return 0;
// }