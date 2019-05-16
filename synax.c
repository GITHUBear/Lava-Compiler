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

static const char* synax_ele[] = {"program", "deflist", "tp", "def", "vallist", "fundef",
                              "stmtlist", "args", "param", "stmt", "exp", "initlist", "init",
                              "steplist", "step", "stmt1"};


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