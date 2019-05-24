#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define DX 12

#define FUNC 1
#define VAR 2
#define ARRAY 3

#define ROWMAX 10000

typedef struct symtable{
    char *name;
    int type;
    int level;
    int offset;
    int *types;
    int *argnum;
    char oname[20];
    int namedup;
} symtable;

// symtable table[ROWMAX];
// int tptr;
// int bptr;

#define NOT_FOUND 1
#define FOUND_IN_SAME_LEVEL 2
#define FOUND_IN_DIFF_LEVEL 3
#define FUNC_CONFLICT 4
#define SYMBOL_RENAME 5

#endif