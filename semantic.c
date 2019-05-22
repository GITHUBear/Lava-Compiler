#include "wordunit.h"
#include "synax.h"
#include "symbolTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void showAST(Sentence* node, int lv);
extern void lex_part(FILE* file);
extern void syn_part();

extern symtable table[];
extern int tptr;
extern int bptr;

int main()
{
    FILE* f = fopen("./Test/SYNTAX_TEST3.txt", "r");
    freopen("./TestRes/TEST3_RES.txt", "w", stdout);
    printf("lex:\n");
    lex_part(f);
    printf("\n\nsyntax:\n");
    syn_part();
    // printf("%d\n", fin);
    showAST(pgm, 0);
    fclose(f);
    return 0;
}