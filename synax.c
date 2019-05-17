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
extern Word words[MAXWORDS];
extern int wordsSize;

const char* synax_ele[] = {"program", "deflist", "tp", "def", "vallist", "fundef",
                              "stmtlist", "args", "param", "stmt", "exp", "initlist", "init",
                              "steplist", "step", "stmt1"};
const int grammar_length[] = {1, 3, 0, 1, 1, 4, 4, 3, 1, 2, 4, 4, 3, 3, 1, 2, 2, 0, 2, 3, 3, 3, 5,
                                  7, 5, 9, 3, 1, 3, 3, 3, 3, 1, 2, 2, 2, 2, 3, 7, 5, 9, 3, 3, 3, 3, 3, 3,
                                  3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 3, 2, 3, 3, 3, 3, 3, 4, 3};
const int grammar_rule[] = {program, deflist, deflist, tp, tp, tp, tp, vallist, vallist, def, def, fundef, fundef,
                          args, args, param, stmtlist, stmtlist, stmt, stmt, stmt, stmt, stmt, stmt, stmt, stmt,
                          initlist, initlist, init, init, init, steplist, steplist, step, step, step, step, step, stmt1, stmt1, stmt1,
                          exp, exp, exp, exp, exp, exp, exp, exp, exp, exp, exp, exp, 
                          exp, exp, exp, exp, exp, exp, exp, exp, exp, exp, exp, exp, exp};

Sentence* pgm;

Sentence* mkNode(int ruleNum){
    Sentence* newnode = (Sentence*) malloc(sizeof(Sentence));
    newnode->type = SYNAXELE;
    newnode->tval.synVal = grammar_rule[ruleNum];
    int startpos = stkTop - grammar_length[ruleNum] + 1;
    int endpos = stkTop;
    pgm = newnode;
    for(; startpos <= endpos; startpos++)
        newnode->next[startpos] = stcStk[startpos];
    return newnode;
}

Sentence* shift(int nextState, Sentence* stc){
    printf("shift %d\n", nextState);
    stateStk[++stkTop] = nextState;
    stcStk[stkTop] = stc;
    syn_analysis_state = nextState;
    return NULL;
}

Sentence* reduce(int ruleNum){
    printf("reduce %d\n", ruleNum);
    Sentence* newnode = mkNode(ruleNum);
    stkTop = stkTop - grammar_length[ruleNum];
    syn_analysis_state = stateStk[stkTop];
    return newnode;
}

void initSynaxDfa(){
    fin = 0;
    stkTop = 0;
    syn_analysis_state = 0;
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
        case OPERATER:
            return stc->op;
        case SEPERATOR:
            return stc->tval.sepType;
        case SYNAXELE:
            return stc->tval.synVal;
    }
}

