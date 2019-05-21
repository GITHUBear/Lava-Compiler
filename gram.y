/* simple calculator */
%left COMMA.
%right ASSIGN ADDEQ MINUSEQ.
%left BIOR.
%left BIAND.
%left OR.
%left XOR.
%left AND.
%left EQ NOTEQ.
%left LSS LSSEQ GRT GRTEQ.
%left ADD MINUS.
%left DIV MOD MULTI.
%right BIADD BIMINUS NOT BITNOT.
%nonassoc LOWELSE.
%nonassoc ELSE.

program ::= deflist.{}
deflist ::= tp def deflist.{}
deflist ::= .{}
tp ::= INT.{}
tp ::= FLOAT.{}
tp ::= CHAR.{}
tp ::= INT LMB INUM RMB.{}
tp ::= FLOAT LMB INUM RMB.{}
tp ::= CHAR LMB INUM RMB.{}
vallist ::= ID COMMA vallist.{}
vallist ::= ID.{}
def ::= vallist SEMICOLON.{}
def ::= fundef compstmt.{}
fundef ::= ID LLB args RLB.{}
fundef ::= ID LLB RLB.{}
args ::= param COMMA args.{}
args ::= param.{}
param ::= tp ID.{}
compstmt ::= LGB innerdeflist stmtlist RGB.{}
innerdeflist ::= innerdef innerdeflist.{}
innerdeflist ::= .{}
innerdef ::= tp innerdeclist SEMICOLON.{}
innerdeclist ::= innerdec COMMA innerdeclist.{}
innerdeclist ::= innerdec.{}
innerdec ::= ID ASSIGN exp.{}
innerdec ::= ID.{}
stmtlist ::= stmt stmtlist.{}
stmtlist ::= LGB stmtlist RGB stmtlist.{}
stmtlist ::= .{}
stmt ::= exp SEMICOLON.{}
stmt ::= RETURN exp SEMICOLON.{}
stmt ::= IF LLB exp RLB stmt.[LOWELSE]{}
stmt ::= IF LLB exp RLB LGB stmtlist RGB.[LOWELSE]{}
stmt ::= IF LLB exp RLB stmt ELSE stmt.{}
stmt ::= IF LLB exp RLB stmt ELSE LGB stmtlist RGB.{}
stmt ::= IF LLB exp RLB LGB stmtlist RGB ELSE LGB stmtlist RGB.{}
stmt ::= IF LLB exp RLB LGB stmtlist RGB ELSE stmt.{}
stmt ::= WHILE LLB exp RLB stmt.{}
stmt ::= WHILE LLB exp RLB LGB stmtlist RGB.{}
stmt ::= FOR LLB initlist SEMICOLON exp SEMICOLON steplist RLB stmt.{}
stmt ::= FOR LLB initlist SEMICOLON exp SEMICOLON steplist RLB LGB stmtlist RGB.{}
initlist ::= init COMMA initlist.{}
initlist ::= init.{}
init ::= ID ASSIGN exp.{}
init ::= ID ADDEQ exp.{}
init ::= ID MINUSEQ exp.{}
steplist ::= step COMMA steplist.{}
steplist ::= step.{}
step ::= BIADD ID.{}
step ::= ID BIADD.{}
step ::= BIMINUS ID.{}
step ::= ID BIMINUS.{}
step ::= ID ASSIGN exp.{}

exp ::= exp BIAND exp. {
   
}

exp ::= exp BIOR exp. {
    
}

exp ::= exp LSS exp. {
    
}

exp ::= exp LSSEQ exp. {
    
}

exp ::= exp GRT exp. {
    
}

exp ::= exp GRTEQ exp. {
    
}

exp ::= exp EQ exp. {
    
}

exp ::= exp NOTEQ exp. {
    
}

exp ::= exp ADD exp. {
    
}

exp ::= exp MINUS exp. {
    
}

exp ::= exp MULTI exp. {
    
}

exp ::= exp DIV exp. {
    
}

exp ::= exp MOD exp. {
    
}

exp ::= INUM. {
    
}

exp ::= FNUM. {
    
}

exp ::= ID.{
    
}

exp ::= LLB exp RLB. {
    
}

exp ::= NOT exp.{
    
}

exp ::= exp AND exp.{
    
}

exp ::= exp OR exp.{
    
}

exp ::= exp XOR exp.{
    
}

exp ::= exp BITNOT exp.{
    
}

exp ::= exp COMMA exp.{
    
}

exp ::= ID LMB exp RMB.{
    
}

exp ::= exp ASSIGN exp.{
    
} 


exp ::= exp ADDEQ exp.{
    
}

exp ::= exp MINUSEQ exp.{
    
}

exp ::= exp BIADD.{
    
}

exp ::= exp BIMINUS.{
    
}

exp ::= BIADD exp.{
    
}

exp ::= BIMINUS exp.{
    
}

exp ::= CNUM.{
    
}

exp ::= ID LLB exp RLB.{
    
}

%code{
    
}