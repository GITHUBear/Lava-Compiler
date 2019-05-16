#ifndef WORD_UNIT_H
#define WORD_UNIT_H

// reserved word
#define BOOL 1
#define BREAK 2
#define CHAR 3
#define CONTINUE 4
#define DO 5
#define ELSE 6
#define FALSE 7
#define FLOAT 8
#define FOR 9
#define IF 10
#define INT 11
#define MAIN 12
#define RETURN 13
#define TRUE 14
#define WHILE 15

// operator
// ++, --, !, ~
#define BIADD 30
#define BIMINUS 31
// #define NEG 32
#define NOT 33
#define BITNOT 34
// *, /, %, +, -
#define MULTI 35
#define DIV 36
#define MOD 37
#define ADD 38
#define MINUS 39
// <<, >>
#define LSHIFT 40
#define RSHIFT 41
// >, >=, <, <=, ==, !=
#define GRT 42
#define GRTEQ 43
#define LSS 44
#define LSSEQ 45
#define EQ 46
#define NOTEQ 47
// &, ^, |, &&, ||
#define AND 48
#define XOR 49
#define OR 50
#define BIAND 51
#define BIOR 52
// +=, -=, *=, /=, %=
#define ADDEQ 53
#define MINUSEQ 54
#define MULTIEQ 55
#define DIVEQ 56
#define MODEQ 57
// =
#define SETVAL 58

// operator type
#define UNARY_OP 60
#define BIT_OP 61
#define ARITH_OP 62
#define SHIFT_OP 63
#define RELAT_OP 64
#define LOGIC_OP 65
#define MIXASSIGN_OP 66
#define ASSIGN_OP 67

// seperator
#define COMMA 70      // ,
#define SEMICOLON 71   // ;
#define LLB 72          // (
#define RLB 73          // )
#define LMB 74          // [
#define RMB 75          // ]
#define LGB 76           // {
#define RGB 77           // }

// word type
#define INUM 100
#define FNUM 101
#define ID 102
#define KEYWORD 103
#define OPERATER 104
#define SEPERATOR 105

typedef struct TrieNode{
    int no;
    const char *value;
    struct TrieNode *next[26];
} TrieNode;

typedef TrieNode* Trie; 

void insertTrie(const char *str, int no, Trie root);
void initTrie();
Trie matchTrie(const char *str, Trie root);

typedef union typeVal{
    char* name;   // only for ID
    int keyIdx;     // only for KEYWORD
    int ivalue;     // only for INUM
    float fvalue;    // only for FNUM
    int opType;    // only for OPERATER
    int sepType;   // only for SEPERATOR
} typeVal;

typedef struct Word{
    int type;      // ID, KEYWORD, INUM, FNUM, OPERATER, SEPERATOR
    typeVal tval;
    int op;        // only for OPERATER
} Word;

#define MAXWORDS 10007
#define MAXSIZE 307

const char* keywords[MAXSIZE];
const char* words_type[MAXSIZE];
const char* op_type[MAXSIZE];
const char* sep_type[MAXSIZE];
Word words[MAXWORDS];
int wordsSize;

#endif