Sentence* synaxdfa(Sentence* stc){
    int synid = getSynFromSentence(stc);
    switch(syn_analysis_state){
        case 0:
    if(synid == END){
        return reduce(2);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == program){
        fin = 1;
        return NULL;
    }
    if(synid == deflist){
        return shift(1, stc);
    }
    if(synid == tp){
        return shift(2, stc);
    }
    exit(0);
case 1:
    if(synid == END){
        return reduce(0);
    }
    exit(0);
case 2:
    if(synid == ID){
        return shift(13, stc);
    }
    if(synid == def){
        return shift(3, stc);
    }
    if(synid == vallist){
        return shift(26, stc);
    }
    if(synid == fundef){
        return shift(28, stc);
    }
    exit(0);
case 3:
    if(synid == END){
        return reduce(2);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == deflist){
        return shift(4, stc);
    }
    if(synid == tp){
        return shift(2, stc);
    }
    exit(0);
case 4:
    if(synid == END){
        return reduce(1);
    }
    exit(0);
case 5:
    if(synid == LMB){
        return shift(6, stc);
    }
    if(synid == ID){
        return reduce(3);
    }
    exit(0);
case 6:
    if(synid == INUM){
        return shift(7, stc);
    }
    exit(0);
case 7:
    if(synid == RMB){
        return shift(8, stc);
    }
    exit(0);
case 8:
    if(synid == ID){
        return reduce(5);
    }
    exit(0);
case 9:
    if(synid == LMB){
        return shift(10, stc);
    }
    if(synid == ID){
        return reduce(4);
    }
    exit(0);
case 10:
    if(synid == INUM){
        return shift(11, stc);
    }
    exit(0);
case 11:
    if(synid == RMB){
        return shift(12, stc);
    }
    exit(0);
case 12:
    if(synid == ID){
        return reduce(6);
    }
    exit(0);
case 13:
    if(synid == COMMA){
        return shift(14, stc);
    }
    if(synid == SEMICOLON){
        return reduce(8);
    }
    if(synid == LLB){
        return shift(17, stc);
    }
    exit(0);
case 14:
    if(synid == ID){
        return shift(15, stc);
    }
    if(synid == vallist){
        return shift(16, stc);
    }
    exit(0);
case 15:
    if(synid == COMMA){
        return shift(14, stc);
    }
    if(synid == SEMICOLON){
        return reduce(8);
    }
    exit(0);
case 16:
    if(synid == SEMICOLON){
        return reduce(7);
    }
    exit(0);
case 17:
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == RLB){
        return shift(20, stc);
    }
    if(synid == tp){
        return shift(24, stc);
    }
    if(synid == args){
        return shift(18, stc);
    }
    if(synid == param){
        return shift(21, stc);
    }
    exit(0);
case 18:
    if(synid == RLB){
        return shift(19, stc);
    }
    exit(0);
case 19:
    if(synid == LGB){
        return reduce(11);
    }
    exit(0);
case 20:
    if(synid == LGB){
        return reduce(12);
    }
    exit(0);
case 21:
    if(synid == COMMA){
        return shift(22, stc);
    }
    if(synid == RLB){
        return reduce(14);
    }
    exit(0);
case 22:
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == tp){
        return shift(24, stc);
    }
    if(synid == args){
        return shift(23, stc);
    }
    if(synid == param){
        return shift(21, stc);
    }
    exit(0);
case 23:
    if(synid == RLB){
        return reduce(13);
    }
    exit(0);
case 24:
    if(synid == ID){
        return shift(25, stc);
    }
    exit(0);
case 25:
    return reduce(15);
case 26:
    if(synid == SEMICOLON){
        return shift(27, stc);
    }
    exit(0);
case 27:
    return reduce(9);
case 28:
    if(synid == LGB){
        return shift(29, stc);
    }
    exit(0);
case 29:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LGB){
        return shift(91, stc);
    }
    if(synid == RGB){
        return reduce(17);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == RETURN){
        return shift(88, stc);
    }
    if(synid == IF){
        return shift(94, stc);
    }
    if(synid == WHILE){
        return shift(147, stc);
    }
    if(synid == FOR){
        return shift(151, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == tp){
        return shift(85, stc);
    }
    if(synid == stmtlist){
        return shift(30, stc);
    }
    if(synid == stmt){
        return shift(32, stc);
    }
    if(synid == exp){
        return shift(34, stc);
    }
    exit(0);
case 30:
    if(synid == RGB){
        return shift(31, stc);
    }
    exit(0);
case 31:
    return reduce(10);
case 32:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LGB){
        return shift(91, stc);
    }
    if(synid == RGB){
        return reduce(17);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == RETURN){
        return shift(88, stc);
    }
    if(synid == IF){
        return shift(94, stc);
    }
    if(synid == WHILE){
        return shift(147, stc);
    }
    if(synid == FOR){
        return shift(151, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == tp){
        return shift(85, stc);
    }
    if(synid == stmtlist){
        return shift(33, stc);
    }
    if(synid == stmt){
        return shift(32, stc);
    }
    if(synid == exp){
        return shift(34, stc);
    }
    exit(0);
case 33:
    if(synid == RGB){
        return reduce(16);
    }
    exit(0);
case 34:
    if(synid == COMMA){
        return shift(70, stc);
    }
    if(synid == SETVAL){
        return shift(72, stc);
    }
    if(synid == BIOR){
        return shift(38, stc);
    }
    if(synid == BIAND){
        return shift(36, stc);
    }
    if(synid == OR){
        return shift(64, stc);
    }
    if(synid == XOR){
        return shift(66, stc);
    }
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    if(synid == SEMICOLON){
        return shift(35, stc);
    }
    exit(0);
case 35:
    return reduce(18);
case 36:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(37, stc);
    }
    exit(0);
