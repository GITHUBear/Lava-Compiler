#include "wordunit.h"
#include "dfa.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEBUG

#define isBlank(x) ((x) == ' ' || (x) == '\n' || (x) == '\t' || (x) == '\v' || (x) == '\f' || (x) == '\r')
#define isAlpha(x) ((x) >= 'A' && (x) <= 'Z' || (x) >= 'a' && (x) <= 'z')
#define isNum(x) ((x) >= '0' && (x) <= '9')
#define isAlnum(x) (isAlpha((x)) || isNum((x)))

const char* keywords[] = {"", "bool", "break", "char", "continue",
                              "do", "else", "false", "float", "for", "if",
                              "int", "return", "true", "while"};
const char* words_type[] = {"INUM", "FNUM", "ID", "KEYWORD", "OPERATOR", "SEPERATOR", "SYNAXELE", "CNUM"};
const char* op_type[] = {"UNARY_OP", "BIT_OP", "ARITH_OP", 
                              "SHIFT_OP", "RELAT_OP", "LOGIC_OP",
                              "MIXASSIGN_OP", "ASSIGN_OP"};
const char* sep_type[] = {",", ";", "(", ")", "[", "]", "{", "}"};
const char* op_detail_type[] = {"++", "--", "!", "~", "*", "/", "%", "+", "-", "<<", ">>",
                             ">", ">=", "<", "<=", "==", "!=", "&", "^", "|", "&&", "||",
                             "+=", "-=", "*=", "/=", "%=", "="};

Trie root;
int dfastate = SPACE;

Word words[MAXWORDS];
int wordsSize = 0;

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
        if(str[i] > 'z' || str[i] < 'a') return NULL;
        int idx = str[i] - 'a';
        if(p->next[idx] == NULL) return NULL;
        p = p->next[idx];
    }
    if(p->no == 0) return NULL;
    return p;
}

char tmp[256];
char tmpChar;
int tmptop = 0;
int tmpIval = 0;
float fw = 0.1f;
float tmpFval = 0.0f;

void lex_error(int line, int pos){
    tmp[tmptop + 1] = '\0';
    printf("Lex error: \"...[%s]...\" in line %d at %d\n", tmp + 1, line, pos + 1);
    printf("Error code: [%d]\n", dfastate);   
}

void back2Space(){
    tmpChar = 0;
    tmptop = 0;
    tmpIval = 0;
    fw = 0.1f;
    tmpFval = 0.0f;
    dfastate = SPACE;
}

// void printWordInfo(Word w){
//     printf("(%s):", words_type[w.type - INUM]);
//     switch(w.type){
//         case ID:
//             printf("[%s]\n", w.tval.name);
//             break;
//         case KEYWORD:
//             printf("[%s]\n", keywords[w.tval.keyIdx]);
//             break;
//         case INUM:
//             printf("[%d]\n", w.tval.ivalue);
//             break;
//         case FNUM:
//             printf("[%f]\n", w.tval.fvalue);
//             break;
//         case OPERATER:
//             printf("[%s, %d]\n", op_type[w.tval.opType - UNARY_OP], w.op);
//             break;
//         case SEPERATOR:
//             printf("[%s]\n", sep_type[w.tval.sepType - COMMA]);
//             break;
//         default:
//             printf("\n");
//             break;
//     }
// }

