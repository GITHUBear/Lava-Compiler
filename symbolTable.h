#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define FUNC 1
#define VAR 2

#define ROWMAX 10000

typedef struct symtable{
    char *name;
    int type;
    int level;
    int offset;
    int types[12];
} symtable;

symtable table[ROWMAX];
int tptr;
int bptr;

#endif