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
                                  3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 3, 2, 3, 3, 3, 3, 3, 4};
const int grammar_rule[] = {program, deflist, deflist, tp, tp, tp, tp, vallist, vallist, def, def, fundef, fundef,
                          args, args, param, stmtlist, stmtlist, stmt, stmt, stmt, stmt, stmt, stmt, stmt, stmt,
                          initlist, initlist, init, init, init, steplist, steplist, step, step, step, step, step, stmt1, stmt1, stmt1,
                          exp, exp, exp, exp, exp, exp, exp, exp, exp, exp, exp, exp, 
                          exp, exp, exp, exp, exp, exp, exp, exp, exp, exp, exp, exp};

Sentence* mkNode(int ruleNum){
    Sentence* newnode = (Sentence*) malloc(sizeof(Sentence));
    newnode->type = SYNAXELE;
    newnode->tval.synVal = grammar_rule[ruleNum];
    int startpos = stkTop - grammar_length[ruleNum] + 1;
    int endpos = stkTop;
    for(; startpos <= endpos; startpos++)
        newnode->next[startpos] = stcStk[startpos];
    return newnode;
}

Sentence* shift(int nextState, Sentence* stc){
    stateStk[++stkTop] = nextState;
    stcStk[stkTop] = stc;
    return NULL;
}

Sentence* reduce(int ruleNum){
    Sentence* newnode = mkNode(ruleNum);
    stkTop = stkTop - grammar_length[ruleNum];
    syn_analysis_state = stateStk[stkTop];
    return newnode;
}

void initSynaxDfa(){
    stkTop = 0;
    syn_analysis_state = 0;
}

Sentence* synaxdfa(Sentence* stc){
    switch(syn_analysis_state){
        
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
        default:
            printf("\n");
            break;
    }
}

int main()
{
    initTrie();
    // char cc[100] = "while Rick hits Morty break dollardollar 100 for 1024 plumbus 1.11  == 2.22 ++ ";
    char cc[100] = "int i, j; int fun(int a, float b){int m;if(a>b) m=a;else m=b;return m;}float x,y; ";
    for(int i = 0; i < strlen(cc); i++){
        if(lex(cc[i], 1, i)){
            lex(cc[i], 1, i);
        }
    }
    for(int i = 1; i <= wordsSize; i++){
        printWordInfo(words[i]);
    }
    return 0;
}