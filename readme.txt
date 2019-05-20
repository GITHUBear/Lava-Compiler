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
Rules:
   0: program ::= deflist.
   1: deflist ::= tp def deflist.
   2: deflist ::=.
   3: tp ::= INT.
   4: tp ::= FLOAT.
   5: tp ::= INT LMB INUM RMB.
   6: tp ::= FLOAT LMB INUM RMB.
   7: vallist ::= ID COMMA vallist. [COMMA precedence=1]
   8: vallist ::= ID.
   9: def ::= vallist SEMICOLON.
  10: def ::= fundef compstmt.
  11: fundef ::= ID LLB args RLB.
  12: fundef ::= ID LLB RLB.
  13: args ::= param COMMA args. [COMMA precedence=1]
  14: args ::= param.
  15: param ::= tp ID.
  16: compstmt ::= LGB innerdeflist stmtlist RGB.
  17: innerdeflist ::= innerdef innerdeflist.
  18: innerdeflist ::=.
  19: innerdef ::= tp innerdeclist SEMICOLON.
  20: innerdeclist ::= innerdec COMMA innerdeclist. [COMMA precedence=1]
  21: innerdeclist ::= innerdec.
  22: innerdec ::= ID ASSIGN exp. [ASSIGN precedence=2]
  23: innerdec ::= ID.
  24: stmtlist ::= stmt stmtlist.
  25: stmtlist ::= LGB stmtlist RGB stmtlist.
  26: stmtlist ::=.
  27: stmt ::= exp SEMICOLON.
  28: stmt ::= RETURN exp SEMICOLON.
  29: stmt ::= IF LLB exp RLB stmt. [LOWELSE precedence=13]
  30: stmt ::= IF LLB exp RLB LGB stmtlist RGB. [LOWELSE precedence=13]
  31: stmt ::= IF LLB exp RLB stmt ELSE stmt. [ELSE precedence=14]
  32: stmt ::= IF LLB exp RLB stmt ELSE LGB stmtlist RGB. [ELSE precedence=14]
  33: stmt ::= IF LLB exp RLB LGB stmtlist RGB ELSE LGB stmtlist RGB. [ELSE precedence=14]
  34: stmt ::= IF LLB exp RLB LGB stmtlist RGB ELSE stmt. [ELSE precedence=14]
  35: stmt ::= WHILE LLB exp RLB stmt.
  36: stmt ::= WHILE LLB exp RLB LGB stmtlist RGB.
  37: stmt ::= FOR LLB initlist SEMICOLON exp SEMICOLON steplist RLB stmt.
  38: stmt ::= FOR LLB initlist SEMICOLON exp SEMICOLON steplist RLB LGB stmtlist RGB.
  39: initlist ::= init COMMA initlist. [COMMA precedence=1]
  40: initlist ::= init.
  41: init ::= ID ASSIGN exp. [ASSIGN precedence=2]
  42: init ::= ID ADDEQ exp. [ADDEQ precedence=2]
  43: init ::= ID MINUSEQ exp. [MINUSEQ precedence=2]
  44: steplist ::= step COMMA steplist. [COMMA precedence=1]
  45: steplist ::= step.
  46: step ::= BIADD ID. [BIADD precedence=12]
  47: step ::= ID BIADD. [BIADD precedence=12]
  48: step ::= BIMINUS ID. [BIMINUS precedence=12]
  49: step ::= ID BIMINUS. [BIMINUS precedence=12]
  50: step ::= ID ASSIGN exp. [ASSIGN precedence=2]
  51: exp ::= exp BIAND exp. [BIAND precedence=4]
  52: exp ::= exp BIOR exp. [BIOR precedence=3]
  53: exp ::= exp LSS exp. [LSS precedence=9]
  54: exp ::= exp LSSEQ exp. [LSSEQ precedence=9]
  55: exp ::= exp GRT exp. [GRT precedence=9]
  56: exp ::= exp GRTEQ exp. [GRTEQ precedence=9]
  57: exp ::= exp EQ exp. [EQ precedence=8]
  58: exp ::= exp NOTEQ exp. [NOTEQ precedence=8]
  59: exp ::= exp ADD exp. [ADD precedence=10]
  60: exp ::= exp MINUS exp. [MINUS precedence=10]
  61: exp ::= exp MULTI exp. [MULTI precedence=11]
  62: exp ::= exp DIV exp. [DIV precedence=11]
  63: exp ::= exp MOD exp. [MOD precedence=11]
  64: exp ::= INUM.
  65: exp ::= FNUM.
  66: exp ::= ID.
  67: exp ::= LLB exp RLB.
  68: exp ::= NOT exp. [NOT precedence=12]
  69: exp ::= exp AND exp. [AND precedence=7]
  70: exp ::= exp OR exp. [OR precedence=5]
  71: exp ::= exp XOR exp. [XOR precedence=6]
  72: exp ::= exp BITNOT exp. [BITNOT precedence=12]
  73: exp ::= exp COMMA exp. [COMMA precedence=1]
  74: exp ::= ID LMB exp RMB.
  75: exp ::= exp ASSIGN exp. [ASSIGN precedence=2]
  76: exp ::= exp ADDEQ exp. [ADDEQ precedence=2]
  77: exp ::= exp MINUSEQ exp. [MINUSEQ precedence=2]
  78: exp ::= exp BIADD. [BIADD precedence=12]
  79: exp ::= exp BIMINUS. [BIMINUS precedence=12]
  80: exp ::= BIADD exp. [BIADD precedence=12]
  81: exp ::= BIMINUS exp. [BIMINUS precedence=12]