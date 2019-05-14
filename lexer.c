#include "wordunit.h"
#include "dfa.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define isAlpha(x) ((x) >= 'A' && (x) <= 'Z' || (x) >= 'a' && (x) <= 'z')
#define isNum(x) ((x) >= '0' && (x) <= '9')
#define isAlnum(x) (isAlpha((x)) || isNum((x)))

static const char* keywords[] = {"", "bool", "break", "char", "continue",
                              "do", "else", "false", "float", "for", "if",
                              "int", "main", "return", "true", "while"};
static Trie root;
static int dfastate = SPACE;

void insertTrie(const char *str, int no, Trie root){
    Trie p = root;
    for(int i = 0; str[i]; i++){
        int idx = str[i] - 'a';
        if(p->next[idx] == NULL){
            p->next[idx] = (Trie) malloc(sizeof(TrieNode));
            memset(p->next[idx], 0, sizeof(TrieNode));
        }
        p = p->next[idx]; 
    }
    p->no = no;
    p->value = str;
}

void initTrie(){
    root = (Trie) malloc(sizeof(TrieNode));
    memset(root, 0, sizeof(TrieNode));
    for(int i = 1; i <= WHILE; i++)
        insertTrie(keywords[i], i, root);
}

Trie matchTrie(const char *str, Trie root){
    Trie p = root;
    for(int i = 0; str[i]; i++){
        int idx = str[i] - 'a';
        if(p->next[idx] == NULL) return NULL;
        p = p->next[idx];
    }
    if(p->no == 0) return NULL;
    return p;
}

char tmp[256];
int tmptop = 0;
float tmpVal = 0f;

void lex_error(int line){
    tmp[tmptop + 1] = '\0';
    printf("Lex error: \"%s...\" in line %d\n", tmp + 1, line);
    printf("Error code: [%d]", dfastate);   
}

void back2Space(){
    tmptop = 0;
    tmpVal = 0;
    dfastate = SPACE;
}

int lex(char c, int line){
    switch(dfastate){
        case SPACE:
            if(c == ' ' || c == '\t' || c == '\n'
                || c == '\v' || c == '\f' || c == '\r')
                dfastate = SPACE;
            else if(c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z'){
                tmp[++tmptop] = c;
                dfastate = ID_KEY_ALNUM;
            }else
                lex_error(line);
            break;

        case ID_KEY_ALNUM:
            if(c >= 'A')
            break;
        case ID_KEY_GET:
            break;

        case INUM_NUM:
            break;
        case INUM_GET:
            break;

        case FNUM_DOT:
            break;
        case FNUM_GET:
            break;

        case ADD_ONE:
            break;
        case ADD_GET:
            break;
        case ADD_ADD_GET:
            break;
        case ADDEQ_GET:
            break;

        case MINUS_ONE:
            break;
        case MINUS_GET:
            break;
        case MINUS_MINUS_GET:
            break;
        case MINUSEQ_GET:
            break;

        case NOT_ONE:
            break;
        case NOT_GET:
            break;
        case NOTEQ_GET:
            break;
        case BITNOT_GET:
            break;

        case OR_ONE:
            break;
        case OR_GET:
            break;
        case BIOR_GET:
            break;

        case AND_ONE:
            break;
        case AND_GET:
            break;
        case BIAND_GET:
            break;

        case XOR_GET:
            break;

        case LSS_ONE:
            break;
        case LSS_GET:
            break;
        case LSSEQ_GET:
            break;
        case LSHIFT_GET:
            break;

        case GRT_ONE:
            break;
        case GRT_GET:
            break;
        case GRTEQ_GET:
            break;
        case RSHIFT_GET:
            break;

        case MUITI_ONE:
            break;
        case MUITI_GET:
            break;
        case MUITIEQ_GET:
            break;

        case DIV_ONE:
            break;
        case DIV_GET:
            break;
        case DIVEQ_GET:
            break;

        case MOD_ONE:
            break;
        case MOD_GET:
            break;
        case MODEQ_GET:
            break;

        case ASSIGN_ONE:
            break;
        case ASSIGN_GET:
            break;
        case EQ_GET:
            break;

        default:
            break    
    }
}


// int main()
// {
//     initTrie();
//     Trie res = matchTrie("", root);
//     if(res != NULL)
//         printf("%d %s\n", res->no, res->value);
//     else
//         printf("can't find\n");
//     return 0;
// }