case 37:
    if(synid == OR){
        return shift(64, stc);
    }
    if(synid == XOR){
        return shift(66, stc);
    }
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(41);
case 38:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(39, stc);
    }
    exit(0);
case 39:
    if(synid == BIAND){
        return shift(36, stc);
    }
    if(synid == OR){
        return shift(64, stc);
    }
    if(synid == XOR){
        return shift(66, stc);
    }
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(42);
case 40:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(41, stc);
    }
    exit(0);
case 41:
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(43);
case 42:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(43, stc);
    }
    exit(0);
case 43:
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(44);
case 44:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(45, stc);
    }
    exit(0);
case 45:
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(45);
case 46:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(47, stc);
    }
    exit(0);
case 47:
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(46);
case 48:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(49, stc);
    }
    exit(0);
case 49:
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(47);
case 50:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(51, stc);
    }
    exit(0);
case 51:
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(48);
case 52:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(53, stc);
    }
    exit(0);
case 53:
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(49);
case 54:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(55, stc);
    }
    exit(0);
case 55:
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(50);
case 56:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(57, stc);
    }
    exit(0);
case 57:
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(51);
case 58:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(59, stc);
    }
    exit(0);
case 59:
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(52);
case 60:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(61, stc);
    }
    exit(0);
case 61:
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(53);
case 62:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(63, stc);
    }
    exit(0);
case 63:
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(59);
case 64:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(65, stc);
    }
    exit(0);
case 65:
    if(synid == XOR){
        return shift(66, stc);
    }
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(60);
case 66:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(67, stc);
    }
    exit(0);
case 67:
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(61);
case 68:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(69, stc);
    }
    exit(0);
case 69:
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(62);
case 70:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(71, stc);
    }
    exit(0);
case 71:
    if(synid == SETVAL){
        return shift(72, stc);
    }
    if(synid == BIOR){
        return shift(38, stc);
    }
    if(synid == BIAND){
        return shift(36, stc);
    }
    if(synid == OR){
        return shift(64, stc);
    }
    if(synid == XOR){
        return shift(66, stc);
    }
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(63);
case 72:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(73, stc);
    }
    exit(0);
case 73:
    if(synid == SETVAL){
        return shift(72, stc);
    }
    if(synid == BIOR){
        return shift(38, stc);
    }
    if(synid == BIAND){
        return shift(36, stc);
    }
    if(synid == OR){
        return shift(64, stc);
    }
    if(synid == XOR){
        return shift(66, stc);
    }
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(65);
case 74:
    return reduce(54);
case 75:
    return reduce(55);
case 76:
    if(synid == LMB){
        return shift(77, stc);
    }
    return reduce(56);
case 77:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(78, stc);
    }
    exit(0);
case 78:
    if(synid == COMMA){
        return shift(70, stc);
    }
    if(synid == SETVAL){
        return shift(72, stc);
    }
    if(synid == BIOR){
        return shift(38, stc);
    }
    if(synid == BIAND){
        return shift(36, stc);
    }
    if(synid == OR){
        return shift(64, stc);
    }
    if(synid == XOR){
        return shift(66, stc);
    }
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    if(synid == RMB){
        return shift(79, stc);
    }
    exit(0);
