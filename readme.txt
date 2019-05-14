Feature:
    operator:
            1. 一元运算:  ++, --, !, ~    ++,--应前后缀
            2. 逻辑运算: ||, &&
            3. 位运算符: &, |, ^, <<, >>
            4. 算术运算: +, -, *, /, %
            5.关系运算: >, >=, <, <=, ==, !=
            6. 复合赋值运算: +=, -=, *=, /=, %=
            ７.赋值运算: =
     控制流: 
　　　for loop, while loop, do-while loop
    判断语句: if, else
    数据类型: bool, char, int, float, 一维数组

1. 词法分析器
2. 语法分析器
Program -> Def int main() { Stmt }












Program -> Type main() Block
Type -> int | bool
Block -> { Stmts return Num ; }
Decl -> Type Array ;
Array -> Identifier [ Num ] | Identifier [ Identifier ] | Identifier
Stmts -> Stmts M Stmt | Stmt

Letter -> A | B | C | D | E | F | G | H | I | J | K | L | M | N 
    | O | P | Q | R | S | T | U | V | W | X | Y | Z | a | b
    | c | d | e | f | g | h | i | j | k | l | m | n | o | p
    | q | r | s | t | u | v | w | x | y | z | _
Digit -> 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
Non_digit -> 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
Num -> Num Digit | Non_digit
Identifier -> Identifier Digit | Identifier Letter | Letter
Bool_value -> true | false

Self_op -> ++ | --
HLogic_op -> &&
LLogic_op -> ||
HMath_op -> * | / | %
LMath_op -> + | - 
Judge_op -> == | != | >= | <= | > | <

Stmt -> Assignment ;
    | Decl
    | if ( Bool ) M Stmt
    | if ( Bool ) M Stmt N else M Stmt
    | while M ( Bool ) M Stmt
    | for ( Fora ; M Forb ; Forc ) M Stmt
    | { Stmts }
Fora -> Assignment | ε
Forb -> Bool | ε
Forc -> ForAssignment | ε

Assignment -> LArray = Bool ; | LArray Self_op ; | Self_op LArray ; 

ForAssignment -> LArray = Bool ; | LArray Self_op ; | Self_op LArray ; 

Factor -> ( Bool ) | Array | Num | Bool_value | ! ( Bool )
HExpr -> HExpr HMath_op Factor | Factor
LExpr -> LExpr LMath_op HExpr | HExpr
Rel -> Rel Judge_op LExpr | LExpr
HRel -> HRel HLogic_op M Rel | Rel
Bool -> Bool LLogic_op M HRel | HRel

M -> ε 
N -> ε  

