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
// #define MAIN 12
#define RETURN 12
#define TRUE 13
#define WHILE 14

// operator
// ++, --, !, ~
#define BIADD 30
#define BIMINUS 31
// #define NEG 32
#define NOT 32
#define BITNOT 33
// *, /, %, +, -
#define MULTI 34
#define DIV 35
#define MOD 36
#define ADD 37
#define MINUS 38
// <<, >>
#define LSHIFT 39
#define RSHIFT 40
// >, >=, <, <=, ==, !=
#define GRT 41
#define GRTEQ 42
#define LSS 43
#define LSSEQ 44
#define EQ 45
#define NOTEQ 46
// &, ^, |, &&, ||
#define AND 47
#define XOR 48
#define OR 49
#define BIAND 50
#define BIOR 51
// +=, -=, *=, /=, %=
#define ADDEQ 52
#define MINUSEQ 53
#define MULTIEQ 54
#define DIVEQ 55
#define MODEQ 56
// =
#define SETVAL 57

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
#define SYNAXELE 106

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
    int sepType;   // only for SEPERATORT
    int synVal;     // only for SYNAXELE
} typeVal;

typedef struct Word{
    int type;      // ID, KEYWORD, INUM, FNUM, OPERATER, SEPERATOR, SYNAXELE
    typeVal tval;
    int op;        // only for OPERATER
    struct Word* next[20];
} Word;

typedef struct Word Sentence;

#define MAXWORDS 10007
#define MAXSIZE 307

const char* keywords[MAXSIZE];
const char* words_type[MAXSIZE];
const char* op_type[MAXSIZE];
const char* op_detail_type[MAXSIZE];
const char* sep_type[MAXSIZE];
Word words[MAXWORDS];
int wordsSize;

#endif