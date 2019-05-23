#ifndef SEM_H
#define SEM_H

#define SEM_VOID 0
#define SEM_INT 1
#define SEM_FLOAT 2
#define SEM_CHAR 3
#define SEM_FUNC 4

#define SEM_INT_ARRAY 5
#define SEM_FLOAT_ARRAY 6
#define SEM_CHAR_ARRAY 7

#define SEM_INT_EXP 11
#define SEM_FLOAT_EXP 12
#define SEM_CHAR_EXP 13
#define SEM_BOOL_EXP 14
#define SEM_UNDEF_EXP 15

#define EXPOFFSET (SEM_INT_EXP - SEM_INT)

#define INT_SIZE 4
#define FLOAT_SIZE 4
#define CHAR_SIZE 1

void sem(Sentence* node, int rulenum, int stepnum);

#endif