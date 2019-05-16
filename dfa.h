#ifndef DFA_H
#define DFA_H

// DFA state
#define SPACE 1
#define ID_KEY_ALNUM 2
#define ID_KEY_GET 3
#define INUM_NUM 4
#define INUM_GET 5
#define FNUM_DOT 6
#define FNUM_GET 7

#define ADD_ONE 8
#define ADD_GET 9
#define ADD_ADD_GET 10
#define ADDEQ_GET 11

#define MINUS_ONE 12
#define MINUS_GET 13
#define MINUS_MINUS_GET 14
#define MINUSEQ_GET 15

#define NOT_ONE 16
#define NOT_GET 17
#define NOTEQ_GET 18

#define BITNOT_GET 19

#define OR_ONE 20
#define OR_GET 21
#define BIOR_GET 22

#define AND_ONE 23
#define AND_GET 24
#define BIAND_GET 25

#define XOR_GET 26

#define LSS_ONE 27
#define LSS_GET 28
#define LSSEQ_GET 29
#define LSHIFT_GET 30

#define GRT_ONE 31
#define GRT_GET 32
#define GRTEQ_GET 33
#define RSHIFT_GET 34

#define MUITI_ONE 35
#define MUITI_GET 36
#define MUITIEQ_GET 37

#define DIV_ONE 38
#define DIV_GET 39
#define DIVEQ_GET 40

#define MOD_ONE 41
#define MOD_GET 42
#define MODEQ_GET 43

#define ASSIGN_ONE 44
#define ASSIGN_GET 45
#define EQ_GET 46

int lex(char c, int line, int pos);

#endif