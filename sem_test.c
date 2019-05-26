int b;
int[10] barray;
int global;
// int[12] b;                    // 1. variable redefined

int a(int a, int b){
    if(a > b) return a;
    else return b;
}

int max3(int a, int b, int c){
    return a(a, a(b, c));
}

int c(int a){
    return a;
}

/* int c2(int a, int a){              // 9. args redefined
    return a;
} */

/* int max3(int a, int b, int c){    // 8. function redefined
    return a;
} */

/* float testFloat(int a, int b, int c){   // 11. function return type declaration doesn't match 'return' exp;
    return max3(a, b, c);
}*/

int main(){
    // int res1 = c(a(1) + 1);  // 2. args number not match 
    // int res2 = c(a(1,2.0)+1);   // 3. args type not match
    int res2 = c(a(1,2)+1);
    int i;
    {
        int inner = 1;
        float finner = 1.0;
        {
            int inner = 2;
            int inner2 = 3;
            // s++;              // 4. var not defined
        }
        if(res2 > inner){
           // finner++;       // 5. float type 'finner' try to use INT operator '++'         
        }
    }
    // dump(res2);          // 6. call an undefined function
    // res2(res2);            // 7. call a variable
    for(i = 0; i <= 9; i++){
        barray[i] = i;
        if(barray[i] > 4){
            barray[i] -= 2;
            // (barray[i] + res2)++;    // 10. not a left value
        }
    }
    if(res2 > i)
        return 1;
    // else
    //  return 0.0;                  // 12. in function, the return type conflicts 
}