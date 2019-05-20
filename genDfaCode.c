#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char A[100];
char B[100];
int num;
int tmp;

int main(){
    freopen("./gramtmp.txt", "r", stdin);
    freopen("./dfaCode.txt", "w", stdout);
    for(int i = 0; i <= 136; i++){
        scanf("%s %d %s", A, &num, B);
        printf("case %d:\n", num);
        scanf("%d", &num);
        int hasDefault = 0;
        for(int j = 1; j <= num; j++){
            scanf("%s %s", A, B);
            if(A[0] == '$'){
                printf("\tif(synid == END){\n");
            }else if(A[0] == '{'){
                hasDefault = 1;
                scanf("%d", &tmp);
                if(B[0] == 'r'){
                    printf("\treturn reduce(%d);\n", tmp);
                }else if(B[0] == 's'){
                    printf("\treturn shift(%d, stc, canGo);\n", tmp);
                }
                break;
            }else{
                printf("\tif(synid == %s){\n", A);
            }
            if(B[0] == 'a'){
                printf("\t\tfin = 1;\n\t\treturn NULL;\n\t}\n");
            }else{
                scanf("%d", &tmp);
                if(strlen(B) >= 7){
                    printf("\t\tshift(0, stc, canGo);\n\t\treturn reduce(%d);\n\t}\n", tmp);
                }else if(B[0] == 'r'){
                    printf("\t\treturn reduce(%d);\n\t}\n", tmp);
                }else if(B[0] == 's'){
                    printf("\t\treturn shift(%d, stc, canGo);\n\t}\n", tmp);
                }
            }
        }
        if(!hasDefault)
            printf("\texit(0);\n");
    }
    return 0;
}