case 79:
    return reduce(64);
case 80:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(81, stc);
    }
    exit(0);
case 81:
    if(synid == COMMA){
        return shift(70, stc);
    }
    if(synid == SETVAL){
        return shift(72, stc);
    }
    if(synid == BIOR){
        return shift(38, stc);
    }
    if(synid == BIAND){
        return shift(36, stc);
    }
    if(synid == OR){
        return shift(64, stc);
    }
    if(synid == XOR){
        return shift(66, stc);
    }
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    if(synid == RLB){
        return shift(82, stc);
    }
    exit(0);
case 82:
    return reduce(57);
case 83:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(84, stc);
    }
    exit(0);
case 84:
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(58);
case 85:
    if(synid == ID){
        return shift(15, stc);
    }
    if(synid == vallist){
        return shift(86, stc);
    }
    exit(0);
case 86:
    if(synid == SEMICOLON){
        return shift(87, stc);
    }
    exit(0);
case 87:
    return reduce(19);
case 88:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(89, stc);
    }
    exit(0);
case 89:
    if(synid == COMMA){
        return shift(70, stc);
    }
    if(synid == SETVAL){
        return shift(72, stc);
    }
    if(synid == BIOR){
        return shift(38, stc);
    }
    if(synid == BIAND){
        return shift(36, stc);
    }
    if(synid == OR){
        return shift(64, stc);
    }
    if(synid == XOR){
        return shift(66, stc);
    }
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    if(synid == SEMICOLON){
        return shift(90, stc);
    }
    exit(0);
case 90:
    return reduce(20);
