#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char lines[100][500];
char tmplines[500];
char a[12];
int lineno = 0;

int main(){
    freopen("./gram.out", "r", stdin);
    FILE* fout = fopen("./gramtmp.txt", "w");
    int i = 1;
    while(i <= 4092){
        lineno = 0;
        fgets(lines[lineno], 500, stdin);
        lineno++; i++;
        do{
            fgets(tmplines, 500, stdin);
            // printf("%d: %d %s\n", i, strlen(tmplines), tmplines);
            i++;
        }while(strlen(tmplines) != 1);
        do{
            fgets(lines[lineno], 500, stdin);
            // printf("%d: %d %s\n", i, strlen(lines[lineno]), lines[lineno]);
            lineno++; i++;
        }while(strlen(lines[lineno - 1]) != 1);
        fputs(lines[0], fout);
        sprintf(a, "%d", lineno - 2);
        fputs(a, fout);
        for(int j = 1; j < lineno; j++){
            fputs(lines[j], fout);
        }
    }
    fclose(fout);
    return 0;
}