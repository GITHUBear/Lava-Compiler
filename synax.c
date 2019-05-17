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
    break;
case 1:
    if(synid == END){
        return reduce(0);
    }
    break;
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
    break;
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
    break;
case 4:
    if(synid == END){
        return reduce(1);
    }
    break;
case 5:
    if(synid == LMB){
        return shift(6, stc);
    }
    if(synid == ID){
        return reduce(3);
    }
    break;
case 6:
    if(synid == INUM){
        return shift(7, stc);
    }
    break;
case 7:
    if(synid == RMB){
        return shift(8, stc);
    }
    break;
case 8:
    if(synid == ID){
        return reduce(5);
    }
    break;
case 9:
    if(synid == LMB){
        return shift(10, stc);
    }
    if(synid == ID){
        return reduce(4);
    }
    break;
case 10:
    if(synid == INUM){
        return shift(11, stc);
    }
    break;
case 11:
    if(synid == RMB){
        return shift(12, stc);
    }
    break;
case 12:
    if(synid == ID){
        return reduce(6);
    }
    break;
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
    break;
case 14:
    if(synid == ID){
        return shift(15, stc);
    }
    if(synid == vallist){
        return shift(16, stc);
    }
    break;
case 15:
    if(synid == COMMA){
        return shift(14, stc);
    }
    if(synid == SEMICOLON){
        return reduce(8);
    }
    break;
case 16:
    if(synid == SEMICOLON){
        return reduce(7);
    }
    break;
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
    break;
case 18:
    if(synid == RLB){
        return shift(19, stc);
    }
    break;
case 19:
    if(synid == LGB){
        return reduce(11);
    }
    break;
case 20:
    if(synid == LGB){
        return reduce(12);
    }
    break;
case 21:
    if(synid == COMMA){
        return shift(22, stc);
    }
    if(synid == RLB){
        return reduce(14);
    }
    break;
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
    break;
case 23:
    if(synid == RLB){
        return reduce(13);
    }
    break;
case 24:
    if(synid == ID){
        return shift(25, stc);
    }
    break;
case 25:
    return reduce(15);
    break;
case 26:
    if(synid == SEMICOLON){
        return shift(27, stc);
    }
    break;
case 27:
    return reduce(9);
    break;
case 28:
    if(synid == LGB){
        return shift(29, stc);
    }
    break;
case 29:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LGB){
        return shift(89, stc);
    }
    if(synid == RGB){
        return reduce(17);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == RETURN){
        return shift(86, stc);
    }
    if(synid == IF){
        return shift(92, stc);
    }
    if(synid == WHILE){
        return shift(145, stc);
    }
    if(synid == FOR){
        return shift(149, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == tp){
        return shift(83, stc);
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
    break;
case 30:
    if(synid == RGB){
        return shift(31, stc);
    }
    break;
case 31:
    return reduce(10);
    break;
case 32:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LGB){
        return shift(89, stc);
    }
    if(synid == RGB){
        return reduce(17);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == RETURN){
        return shift(86, stc);
    }
    if(synid == IF){
        return shift(92, stc);
    }
    if(synid == WHILE){
        return shift(145, stc);
    }
    if(synid == FOR){
        return shift(149, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == tp){
        return shift(83, stc);
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
    break;
case 33:
    if(synid == RGB){
        return reduce(16);
    }
    break;
case 34:
    if(synid == COMMA){
        return shift(70, stc);
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
    break;
case 35:
    return reduce(18);
    break;
case 36:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(37, stc);
    }
    break;
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
    break;
case 38:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(39, stc);
    }
    break;
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
    break;
case 40:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(41, stc);
    }
    break;
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
    break;
case 42:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(43, stc);
    }
    break;
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
    break;
case 44:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(45, stc);
    }
    break;
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
    break;
case 46:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(47, stc);
    }
    break;
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
    break;
case 48:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(49, stc);
    }
    break;
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
    break;
case 50:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(51, stc);
    }
    break;
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
    break;
case 52:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(53, stc);
    }
    break;
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
    break;
case 54:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(55, stc);
    }
    break;
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
    break;
case 56:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(57, stc);
    }
    break;
case 57:
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(51);
    break;