int lex(char c, int line, int pos){
    switch(dfastate){
        case SPACE:
            if(isBlank(c))
                dfastate = SPACE;
            else if(isAlpha(c)){
                tmp[++tmptop] = c;
                dfastate = ID_KEY_ALNUM;
            }else if(isNum(c)){
                tmpIval = (tmpIval << 3) + (tmpIval << 1) + (c - '0');
                dfastate = INUM_NUM;
            }else if(c == '+'){
                dfastate = ADD_ONE;
            }else if(c == '-'){
                dfastate = MINUS_ONE;
            }else if(c == '!'){
                dfastate = NOT_ONE;
            }else if(c == '~'){
                dfastate = BITNOT_GET;
            }else if(c == '|'){
                dfastate = OR_ONE;
            }else if(c == '&'){
                dfastate = AND_ONE;
            }else if(c == '^'){
                dfastate = XOR_GET;
            }else if(c == '<'){
                dfastate = LSS_ONE;
            }else if(c == '>'){
                dfastate = GRT_ONE;
            }else if(c == '*'){
                dfastate = MUITI_ONE;
            }else if(c == '/'){
                dfastate = DIV_ONE;
            }else if(c == '%'){
                dfastate = MOD_ONE;
            }else if(c == '='){
                dfastate = ASSIGN_ONE;
            }else if(c == ','){
                words[++wordsSize].type = SEPERATOR;
                words[wordsSize].tval.sepType = COMMA;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
            }else if(c == ';'){
                words[++wordsSize].type = SEPERATOR;
                words[wordsSize].tval.sepType = SEMICOLON;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
            }else if(c == '('){
                words[++wordsSize].type = SEPERATOR;
                words[wordsSize].tval.sepType = LLB;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
            }else if(c == ')'){
                words[++wordsSize].type = SEPERATOR;
                words[wordsSize].tval.sepType = RLB;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
            }else if(c == '['){
                words[++wordsSize].type = SEPERATOR;
                words[wordsSize].tval.sepType = LMB;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
            }else if(c == ']'){
                words[++wordsSize].type = SEPERATOR;
                words[wordsSize].tval.sepType = RMB;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
            }else if(c == '{'){
                words[++wordsSize].type = SEPERATOR;
                words[wordsSize].tval.sepType = LGB;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
            }else if(c == '}'){
                words[++wordsSize].type = SEPERATOR;
                words[wordsSize].tval.sepType = RGB;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
            }else if(c == '\''){
                dfastate = CHAR_ONE;
            }else{
                tmp[++tmptop] = c;
                lex_error(line, pos);
                exit(0);
            }
            return 0;

        case ID_KEY_ALNUM:
            if(isAlnum(c)){
                tmp[++tmptop] = c;
                dfastate = ID_KEY_ALNUM;
                return 0;
            }else{
                tmp[++tmptop] = '\0';
                Trie p;
                if((p = matchTrie(tmp + 1, root))){
                    words[++wordsSize].type = KEYWORD;
                    words[wordsSize].tval.keyIdx = p->no;
                    words[wordsSize].line = line;
                    words[wordsSize].colomn = pos;
                }else{
                    words[++wordsSize].type = ID;
                    words[wordsSize].tval.name = (char *) malloc(sizeof(char) * tmptop);
                    words[wordsSize].line = line;
                    words[wordsSize].colomn = pos;
                    strcpy(words[wordsSize].tval.name, tmp + 1);
                }
                back2Space();
                return 1;
            }

        case CHAR_ONE:
            words[++wordsSize].type = CNUM;
            words[wordsSize].tval.cvalue = c;
            words[wordsSize].line = line;
            words[wordsSize].colomn = pos;
            dfastate = CHAR_END;
            return 0;

        case CHAR_END:
            if(c == '\''){
                back2Space();
                return 0;
            }else{
                lex_error(line, pos);
                exit(0);
            }

        case INUM_NUM:
            if(isNum(c)){
                tmpIval = (tmpIval << 3) + (tmpIval << 1) + (c - '0');
                dfastate = INUM_NUM;
                return 0;
            }else if(c == '.'){
                dfastate = FNUM_DOT;
                return 0;
            }else{
                words[++wordsSize].type = INUM;
                words[wordsSize].tval.ivalue = tmpIval;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 1;
            }

        case FNUM_DOT:
            if(isNum(c)){
                tmpFval = tmpFval + fw * (c - '0');
                fw = fw * 0.1f;
                dfastate = FNUM_DOT;
                return 0;
            }else{
                words[++wordsSize].type = FNUM;
                words[wordsSize].tval.fvalue = tmpFval + tmpIval;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 1;
            }

        case ADD_ONE:
            if(c == '+'){
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = UNARY_OP;
                words[wordsSize].op = BIADD;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 0;
            }else if(c == '='){
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = MIXASSIGN_OP;
                words[wordsSize].op = ADDEQ;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 0;
            }else{
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = ARITH_OP;
                words[wordsSize].op = ADD;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 1;
            }

        case MINUS_ONE:
            if(c == '-'){
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = UNARY_OP;
                words[wordsSize].op = BIMINUS;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 0;
            }else if(c == '='){
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = MIXASSIGN_OP;
                words[wordsSize].op = MINUSEQ;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 0;
            }else{
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = ARITH_OP;
                words[wordsSize].op = MINUS;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 1;
            }

        case NOT_ONE:
            if(c == '='){
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = RELAT_OP;
                words[wordsSize].op = NOTEQ;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 0;
            }else{
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = UNARY_OP;
                words[wordsSize].op = NOT;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 1;
            }

        case BITNOT_GET:
            words[++wordsSize].type = OPERATER;
            words[wordsSize].tval.opType = UNARY_OP;
            words[wordsSize].op = BITNOT;
            words[wordsSize].line = line;
            words[wordsSize].colomn = pos;
            back2Space();
            return 1;

        case OR_ONE:
            if(c == '|'){
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = LOGIC_OP;
                words[wordsSize].op = BIOR;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 0;
            }else{
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = BIT_OP;
                words[wordsSize].op = OR;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 1;
            }

        case AND_ONE:
            if(c == '&'){
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = LOGIC_OP;
                words[wordsSize].op = BIAND;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 0;
            }else{
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = BIT_OP;
                words[wordsSize].op = AND;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 1;
            }

        case XOR_GET:
            words[++wordsSize].type = OPERATER;
            words[wordsSize].tval.opType = BIT_OP;
            words[wordsSize].op = XOR;
            words[wordsSize].line = line;
            words[wordsSize].colomn = pos;
            back2Space();
            return 1;

        case LSS_ONE:
            if(c == '='){
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = RELAT_OP;
                words[wordsSize].op = LSSEQ;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 0;
            }else if(c == '<'){
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = SHIFT_OP;
                words[wordsSize].op = LSHIFT;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 0;
            }else{
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = RELAT_OP;
                words[wordsSize].op = LSS;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 1;
            }

        case GRT_ONE:
            if(c == '='){
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = RELAT_OP;
                words[wordsSize].op = GRTEQ;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 0;
            }else if(c == '>'){
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = SHIFT_OP;
                words[wordsSize].op = RSHIFT;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 0;
            }else{
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = RELAT_OP;
                words[wordsSize].op = GRT;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 1;
            }
      
        case MUITI_ONE:
            if(c == '='){
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = MIXASSIGN_OP;
                words[wordsSize].op = MULTIEQ;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 0;
            }else{
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = ARITH_OP;
                words[wordsSize].op = MULTI;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 1;
            }

        case DIV_ONE:
            if(c == '='){
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = MIXASSIGN_OP;
                words[wordsSize].op = DIVEQ;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 0;
            }else if(c == '/'){
                dfastate = LINECOMMENT;
                return 0;
            }else if(c == '*'){
                dfastate = BLOCKCOMMENT;
                return 0;
            }else{
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = ARITH_OP;
                words[wordsSize].op = DIV;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 1;
            }

        case LINECOMMENT:
            if(c == '\n'){
                back2Space();
                return 0;
            }else{
                dfastate = LINECOMMENT;
                return 0;
            }

        case BLOCKCOMMENT:
            if(c == '*'){
                dfastate = BLOCKCOMMENTEND;
                return 0;
            }else{
                dfastate = BLOCKCOMMENT;
                return 0;
            }

        case BLOCKCOMMENTEND:
            if(c == '/'){
                back2Space();
                return 0;
            }else{
                dfastate = BLOCKCOMMENT;
                return 0;
            }

        case MOD_ONE:
            if(c == '='){
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = MIXASSIGN_OP;
                words[wordsSize].op = MODEQ;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 0;
            }else{
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = ARITH_OP;
                words[wordsSize].op = MOD;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 1;
            }

        case ASSIGN_ONE:
            if(c == '='){
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = RELAT_OP;
                words[wordsSize].op = EQ;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 0;
            }else{
                words[++wordsSize].type = OPERATER;
                words[wordsSize].tval.opType = ASSIGN_OP;
                words[wordsSize].op = SETVAL;
                words[wordsSize].line = line;
                words[wordsSize].colomn = pos;
                back2Space();
                return 1;
            }

        default:
            lex_error(line, pos);
            exit(0);
            break;    
    }
}

// int main()
// {
//     initTrie();
//     // char cc[100] = "while Rick hits Morty break dollardollar 100 for 1024 plumbus 1.11  == 2.22 ++ ";
//     char cc[100] = "int i, j; int fun(int a, float b){int m;if(a>b) m=a;else m=b;return m;}float x,y; ";
//     for(int i = 0; i < strlen(cc); i++){
//         if(lex(cc[i], 1, i)){
//             lex(cc[i], 1, i);
//         }
//     }
//     for(int i = 1; i <= wordsSize; i++){
//         printWordInfo(words[i]);
//     }
//     return 0;
// }