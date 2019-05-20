#ifndef SYNAX_H
#define SYNAX_H

// syntax elements
#define program 120
#define deflist 121
#define tp 122
#define def 123
#define vallist 124
#define fundef 125
#define stmtlist 126
#define args 127
#define param 128
#define stmt 129
#define exp 130
#define initlist 131
#define init 132
#define steplist 133
#define step 134
#define compstmt 135
#define innerdeflist 136
#define innerdef 137
#define innerdeclist 138
#define innerdec 139
#define END 140

#define SYNAX_ELE_MAX 300
#define STACKMAX 10000

const char* synax_ele[SYNAX_ELE_MAX];
const int grammar_length[SYNAX_ELE_MAX];
int stateStk[STACKMAX];
Sentence* stcStk[STACKMAX];
int stkTop;
int syn_analysis_state;
int fin;

#endif