case 58:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(59, stc);
    }
    break;
case 59:
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(52);
    break;
case 60:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(61, stc);
    }
    break;
case 61:
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(53);
    break;
case 62:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(63, stc);
    }
    break;
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
    break;
case 64:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(65, stc);
    }
    break;
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
    break;
case 66:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(67, stc);
    }
    break;
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
    break;
case 68:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(69, stc);
    }
    break;
case 69:
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(62);
    break;
case 70:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(71, stc);
    }
    break;
case 71:
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
    break;
case 72:
    return reduce(54);
    break;
case 73:
    return reduce(55);
    break;
case 74:
    if(synid == LMB){
        return shift(75, stc);
    }
    return reduce(56);
    break;
case 75:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(76, stc);
    }
    break;
case 76:
    if(synid == COMMA){
        return shift(70, stc);
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
        return shift(77, stc);
    }
    break;
case 77:
    return reduce(64);
    break;
case 78:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(79, stc);
    }
    break;
case 79:
    if(synid == COMMA){
        return shift(70, stc);
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
        return shift(80, stc);
    }
    break;
case 80:
    return reduce(57);
    break;
case 81:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(82, stc);
    }
    break;
case 82:
    if(synid == BITNOT){
        return shift(68, stc);
    }
    return reduce(58);
    break;
case 83:
    if(synid == ID){
        return shift(15, stc);
    }
    if(synid == vallist){
        return shift(84, stc);
    }
    break;
case 84:
    if(synid == SEMICOLON){
        return shift(85, stc);
    }
    break;
case 85:
    return reduce(19);
    break;
case 86:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(87, stc);
    }
    break;
case 87:
    if(synid == COMMA){
        return shift(70, stc);
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
        return shift(88, stc);
    }
    break;
case 88:
    return reduce(20);
    break;