case 91:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LGB){
        return shift(91, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == RETURN){
        return shift(88, stc);
    }
    if(synid == IF){
        return shift(94, stc);
    }
    if(synid == WHILE){
        return shift(147, stc);
    }
    if(synid == FOR){
        return shift(151, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == tp){
        return shift(85, stc);
    }
    if(synid == stmt){
        return shift(92, stc);
    }
    if(synid == exp){
        return shift(34, stc);
    }
    exit(0);
case 92:
    if(synid == RGB){
        return shift(93, stc);
    }
    exit(0);
case 93:
    return reduce(21);
case 94:
    if(synid == LLB){
        return shift(95, stc);
    }
    exit(0);
case 95:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(96, stc);
    }
    exit(0);
case 96:
    if(synid == COMMA){
        return shift(70, stc);
    }
    if(synid == SETVAL){
        return shift(72, stc);
    }
    if(synid == BIOR){
        return shift(38, stc);
    }
    if(synid == BIAND){
        return shift(36, stc);
    }
    if(synid == OR){
        return shift(64, stc);
    }
    if(synid == XOR){
        return shift(66, stc);
    }
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    if(synid == RLB){
        return shift(97, stc);
    }
    exit(0);
case 97:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LGB){
        return shift(91, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == RETURN){
        return shift(88, stc);
    }
    if(synid == IF){
        return shift(98, stc);
    }
    if(synid == WHILE){
        return shift(106, stc);
    }
    if(synid == FOR){
        return shift(111, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == tp){
        return shift(85, stc);
    }
    if(synid == stmt){
        return shift(102, stc);
    }
    if(synid == exp){
        return shift(34, stc);
    }
    if(synid == stmt1){
        return shift(145, stc);
    }
    exit(0);
case 98:
    if(synid == LLB){
        return shift(99, stc);
    }
    exit(0);
case 99:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(100, stc);
    }
    exit(0);
case 100:
    if(synid == COMMA){
        return shift(70, stc);
    }
    if(synid == SETVAL){
        return shift(72, stc);
    }
    if(synid == BIOR){
        return shift(38, stc);
    }
    if(synid == BIAND){
        return shift(36, stc);
    }
    if(synid == OR){
        return shift(64, stc);
    }
    if(synid == XOR){
        return shift(66, stc);
    }
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    if(synid == RLB){
        return shift(101, stc);
    }
    exit(0);
case 101:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LGB){
        return shift(91, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == RETURN){
        return shift(88, stc);
    }
    if(synid == IF){
        return shift(98, stc);
    }
    if(synid == WHILE){
        return shift(106, stc);
    }
    if(synid == FOR){
        return shift(111, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == tp){
        return shift(85, stc);
    }
    if(synid == stmt){
        return shift(102, stc);
    }
    if(synid == exp){
        return shift(34, stc);
    }
    if(synid == stmt1){
        return shift(103, stc);
    }
    exit(0);
case 102:
    return reduce(22);
case 103:
    if(synid == ELSE){
        return shift(104, stc);
    }
    exit(0);
case 104:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LGB){
        return shift(91, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == RETURN){
        return shift(88, stc);
    }
    if(synid == IF){
        return shift(98, stc);
    }
    if(synid == WHILE){
        return shift(106, stc);
    }
    if(synid == FOR){
        return shift(111, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == tp){
        return shift(85, stc);
    }
    if(synid == stmt){
        return shift(105, stc);
    }
    if(synid == exp){
        return shift(34, stc);
    }
    if(synid == stmt1){
        return shift(144, stc);
    }
    exit(0);
case 105:
    return reduce(23);
case 106:
    if(synid == LLB){
        return shift(107, stc);
    }
    exit(0);
case 107:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(108, stc);
    }
    exit(0);
case 108:
    if(synid == COMMA){
        return shift(70, stc);
    }
    if(synid == SETVAL){
        return shift(72, stc);
    }
    if(synid == BIOR){
        return shift(38, stc);
    }
    if(synid == BIAND){
        return shift(36, stc);
    }
    if(synid == OR){
        return shift(64, stc);
    }
    if(synid == XOR){
        return shift(66, stc);
    }
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    if(synid == RLB){
        return shift(109, stc);
    }
    exit(0);
case 109:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LGB){
        return shift(91, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == RETURN){
        return shift(88, stc);
    }
    if(synid == IF){
        return shift(98, stc);
    }
    if(synid == WHILE){
        return shift(106, stc);
    }
    if(synid == FOR){
        return shift(111, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == tp){
        return shift(85, stc);
    }
    if(synid == stmt){
        return shift(110, stc);
    }
    if(synid == exp){
        return shift(34, stc);
    }
    if(synid == stmt1){
        return shift(143, stc);
    }
    exit(0);
case 110:
    return reduce(24);
case 111:
    if(synid == LLB){
        return shift(112, stc);
    }
    exit(0);
case 112:
    if(synid == ID){
        return shift(136, stc);
    }
    if(synid == initlist){
        return shift(113, stc);
    }
    if(synid == init){
        return shift(133, stc);
    }
    exit(0);
case 113:
    if(synid == SEMICOLON){
        return shift(114, stc);
    }
    exit(0);
case 114:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(115, stc);
    }
    exit(0);
case 115:
    if(synid == COMMA){
        return shift(70, stc);
    }
    if(synid == SETVAL){
        return shift(72, stc);
    }
    if(synid == BIOR){
        return shift(38, stc);
    }
    if(synid == BIAND){
        return shift(36, stc);
    }
    if(synid == OR){
        return shift(64, stc);
    }
    if(synid == XOR){
        return shift(66, stc);
    }
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    if(synid == SEMICOLON){
        return shift(116, stc);
    }
    exit(0);
case 116:
    if(synid == BIADD){
        return shift(124, stc);
    }
    if(synid == BIMINUS){
        return shift(131, stc);
    }
    if(synid == ID){
        return shift(126, stc);
    }
    if(synid == steplist){
        return shift(117, stc);
    }
    if(synid == step){
        return shift(121, stc);
    }
    exit(0);
case 117:
    if(synid == RLB){
        return shift(118, stc);
    }
    exit(0);
case 118:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LGB){
        return shift(91, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == RETURN){
        return shift(88, stc);
    }
    if(synid == IF){
        return shift(98, stc);
    }
    if(synid == WHILE){
        return shift(106, stc);
    }
    if(synid == FOR){
        return shift(111, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == tp){
        return shift(85, stc);
    }
    if(synid == stmt){
        return shift(119, stc);
    }
    if(synid == exp){
        return shift(34, stc);
    }
    if(synid == stmt1){
        return shift(120, stc);
    }
    exit(0);
case 119:
    return reduce(25);
case 120:
    if(synid == ELSE){
        return reduce(40);
    }
    exit(0);
case 121:
    if(synid == COMMA){
        return shift(122, stc);
    }
    if(synid == RLB){
        return reduce(32);
    }
    exit(0);
case 122:
    if(synid == BIADD){
        return shift(124, stc);
    }
    if(synid == BIMINUS){
        return shift(131, stc);
    }
    if(synid == ID){
        return shift(126, stc);
    }
    if(synid == steplist){
        return shift(123, stc);
    }
    if(synid == step){
        return shift(121, stc);
    }
    exit(0);
case 123:
    if(synid == RLB){
        return reduce(31);
    }
    exit(0);
case 124:
    if(synid == ID){
        return shift(125, stc);
    }
    exit(0);
case 125:
    return reduce(33);
case 126:
    if(synid == SETVAL){
        return shift(129, stc);
    }
    if(synid == BIADD){
        return shift(127, stc);
    }
    if(synid == BIMINUS){
        return shift(128, stc);
    }
    exit(0);
case 127:
    return reduce(34);
case 128:
    return reduce(36);
case 129:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(130, stc);
    }
    exit(0);
case 130:
    if(synid == SETVAL){
        return shift(72, stc);
    }
    if(synid == BIOR){
        return shift(38, stc);
    }
    if(synid == BIAND){
        return shift(36, stc);
    }
    if(synid == OR){
        return shift(64, stc);
    }
    if(synid == XOR){
        return shift(66, stc);
    }
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(37);
case 131:
    if(synid == ID){
        return shift(132, stc);
    }
    exit(0);
case 132:
    return reduce(35);
case 133:
    if(synid == COMMA){
        return shift(134, stc);
    }
    if(synid == SEMICOLON){
        return reduce(27);
    }
    exit(0);
case 134:
    if(synid == ID){
        return shift(136, stc);
    }
    if(synid == initlist){
        return shift(135, stc);
    }
    if(synid == init){
        return shift(133, stc);
    }
    exit(0);
case 135:
    if(synid == SEMICOLON){
        return reduce(26);
    }
    exit(0);
case 136:
    if(synid == SETVAL){
        return shift(137, stc);
    }
    if(synid == ADDEQ){
        return shift(139, stc);
    }
    if(synid == MINUSEQ){
        return shift(141, stc);
    }
    exit(0);
case 137:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(138, stc);
    }
    exit(0);
case 138:
    if(synid == SETVAL){
        return shift(72, stc);
    }
    if(synid == BIOR){
        return shift(38, stc);
    }
    if(synid == BIAND){
        return shift(36, stc);
    }
    if(synid == OR){
        return shift(64, stc);
    }
    if(synid == XOR){
        return shift(66, stc);
    }
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(28);
case 139:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(140, stc);
    }
    exit(0);
