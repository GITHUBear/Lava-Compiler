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
　　　for loop, while loop
    判断语句: if, else
    数据类型: char, int, float, 一维数组

1. 词法分析器
2. 语法分析器
Rules:
   0: program ::= deflist.
   1: deflist ::= tp def deflist.
   2: deflist ::=.
   3: tp ::= INT.
   4: tp ::= FLOAT.
   5: tp ::= CHAR.
   6: tp ::= INT LMB INUM RMB.
   7: tp ::= FLOAT LMB INUM RMB.
   8: tp ::= CHAR LMB INUM RMB.
   9: vallist ::= ID COMMA vallist. [COMMA precedence=1]
  10: vallist ::= ID.
  11: def ::= vallist SEMICOLON.
  12: def ::= fundef compstmt.
  13: fundef ::= ID LLB args RLB.
  14: fundef ::= ID LLB RLB.
  15: args ::= param COMMA args. [COMMA precedence=1]
  16: args ::= param.
  17: param ::= tp ID.
  18: compstmt ::= LGB innerdeflist stmtlist RGB.
  19: innerdeflist ::= innerdef innerdeflist.
  20: innerdeflist ::=.
  21: innerdef ::= tp innerdeclist SEMICOLON.
  22: innerdeclist ::= innerdec COMMA innerdeclist. [COMMA precedence=1]
  23: innerdeclist ::= innerdec.
  24: innerdec ::= ID ASSIGN exp. [ASSIGN precedence=2]
  25: innerdec ::= ID.
  26: stmtlist ::= stmt stmtlist.
  27: stmtlist ::= LGB stmtlist RGB stmtlist.
  28: stmtlist ::=.
  29: stmt ::= exp SEMICOLON.
  30: stmt ::= RETURN exp SEMICOLON.
  31: stmt ::= IF LLB exp RLB stmt. [LOWELSE precedence=13]
  32: stmt ::= IF LLB exp RLB LGB stmtlist RGB. [LOWELSE precedence=13]
  33: stmt ::= IF LLB exp RLB stmt ELSE stmt. [ELSE precedence=14]
  34: stmt ::= IF LLB exp RLB stmt ELSE LGB stmtlist RGB. [ELSE precedence=14]
  35: stmt ::= IF LLB exp RLB LGB stmtlist RGB ELSE LGB stmtlist RGB. [ELSE precedence=14]
  36: stmt ::= IF LLB exp RLB LGB stmtlist RGB ELSE stmt. [ELSE precedence=14]
  37: stmt ::= WHILE LLB exp RLB stmt.
  38: stmt ::= WHILE LLB exp RLB LGB stmtlist RGB.
  39: stmt ::= FOR LLB initlist SEMICOLON exp SEMICOLON steplist RLB stmt.
  40: stmt ::= FOR LLB initlist SEMICOLON exp SEMICOLON steplist RLB LGB stmtlist RGB.
  41: initlist ::= init COMMA initlist. [COMMA precedence=1]
  42: initlist ::= init.
  43: init ::= ID ASSIGN exp. [ASSIGN precedence=2]
  44: init ::= ID ADDEQ exp. [ADDEQ precedence=2]
  45: init ::= ID MINUSEQ exp. [MINUSEQ precedence=2]
  46: steplist ::= step COMMA steplist. [COMMA precedence=1]
  47: steplist ::= step.
  48: step ::= BIADD ID. [BIADD precedence=12]
  49: step ::= ID BIADD. [BIADD precedence=12]
  50: step ::= BIMINUS ID. [BIMINUS precedence=12]
  51: step ::= ID BIMINUS. [BIMINUS precedence=12]
  52: step ::= ID ASSIGN exp. [ASSIGN precedence=2]
  53: exp ::= exp BIAND exp. [BIAND precedence=4]
  54: exp ::= exp BIOR exp. [BIOR precedence=3]
  55: exp ::= exp LSS exp. [LSS precedence=9]
  56: exp ::= exp LSSEQ exp. [LSSEQ precedence=9]
  57: exp ::= exp GRT exp. [GRT precedence=9]
  58: exp ::= exp GRTEQ exp. [GRTEQ precedence=9]
  59: exp ::= exp EQ exp. [EQ precedence=8]
  60: exp ::= exp NOTEQ exp. [NOTEQ precedence=8]
  61: exp ::= exp ADD exp. [ADD precedence=10]
  62: exp ::= exp MINUS exp. [MINUS precedence=10]
  63: exp ::= exp MULTI exp. [MULTI precedence=11]
  64: exp ::= exp DIV exp. [DIV precedence=11]
  65: exp ::= exp MOD exp. [MOD precedence=11]
  66: exp ::= INUM.
  67: exp ::= FNUM.
  68: exp ::= ID.
  69: exp ::= LLB exp RLB.
  70: exp ::= NOT exp. [NOT precedence=12]
  71: exp ::= exp AND exp. [AND precedence=7]
  72: exp ::= exp OR exp. [OR precedence=5]
  73: exp ::= exp XOR exp. [XOR precedence=6]
  74: exp ::= exp BITNOT exp. [BITNOT precedence=12]
  75: exp ::= exp COMMA exp. [COMMA precedence=1]
  76: exp ::= ID LMB exp RMB.
  77: exp ::= exp ASSIGN exp. [ASSIGN precedence=2]
  78: exp ::= exp ADDEQ exp. [ADDEQ precedence=2]
  79: exp ::= exp MINUSEQ exp. [MINUSEQ precedence=2]
  80: exp ::= exp BIADD. [BIADD precedence=12]
  81: exp ::= exp BIMINUS. [BIMINUS precedence=12]
  82: exp ::= BIADD exp. [BIADD precedence=12]
  83: exp ::= BIMINUS exp. [BIMINUS precedence=12]
  84: exp ::= CNUM.