case 89:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LGB){
        return shift(89, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == RETURN){
        return shift(86, stc);
    }
    if(synid == IF){
        return shift(92, stc);
    }
    if(synid == WHILE){
        return shift(145, stc);
    }
    if(synid == FOR){
        return shift(149, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == tp){
        return shift(83, stc);
    }
    if(synid == stmt){
        return shift(90, stc);
    }
    if(synid == exp){
        return shift(34, stc);
    }
    break;
case 90:
    if(synid == RGB){
        return shift(91, stc);
    }
    break;
case 91:
    return reduce(21);
    break;
case 92:
    if(synid == LLB){
        return shift(93, stc);
    }
    break;
case 93:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(94, stc);
    }
    break;
case 94:
    if(synid == COMMA){
        return shift(70, stc);
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
        return shift(95, stc);
    }
    break;
case 95:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LGB){
        return shift(89, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == RETURN){
        return shift(86, stc);
    }
    if(synid == IF){
        return shift(96, stc);
    }
    if(synid == WHILE){
        return shift(104, stc);
    }
    if(synid == FOR){
        return shift(109, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == tp){
        return shift(83, stc);
    }
    if(synid == stmt){
        return shift(100, stc);
    }
    if(synid == exp){
        return shift(34, stc);
    }
    if(synid == stmt1){
        return shift(143, stc);
    }
    break;
case 96:
    if(synid == LLB){
        return shift(97, stc);
    }
    break;
case 97:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(98, stc);
    }
    break;
case 98:
    if(synid == COMMA){
        return shift(70, stc);
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
        return shift(99, stc);
    }
    break;
case 99:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LGB){
        return shift(89, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == RETURN){
        return shift(86, stc);
    }
    if(synid == IF){
        return shift(96, stc);
    }
    if(synid == WHILE){
        return shift(104, stc);
    }
    if(synid == FOR){
        return shift(109, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == tp){
        return shift(83, stc);
    }
    if(synid == stmt){
        return shift(100, stc);
    }
    if(synid == exp){
        return shift(34, stc);
    }
    if(synid == stmt1){
        return shift(101, stc);
    }
    break;
case 100:
    return reduce(22);
    break;
case 101:
    if(synid == ELSE){
        return shift(102, stc);
    }
    break;
case 102:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LGB){
        return shift(89, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == RETURN){
        return shift(86, stc);
    }
    if(synid == IF){
        return shift(96, stc);
    }
    if(synid == WHILE){
        return shift(104, stc);
    }
    if(synid == FOR){
        return shift(109, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == tp){
        return shift(83, stc);
    }
    if(synid == stmt){
        return shift(103, stc);
    }
    if(synid == exp){
        return shift(34, stc);
    }
    if(synid == stmt1){
        return shift(142, stc);
    }
    break;
case 103:
    return reduce(23);
    break;
case 104:
    if(synid == LLB){
        return shift(105, stc);
    }
    break;
case 105:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(106, stc);
    }
    break;
case 106:
    if(synid == COMMA){
        return shift(70, stc);
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
        return shift(107, stc);
    }
    break;
case 107:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LGB){
        return shift(89, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == RETURN){
        return shift(86, stc);
    }
    if(synid == IF){
        return shift(96, stc);
    }
    if(synid == WHILE){
        return shift(104, stc);
    }
    if(synid == FOR){
        return shift(109, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == tp){
        return shift(83, stc);
    }
    if(synid == stmt){
        return shift(108, stc);
    }
    if(synid == exp){
        return shift(34, stc);
    }
    if(synid == stmt1){
        return shift(141, stc);
    }
    break;
case 108:
    return reduce(24);
    break;
case 109:
    if(synid == LLB){
        return shift(110, stc);
    }
    break;
case 110:
    if(synid == ID){
        return shift(134, stc);
    }
    if(synid == initlist){
        return shift(111, stc);
    }
    if(synid == init){
        return shift(131, stc);
    }
    break;
case 111:
    if(synid == SEMICOLON){
        return shift(112, stc);
    }
    break;
case 112:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(113, stc);
    }
    break;
case 113:
    if(synid == COMMA){
        return shift(70, stc);
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
        return shift(114, stc);
    }
    break;
case 114:
    if(synid == BIADD){
        return shift(122, stc);
    }
    if(synid == BIMINUS){
        return shift(129, stc);
    }
    if(synid == ID){
        return shift(124, stc);
    }
    if(synid == steplist){
        return shift(115, stc);
    }
    if(synid == step){
        return shift(119, stc);
    }
    break;
case 115:
    if(synid == RLB){
        return shift(116, stc);
    }
    break;
case 116:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LGB){
        return shift(89, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == RETURN){
        return shift(86, stc);
    }
    if(synid == IF){
        return shift(96, stc);
    }
    if(synid == WHILE){
        return shift(104, stc);
    }
    if(synid == FOR){
        return shift(109, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == tp){
        return shift(83, stc);
    }
    if(synid == stmt){
        return shift(117, stc);
    }
    if(synid == exp){
        return shift(34, stc);
    }
    if(synid == stmt1){
        return shift(118, stc);
    }
    break;
case 117:
    return reduce(25);
    break;
case 118:
    if(synid == ELSE){
        return reduce(40);
    }
    break;
case 119:
    if(synid == COMMA){
        return shift(120, stc);
    }
    if(synid == RLB){
        return reduce(32);
    }
    break;
case 120:
    if(synid == BIADD){
        return shift(122, stc);
    }
    if(synid == BIMINUS){
        return shift(129, stc);
    }
    if(synid == ID){
        return shift(124, stc);
    }
    if(synid == steplist){
        return shift(121, stc);
    }
    if(synid == step){
        return shift(119, stc);
    }
    break;
case 121:
    if(synid == RLB){
        return reduce(31);
    }
    break;
case 122:
    if(synid == ID){
        return shift(123, stc);
    }
    break;
case 123:
    return reduce(33);
    break;
case 124:
    if(synid == SETVAL){
        return shift(127, stc);
    }
    if(synid == BIADD){
        return shift(125, stc);
    }
    if(synid == BIMINUS){
        return shift(126, stc);
    }
    break;
case 125:
    return reduce(34);
    break;
case 126:
    return reduce(36);
    break;
case 127:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(128, stc);
    }
    break;
case 128:
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
    break;
case 129:
    if(synid == ID){
        return shift(130, stc);
    }
    break;
case 130:
    return reduce(35);
    break;
case 131:
    if(synid == COMMA){
        return shift(132, stc);
    }
    if(synid == SEMICOLON){
        return reduce(27);
    }
    break;
case 132:
    if(synid == ID){
        return shift(134, stc);
    }
    if(synid == initlist){
        return shift(133, stc);
    }
    if(synid == init){
        return shift(131, stc);
    }
    break;
case 133:
    if(synid == SEMICOLON){
        return reduce(26);
    }
    break;
case 134:
    if(synid == SETVAL){
        return shift(135, stc);
    }
    if(synid == ADDEQ){
        return shift(137, stc);
    }
    if(synid == MINUSEQ){
        return shift(139, stc);
    }
    break;
case 135:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(136, stc);
    }
    break;
case 136:
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
    break;
case 137:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(138, stc);
    }
    break;
