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
    for(int i = 0; i <= 156; i++){
        scanf("%s %d %s", A, &num, B);
        printf("case %d:\n", num);
        scanf("%d", &num);
        for(int j = 1; j <= num; j++){
            scanf("%s %s", A, B);
            if(A[0] == '#'){
                printf("\tif(synid == END){\n");
            }else if(A[0] == '['){
                scanf("%d", &tmp);
                if(B[0] == 'r'){
                    printf("\treturn reduce(%d);\n", tmp);
                }else if(B[0] == 's'){
                    printf("\treturn shift(%d, stc);\n", tmp);
                }
                break;
            }else{
                printf("\tif(synid == %s){\n", A);
            }
            if(B[0] == 'a'){
                printf("\t\tfin = 1;\n\t\treturn NULL;\n\t}\n");
            }else{
                scanf("%d", &tmp);
                if(B[0] == 'r'){
                    printf("\t\treturn reduce(%d);\n\t}\n", tmp);
                }else if(B[0] == 's'){
                    printf("\t\treturn shift(%d, stc);\n\t}\n", tmp);
                }
            }
        }
        printf("\texit(0);\n");
    }
    return 0;
}