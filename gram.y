/* simple calculator */

%include{
using System.IO;
using System.Collections.Generic;
//#define DEBUG
}

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

%syntax_error{
     Console.WriteLine("Syntax Error! \n By Shift " + @@ + " value(" + $$ + ")");
}

%parse_failed{
    Console.WriteLine("Parse Failed!");
    Environment.Exit(0);
}

program -> deflist.{
    
}

deflist -> tp def deflist. {
    
}

deflist -> .{
    
}

tp -> INT. {
    
}

tp -> FLOAT. {
    
}

tp -> INT LMB INUM RMB.{
    
}

tp -> FLOAT LMB INUM RMB.{
    
}

vallist -> ID COMMA vallist.{
    
}

vallist -> ID.{
    
}

def -> vallist SEMICOLON.{
    
}

def -> fundef LGB stmtlist RGB. {
    
}

fundef -> ID LLB args RLB.{
    
}

fundef -> ID LLB RLB.{
    
}

args -> param COMMA args. {
    
}

args -> param.{
    
}

param -> tp ID.{
    
}

stmtlist -> stmt stmtlist.{
    
}

stmtlist -> .{
    
}

stmt -> exp SEMICOLON.{
    
}

stmt -> tp vallist SEMICOLON.{
    
}

stmt -> RETURN exp SEMICOLON.{
    
}

stmt -> LGB stmt RGB.{
    
}

stmt -> IF LLB exp RLB stmt.{
    
}

stmt -> IF LLB exp RLB stmt1 ELSE stmt.{
    
}

stmt -> WHILE LLB exp RLB stmt.{
    
}

stmt -> FOR LLB initlist SEMICOLON exp SEMICOLON steplist RLB stmt. {
    
}

initlist -> init COMMA initlist.{
    
}

initlist -> init.{
    
}

init -> ID ASSIGN exp. {
    
}

init -> ID ADDEQ exp. {
    
}

init -> ID MINUSEQ exp. {
    
}

steplist -> step COMMA steplist. {
    
}

steplist -> step. {
    
}

step -> BIADD ID.{
    
}

step -> ID BIADD.{
    
}

step -> BIMINUS ID.{
    
}

step -> ID BIMINUS.{
    
}

step -> ID ASSIGN exp.{
    
}

stmt1 -> IF LLB exp RLB stmt1 ELSE stmt1. {
    
}

stmt1 -> WHILE LLB exp RLB stmt1.{
    
}

stmt1 -> FOR LLB initlist SEMICOLON exp SEMICOLON steplist RLB stmt1.{
    
}

exp -> exp BIAND exp. {
   
}

exp -> exp BIOR exp. {
    
}

exp -> exp LSS exp. {
    
}

exp -> exp LSSEQ exp. {
    
}

exp -> exp GRT exp. {
    
}

exp -> exp GRTEQ exp. {
    
}

exp -> exp EQ exp. {
    
}

exp -> exp NOTEQ exp. {
    
}

exp -> exp ADD exp. {
    
}

exp -> exp MINUS exp. {
    
}

exp -> exp MULTI exp. {
    
}

exp -> exp DIV exp. {
    
}

exp -> exp MOD exp. {
    
}

exp -> INUM. {
    
}

exp -> FNUM. {
    
}

exp -> ID.{
    
}

exp -> LLB exp RLB. {
    
}

exp -> NOT exp.{
    
}

exp -> exp AND exp.{
    
}

exp -> exp OR exp.{
    
}

exp -> exp XOR exp.{
    
}

exp -> exp BITNOT exp.{
    
}

exp -> exp COMMA exp.{
    
}

exp -> ID LMB exp RMB.{
    
}

exp -> exp ASSIGN exp.{
    
} 

%code{
    
}