case 138:
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
    break;
case 139:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(140, stc);
    }
    break;
case 140:
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
    break;
case 141:
    if(synid == ELSE){
        return reduce(39);
    }
    break;
case 142:
    if(synid == ELSE){
        return reduce(38);
    }
    break;
case 143:
    if(synid == ELSE){
        return shift(144, stc);
    }
    break;
case 144:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LGB){
        return shift(89, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == RETURN){
        return shift(86, stc);
    }
    if(synid == IF){
        return shift(92, stc);
    }
    if(synid == WHILE){
        return shift(145, stc);
    }
    if(synid == FOR){
        return shift(149, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == tp){
        return shift(83, stc);
    }
    if(synid == stmt){
        return shift(103, stc);
    }
    if(synid == exp){
        return shift(34, stc);
    }
    break;
case 145:
    if(synid == LLB){
        return shift(146, stc);
    }
    break;
case 146:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(147, stc);
    }
    break;
case 147:
    if(synid == COMMA){
        return shift(70, stc);
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
        return shift(148, stc);
    }
    break;
case 148:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LGB){
        return shift(89, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == RETURN){
        return shift(86, stc);
    }
    if(synid == IF){
        return shift(92, stc);
    }
    if(synid == WHILE){
        return shift(145, stc);
    }
    if(synid == FOR){
        return shift(149, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == tp){
        return shift(83, stc);
    }
    if(synid == stmt){
        return shift(108, stc);
    }
    if(synid == exp){
        return shift(34, stc);
    }
    break;
case 149:
    if(synid == LLB){
        return shift(150, stc);
    }
    break;
case 150:
    if(synid == ID){
        return shift(134, stc);
    }
    if(synid == initlist){
        return shift(151, stc);
    }
    if(synid == init){
        return shift(131, stc);
    }
    break;
case 151:
    if(synid == SEMICOLON){
        return shift(152, stc);
    }
    break;
case 152:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == exp){
        return shift(153, stc);
    }
    break;
case 153:
    if(synid == COMMA){
        return shift(70, stc);
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
        return shift(154, stc);
    }
    break;
case 154:
    if(synid == BIADD){
        return shift(122, stc);
    }
    if(synid == BIMINUS){
        return shift(129, stc);
    }
    if(synid == ID){
        return shift(124, stc);
    }
    if(synid == steplist){
        return shift(155, stc);
    }
    if(synid == step){
        return shift(119, stc);
    }
    break;
case 155:
    if(synid == RLB){
        return shift(156, stc);
    }
    break;
case 156:
    if(synid == NOT){
        return shift(81, stc);
    }
    if(synid == INT){
        return shift(5, stc);
    }
    if(synid == FLOAT){
        return shift(9, stc);
    }
    if(synid == INUM){
        return shift(72, stc);
    }
    if(synid == ID){
        return shift(74, stc);
    }
    if(synid == LGB){
        return shift(89, stc);
    }
    if(synid == LLB){
        return shift(78, stc);
    }
    if(synid == RETURN){
        return shift(86, stc);
    }
    if(synid == IF){
        return shift(92, stc);
    }
    if(synid == WHILE){
        return shift(145, stc);
    }
    if(synid == FOR){
        return shift(149, stc);
    }
    if(synid == FNUM){
        return shift(73, stc);
    }
    if(synid == tp){
        return shift(83, stc);
    }
    if(synid == stmt){
        return shift(117, stc);
    }
    if(synid == exp){
        return shift(34, stc);
    }
    break;
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