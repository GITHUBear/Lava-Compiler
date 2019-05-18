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
(0) program -> deflist.
(1) deflist -> tp def deflist.
(2) deflist ->.
(3) tp -> INT.
(4) tp -> FLOAT.
(5) tp -> INT LMB INUM RMB.
(6) tp -> FLOAT LMB INUM RMB.
(7) vallist -> ID COMMA vallist.
(8) vallist -> ID.
(9) def -> vallist SEMICOLOM.
(10) def -> fundef LGB stmtlist RGB.
fundef -> ID LLB args RLB.
fundef -> ID LLB RLB.
args -> param COMMA args.
args -> param.
param -> tp ID.
stmtlist -> stmt stmtlist.
stmtlist ->.
stmt -> exp SEMICOLOM.
stmt -> tp vallist SEMICOLOM.
stmt -> RETURN exp SEMICOLOM.
stmt -> LGB stmt RGB.
stmt -> IF LLB exp RLB stmt.
stmt -> IF LLB exp RLB stmt1 ELSE stmt.
stmt -> WHILE LLB exp RLB stmt.
stmt -> FOR LLB initlist SEMICOLOM exp SEMICOLOM steplist RLB stmt.
initlist -> init COMMA initlist.
initlist -> init.
init -> ID ASSIGN exp.
init -> ID ADDEQ exp.
init -> ID MINUSEQ exp.
steplist -> step COMMA steplist.
steplist -> step.
step -> BIADD ID.
step -> ID BIADD.
step -> BIMINUS ID.
step -> ID BIMINUS.
step -> ID ASSIGN exp.
stmt1 -> IF LLB exp RLB stmt1 ELSE stmt1.
stmt1 -> WHILE LLB exp RLB stmt1.
stmt1 -> FOR LLB initlist SEMICOLOM exp SEMICOLOM steplist RLB stmt1.
exp -> exp BIAND exp.
exp -> exp BIOR exp.
exp -> exp LSS exp.
exp -> exp LSSEQ exp.
exp -> exp GRT exp.
exp -> exp GRTEQ exp.
exp -> exp EQ exp.
exp -> exp NOTEQ exp.
exp -> exp ADD exp.
exp -> exp MINUS exp.
exp -> exp MULTI exp.
exp -> exp DIV exp.
exp -> exp MOD exp.
exp -> INUM.
exp -> FNUM.
exp -> ID.
exp -> LLB exp RLB.
exp -> NOT exp.
exp -> exp AND exp.
exp -> exp OR exp.
exp -> exp XOR exp.
exp -> exp BITNOT exp.
exp -> exp COMMA exp.
exp -> ID LMB exp RMB.
exp -> exp ASSIGN exp.