case 140:
    if(synid == SETVAL){
        return shift(72, stc);
    }
    if(synid == BIOR){
        return shift(38, stc);
    }
    if(synid == BIAND){
        return shift(36, stc);
    }
    if(synid == OR){
        return shift(64, stc);
    }
    if(synid == XOR){
        return shift(66, stc);
    }
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(29);
case 141:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(142, stc);
    }
    exit(0);
case 142:
    if(synid == SETVAL){
        return shift(72, stc);
    }
    if(synid == BIOR){
        return shift(38, stc);
    }
    if(synid == BIAND){
        return shift(36, stc);
    }
    if(synid == OR){
        return shift(64, stc);
    }
    if(synid == XOR){
        return shift(66, stc);
    }
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(30);
case 143:
    if(synid == ELSE){
        return reduce(39);
    }
    exit(0);
case 144:
    if(synid == ELSE){
        return reduce(38);
    }
    exit(0);
case 145:
    if(synid == ELSE){
        return shift(146, stc);
    }
    exit(0);
case 146:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LGB){
        return shift(91, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == RETURN){
        return shift(88, stc);
    }
    if(synid == IF){
        return shift(94, stc);
    }
    if(synid == WHILE){
        return shift(147, stc);
    }
    if(synid == FOR){
        return shift(151, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == tp){
        return shift(85, stc);
    }
    if(synid == stmt){
        return shift(105, stc);
    }
    if(synid == exp){
        return shift(34, stc);
    }
    exit(0);
case 147:
    if(synid == LLB){
        return shift(148, stc);
    }
    exit(0);
case 148:
    if(synid == NOT){
        return shift(83, stc);
    }
    if(synid == INUM){
        return shift(74, stc);
    }
    if(synid == ID){
        return shift(76, stc);
    }
    if(synid == LLB){
        return shift(80, stc);
    }
    if(synid == FNUM){
        return shift(75, stc);
    }
    if(synid == exp){
        return shift(149, stc);
    }
    exit(0);
case 149:
    if(synid == COMMA){
        return shift(70, stc);
    }
    if(synid == SETVAL){
        return shift(72, stc);
    }
    if(synid == BIOR){
        return shift(38, stc);
    }
    if(synid == BIAND){
        return shift(36, stc);
    }
    if(synid == OR){
        return shift(64, stc);
    }
    if(synid == XOR){
        return shift(66, stc);
    }
    if(synid == AND){
        return shift(62, stc);
    }
    if(synid == EQ){
        return shift(48, stc);
    }
    if(synid == NOTEQ){
        return shift(50, stc);
    }
    if(synid == LSS){
        return shift(40, stc);
    }
    if(synid == LSSEQ){
        return shift(42, stc);
    }
    if(synid == GRT){
        return shift(44, stc);
    }
    if(synid == GRTEQ){
        return shift(46, stc);
    }
    if(synid == ADD){
        return shift(52, stc);
    }
    if(synid == MINUS){
        return shift(54, stc);
    }
    if(synid == DIV){
        return shift(58, stc);
    }
    if(synid == MOD){
        return shift(60, stc);
    }
    if(synid == MULTI){
        return shift(56, stc);
    }
    if(synid == BITNOT){
        return shift(68, stc);
    }
    if(synid == RLB){
        return shift(150, stc);
    }
    exit(0);
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

void lex_part(){
    initTrie();
    // char cc[100] = "while Rick hits Morty break dollardollar 10 for 1024 plumbus 1.11  == 2.22 ++ ";
    char cc[100] = "int i, j; int fun(int a, int b){int m;if(a > b) m = a; return m;}";
    for(int i = 0; i < strlen(cc); i++){
        if(lex(cc[i], 1, i)){
            lex(cc[i], 1, i);
        }
    }
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
    for(int i = 1; !fin && i <= wordsSize; i++){
        Sentence* res = synaxdfa(words + i);
        printf("%d state:%d:\n", i, syn_analysis_state);
        debugsyn();
        printf("\n");
        if(res){
            printf("%d:(reduce) state:%d\n", i, syn_analysis_state);
            synaxdfa(res);
            debugsyn();
            printf("\n");
            i--;
        }
    }
}

int main()
{
    printf("lex:\n");
    lex_part();
    printf("\n\nsyntax:\n");
    syn_part();
    printf("%d\n", fin);
    return 0;
}