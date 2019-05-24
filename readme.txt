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
  27: stmtlist ::=.
  28: stmt ::= compstmt.
  29: stmt ::= exp SEMICOLON.
  30: stmt ::= RETURN exp SEMICOLON.
  31: stmt ::= IF LLB exp RLB stmt. [LOWELSE precedence=13]
  32: stmt ::= IF LLB exp RLB stmt ELSE stmt. [ELSE precedence=14]
  33: stmt ::= WHILE LLB exp RLB stmt.
  34: stmt ::= FOR LLB initlist SEMICOLON exp SEMICOLON steplist RLB stmt.
  35: initlist ::= init COMMA initlist. [COMMA precedence=1]
  36: initlist ::= init.
  37: init ::= ID ASSIGN exp. [ASSIGN precedence=2]
  38: init ::= ID ADDEQ exp. [ADDEQ precedence=2]
  39: init ::= ID MINUSEQ exp. [MINUSEQ precedence=2]
  40: steplist ::= step COMMA steplist. [COMMA precedence=1]
  41: steplist ::= step.
  42: step ::= BIADD ID. [BIADD precedence=12]
  43: step ::= ID BIADD. [BIADD precedence=12]
  44: step ::= BIMINUS ID. [BIMINUS precedence=12]
  45: step ::= ID BIMINUS. [BIMINUS precedence=12]
  46: step ::= ID ASSIGN exp. [ASSIGN precedence=2]
  47: exp ::= exp BIAND exp. [BIAND precedence=4]
  48: exp ::= exp BIOR exp. [BIOR precedence=3]
  49: exp ::= exp LSS exp. [LSS precedence=9]
  50: exp ::= exp LSSEQ exp. [LSSEQ precedence=9]
  51: exp ::= exp GRT exp. [GRT precedence=9]
  52: exp ::= exp GRTEQ exp. [GRTEQ precedence=9]
  53: exp ::= exp EQ exp. [EQ precedence=8]
  54: exp ::= exp NOTEQ exp. [NOTEQ precedence=8]
  55: exp ::= exp ADD exp. [ADD precedence=10]
  56: exp ::= exp MINUS exp. [MINUS precedence=10]
  57: exp ::= exp MULTI exp. [MULTI precedence=11]
  58: exp ::= exp DIV exp. [DIV precedence=11]
  59: exp ::= exp MOD exp. [MOD precedence=11]
  60: exp ::= INUM.
  61: exp ::= FNUM.
  62: exp ::= ID.
  63: exp ::= LLB exp RLB.
  64: exp ::= NOT exp. [NOT precedence=12]
  65: exp ::= exp AND exp. [AND precedence=7]
  66: exp ::= exp OR exp. [OR precedence=5]
  67: exp ::= exp XOR exp. [XOR precedence=6]
  68: exp ::= exp BITNOT exp. [BITNOT precedence=12]
  69: exp ::= exp COMMA exp. [COMMA precedence=1]
  70: exp ::= ID LMB exp RMB.
  71: exp ::= exp ASSIGN exp. [ASSIGN precedence=2]
  72: exp ::= exp ADDEQ exp. [ADDEQ precedence=2]
  73: exp ::= exp MINUSEQ exp. [MINUSEQ precedence=2]
  74: exp ::= exp BIADD. [BIADD precedence=12]
  75: exp ::= exp BIMINUS. [BIMINUS precedence=12]
  76: exp ::= BIADD exp. [BIADD precedence=12]
  77: exp ::= BIMINUS exp. [BIMINUS precedence=12]
  78: exp ::= CNUM.
  79: exp ::= ID LLB exp RLB.
  80: exp ::= ID LLB RLB.
3. 语义分析与符号表生成
   0: program ::= deflist.
   1: deflist ::= tp {def.type = tp.type} def deflist.
   2: deflist ::=.
   3: tp ::= INT. {tp.type = INT}
   4: tp ::= FLOAT. {tp.type = FLOAT}
   5: tp ::= CHAR. {tp.type = CHAR}
   6: tp ::= INT INUM. {tp.type = INT ARRAY}
   7: tp ::= FLOAT INUM. {tp.type = FLOAT ARRAY}
   8: tp ::= CHAR INUM. {tp.type = CHAR ARRAY}
   9: vallist ::= {ID.type = vallist.type} ID{check ID.name into table} {vallist.type = vallist.type} vallist. {vallist.num = vallist.num + 1} [COMMA precedence=1] 
  10: vallist ::= {ID.type = vallist.type} ID. {vallist.num = 1; ID into table}
  11: def ::= {vallist.type = def.type} vallist. {def.num = vallist.num}
  12: def ::= {fundef.retype = def.type} fundef compstmt. {check compstmt.retype == def.type}
  13: fundef ::= {} ID args{get linklist of types: args.types}.{fun ID into table}
  14: fundef ::= {} ID.{fun ID into table}
  15: args ::= {param.args = args.types}param{.args linklist add param.type} args.{args.num = args.num + 1}
  16: args ::= {param.args = args.types}param.{.args linklist add param.type; args.num = 1}
  17: param ::= tp ID.{check ID into table. param.type = tp.type, param.name = ID.name}
  18: compstmt ::= innerdeflist stmtlist.{table level--; pop table; compstmt.retype = stmtlist.retype}
  19: innerdeflist ::= innerdef innerdeflist.{innerdeflist.num = innerdef.num + innerdeflist.num}
  20: innerdeflist ::=.
  21: innerdef ::= tp {innerdeclist.type = tp.type} innerdeclist. {innerdef.num = innerdeclist.num}
  22: innerdeclist ::= {innerdec.type = innerdeclist.type} innerdec {innerdeclist.type = innerdeclist.type} innerdeclist. {innerdeclist.num = innerdeclist.num + 1} [COMMA precedence=1]
  23: innerdeclist ::= {innerdec.type = innerdeclist.type} innerdec.{innerdeclist.num = 1}
  24: innerdec ::= {ID.type = innerdec.type} ID ASSIGN exp. {check exp.type == innerdec.type; check ID into table} [ASSIGN precedence=2]
  25: innerdec ::= {ID.type = innerdec.type} ID. {check ID into table}
  26: stmtlist ::= stmt stmtlist. {check; stmtlist.retype = stmt.retype || stmtlist.retype}
  27: stmtlist ::=.{stmtlist.retype = void}
  28: stmt ::= compstmt.{stmt.retype = compstmt.retype}
  29: stmt ::= exp.
  30: stmt ::= RETURN exp. {stmt.retype = exp.type}
  31: stmt ::= IF exp {check exp.type == BOOLEXP} stmt. {stmt.retype = stmt.retype} [LOWELSE precedence=13]

  32: stmt ::= IF exp {check exp.type == BOOLEXP} stmt ELSE stmt. {check return; stmt.retype} [ELSE precedence=14]

  33: stmt ::= WHILE exp {check exp.type == BOOLEXP} stmt {stmt.retype = stmt.retype}.
  
  34: stmt ::= FOR initlist exp {check exp.type == BOOLEXP} steplist stmt. {stmt.retype = stmt.retype}

  35: initlist ::= init COMMA initlist. [COMMA precedence=1]
  36: initlist ::= init.
  37: init ::= ID {check ID reference} ASSIGN exp {check ID.type == exp.type}. [ASSIGN precedence=2]
  38: init ::= ID {check ID reference} ADDEQ exp {exp.type == ID.type && (ID.type == INT || ID.type == FLOAT)}. [ADDEQ precedence=2]
  39: init ::= ID {check ID reference} MINUSEQ exp {exp.type == ID.type && (ID.type == INT || ID.type == FLOAT)}. [MINUSEQ precedence=2]
  40: steplist ::= step steplist. [COMMA precedence=1]
  41: steplist ::= step.
  42: step ::= BIADD ID {check ID reference}. [BIADD precedence=12]
  43: step ::= ID {check ID reference} BIADD. [BIADD precedence=12]
  44: step ::= BIMINUS ID {check ID reference}. [BIMINUS precedence=12]
  45: step ::= ID {check ID reference} BIMINUS. [BIMINUS precedence=12]
  46: step ::= ID {check ID reference} ASSIGN exp {check ID.type == exp.type}. [ASSIGN precedence=2]
  47: exp ::= exp {check exp.type == BOOLEXP} BIAND exp {check exp.type == BOOLEXP; exp.type == BOOLEXP}. [BIAND precedence=4]
  48: exp ::= exp {check exp.type == BOOLEXP} BIOR exp {check exp.type == BOOLEXP; exp.type == BOOLEXP}. [BIOR precedence=3]
  49: exp ::= exp LSS exp. [LSS precedence=9] (the same as above)
  50: exp ::= exp LSSEQ exp. [LSSEQ precedence=9] (the same as above)
  51: exp ::= exp GRT exp. [GRT precedence=9] (the same as above)
  52: exp ::= exp GRTEQ exp. [GRTEQ precedence=9] (the same as above)
  53: exp ::= exp EQ exp. [EQ precedence=8] (the same as above)
  54: exp ::= exp NOTEQ exp. [NOTEQ precedence=8] (the same as above)
  55: exp ::= exp ADD exp {check exp1.type == exp2.type && exp1.type != BOOLEXP; exp.type = exp1.type}. [ADD precedence=10]
  56: exp ::= exp MINUS exp {check exp1.type == exp2.type && exp1.type != BOOLEXP; exp.type = exp1.type}. [MINUS precedence=10]
  57: exp ::= exp MULTI exp {check exp1.type == exp2.type && exp1.type != BOOLEXP; exp.type = exp1.type}. [MULTI precedence=11]
  58: exp ::= exp DIV exp {check exp1.type == exp2.type && exp1.type != BOOLEXP; exp.type = exp1.type}. [DIV precedence=11]
  59: exp ::= exp MOD exp {check exp1.type == exp2.type && exp1.type != BOOLEXP; exp.type = exp1.type}. [MOD precedence=11]
  60: exp ::= INUM. {exp.type = INTEXP}
  61: exp ::= FNUM. {exp.type = FLOATEXP}
  62: exp ::= ID. {exp.type = ID.type; exp.canLeft = 1}
  63: exp ::= exp. {exp.type = exp.type; exp.canLeft = exp.canLeft}
  64: exp ::= NOT exp {check exp.type = BOOLEXP}. [NOT precedence=12]
  65: exp ::= exp {check exp.type == INTEXP} AND exp {check exp.type == INTEXP; exp.type = INTEXP}. [AND precedence=7]
  66: exp ::= exp OR exp. [OR precedence=5] (the same as above)
  67: exp ::= exp XOR exp. [XOR precedence=6] (the same as above)
  68: exp ::= exp BITNOT exp. [BITNOT precedence=12] (the same as above)
  69: exp ::= exp exp. {exp.type = UNDEFINEXP} [COMMA precedence=1]
  70: exp ::= ID {check ID reference} exp {check exp.type = INTEXP}. {exp.type = ID.type - ARRAY; exp.canLeft = 1}
  71: exp ::= exp {check exp.canLeft == 1} ASSIGN exp {check exp2.type == exp1.type; exp.type = exp1.type}. {check exp.type} [ASSIGN precedence=2]
  72: exp ::= exp {check exp.type != BOOLEXP && exp.canLeft == 1} ADDEQ exp {check exp.type != BOOLEXP && exp1.type == exp2.type; exp.type = exp1.type}. [ADDEQ precedence=2]
  73: exp ::= exp MINUSEQ exp. [MINUSEQ precedence=2] (the same as above)
  74: exp ::= exp {check exp.canLeft == 1 exp.type == INTEXP} BIADD {exp.type = exp.type}. [BIADD precedence=12]
  75: exp ::= exp BIMINUS. [BIMINUS precedence=12]
  76: exp ::= BIADD exp. [BIADD precedence=12]
  77: exp ::= BIMINUS exp. [BIMINUS precedence=12]
  78: exp ::= CNUM {exp.type = CHAREXP}.
  79: exp ::= ID {check ID references Function} {exp.types = exp.types} exp{check types} {exp.type = ID.type}.
  80: exp :: ID LLB RLB.

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
  27: stmtlist ::=.
  28: stmt ::= compstmt.
  29: stmt ::= exp SEMICOLON.
  30: stmt ::= RETURN exp SEMICOLON.
  31: stmt ::= IF LLB exp RLB stmt. [LOWELSE precedence=13]
  32: stmt ::= IF LLB exp RLB stmt ELSE stmt. [ELSE precedence=14]
  33: stmt ::= WHILE LLB exp RLB stmt.
  34: stmt ::= FOR LLB initlist SEMICOLON exp SEMICOLON steplist RLB stmt.
  35: initlist ::= init COMMA initlist. [COMMA precedence=1]
  36: initlist ::= init.
  37: init ::= ID ASSIGN exp. [ASSIGN precedence=2]
  38: init ::= ID ADDEQ exp. [ADDEQ precedence=2]
  39: init ::= ID MINUSEQ exp. [MINUSEQ precedence=2]
  40: steplist ::= step COMMA steplist. [COMMA precedence=1]
  41: steplist ::= step.
  42: step ::= BIADD ID. [BIADD precedence=12]
  43: step ::= ID BIADD. [BIADD precedence=12]
  44: step ::= BIMINUS ID. [BIMINUS precedence=12]
  45: step ::= ID BIMINUS. [BIMINUS precedence=12]
  46: step ::= ID ASSIGN exp. [ASSIGN precedence=2]
  47: exp ::= exp BIAND exp. [BIAND precedence=4]
  48: exp ::= exp BIOR exp. [BIOR precedence=3]
  49: exp ::= exp LSS exp. [LSS precedence=9]
  50: exp ::= exp LSSEQ exp. [LSSEQ precedence=9]
  51: exp ::= exp GRT exp. [GRT precedence=9]
  52: exp ::= exp GRTEQ exp. [GRTEQ precedence=9]
  53: exp ::= exp EQ exp. [EQ precedence=8]
  54: exp ::= exp NOTEQ exp. [NOTEQ precedence=8]
  55: exp ::= exp ADD exp. [ADD precedence=10]
  56: exp ::= exp MINUS exp. [MINUS precedence=10]
  57: exp ::= exp MULTI exp. [MULTI precedence=11]
  58: exp ::= exp DIV exp. [DIV precedence=11]
  59: exp ::= exp MOD exp. [MOD precedence=11]
  60: exp ::= INUM.
  61: exp ::= FNUM.
  62: exp ::= ID.
  63: exp ::= LLB exp RLB.
  64: exp ::= NOT exp. [NOT precedence=12]
  65: exp ::= exp AND exp. [AND precedence=7]
  66: exp ::= exp OR exp. [OR precedence=5]
  67: exp ::= exp XOR exp. [XOR precedence=6]
  68: exp ::= exp BITNOT exp. [BITNOT precedence=12]
  69: exp ::= exp COMMA exp. [COMMA precedence=1]
  70: exp ::= ID LMB exp RMB.
  71: exp ::= exp ASSIGN exp. [ASSIGN precedence=2]
  72: exp ::= exp ADDEQ exp. [ADDEQ precedence=2]
  73: exp ::= exp MINUSEQ exp. [MINUSEQ precedence=2]
  74: exp ::= exp BIADD. [BIADD precedence=12]
  75: exp ::= exp BIMINUS. [BIMINUS precedence=12]
  76: exp ::= BIADD exp. [BIADD precedence=12]
  77: exp ::= BIMINUS exp. [BIMINUS precedence=12]
  78: exp ::= CNUM.
  79: exp ::= ID LLB exp RLB.
  80: exp ::= ID LLB RLB.