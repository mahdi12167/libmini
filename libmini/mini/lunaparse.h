// (c) by Stefan Roettger

/*

This module parses the RPN-style language LUNA according to the following EBNF grammar:

LUNA        ::= { include | declaration }
include     ::= "include" <string>
declaration ::= var_decl | array_decl | ref_decl | func_decl [ ";" ]
var_decl    ::= "var" <var-id> [ "=" | ":=" expression ]
par_decl    ::= "par" <var-id>
array_decl  ::= "array" [ "byte" ] <array-id> [ "[" expression "]" ]
ref_decl    ::= "ref" [ "byte" ] <ref-id>
list_decl   ::= par_decl | ref_decl { "," par_decl | ref_decl }
func_decl   ::= "main" | ( "func" <func-id> ) "(" [ list_decl ] ")" "{" { statement } "}"
statement   ::= ( <var-id> ( "=" | ":=" expression ) | "++" | "--" ) |
                ( <array-id> | <ref-id> "[" expression "]" ( "=" | ":=" expression ) | "++" | "--" ) |
                ( <func-id> "(" [ expression { [ "," ] expression } ] ")" ) |
                ( "{" { declaration | statement } "}" ) |
                ( "if" "(" expression ")" statement [ "else" statement ] ) |
                ( "while" "(" expression ")" statement ) |
                ( "repeat" statement "until" "(" expression ")" ) |
                ( "for" "(" statement "," expression "," statement ")" statement ) |
                ( "return" [ "(" expression ")" ] ) |
                ( "warn" ) [ ";" ]
expression  ::= ( ["-"]<float-val> ) |
                ( <var-id> ) |
                ( <array-id> | <ref-id> [ "[" expression "]" ] ) |
                ( <func-id> | alpha-op "(" [ expression { [ "," ] expression } ] ")" ) |
                ( "(" [ operator ] { expression } ")" ) |
                ( "size" "(" array-id | ref-id ")" ) |
                ( "true" | "false" )
operator    ::= "+" | "-" | "*" | "/" | "%" |
                "=" | "<>" | "<" | ">" | "<=" | ">=" |
                "&" | "|" | "!"
alpha-op    ::= "min" | "max" | "abs" |
                "sqr" | "sqrt" | "exp" | "log" | "pow" |
                "sin" | "cos" | "tan" | "atan" | "atan2"

The simplest LUNA program possible is:

   main()
      {
      return(0);
      }

The following LUNA program calculates the nth fibonacci number (iteratively):

   main(par fibo)
      {
      var i;

      var a;
      var b;
      var c;

      a=1;
      b=1;

      for (i=1, (< i fibo), i++)
         {
         c=(+ a b);
         a=b;
         b=c;
         }

      return(b);
      }

The following LUNA program calculates the nth fibonacci number (recursively):

   func fibonacci(par n)
      {
      if (> n 1) return(+ fibonacci(- n 2) fibonacci(- n 1));
      else return(1);
      }

   main(par fibo)
      {
      return(fibonacci(fibo));
      }
*/

#ifndef LUNAPARSE_H
#define LUNAPARSE_H

#include "minibase.h"

#include "lunascan.h"
#include "lunacode.h"

class lunaparse
   {
   public:

   enum
      {
      LUNA_INCLUDE=0,
      LUNA_VAR,
      LUNA_PAR,
      LUNA_ARRAY,
      LUNA_REF,
      LUNA_BYTE,
      LUNA_MAIN,
      LUNA_FUNC,
      LUNA_IF,
      LUNA_ELSE,
      LUNA_WHILE,
      LUNA_REPEAT,
      LUNA_UNTIL,
      LUNA_FOR,
      LUNA_RETURN,
      LUNA_WARN,
      LUNA_NULL,
      LUNA_PARENLEFT,
      LUNA_PARENRIGHT,
      LUNA_BRACELEFT,
      LUNA_BRACERIGHT,
      LUNA_BRACKETLEFT,
      LUNA_BRACKETRIGHT,
      LUNA_ASSIGN,
      LUNA_COMMA,
      LUNA_INC,
      LUNA_DEC,
      LUNA_SIZE,
      LUNA_TRUE,
      LUNA_FALSE,
      LUNA_ADD,
      LUNA_SUB,
      LUNA_MUL,
      LUNA_DIV,
      LUNA_MOD,
      LUNA_EQ,
      LUNA_NEQ,
      LUNA_LT,
      LUNA_GT,
      LUNA_LE,
      LUNA_GE,
      LUNA_AND,
      LUNA_OR,
      LUNA_NOT,
      LUNA_MIN,
      LUNA_MAX,
      LUNA_ABS,
      LUNA_SQR,
      LUNA_SQRT,
      LUNA_EXP,
      LUNA_LOG,
      LUNA_POW,
      LUNA_SIN,
      LUNA_COS,
      LUNA_TAN,
      LUNA_ATAN,
      LUNA_ATAN2,
      LUNA_VAR_GLB,
      LUNA_VAR_LOC,
      LUNA_ARRAY_GLB,
      LUNA_ARRAY_LOC,
      LUNA_REF_GLB,
      LUNA_REF_LOC,
      LUNA_FUNCTION,
      LUNA_UNUSED_TOKENS // unused token space
      };

   //! default constructor
   lunaparse();

   //! destructor
   ~lunaparse();

   void setcode(const char *code) {SCANNER.setcode(code);}
   void setcode(const char *code,int bytes) {SCANNER.setcode(code,bytes);}

   void parseLUNA();

   void print();
   void printtokens();
   void printtoken();

   lunascan *getscanner() {return(&SCANNER);}
   lunacode *getcode() {return(&CODE);}

   void parse_include(const char *path=NULL,const char *altpath=NULL);

   void PARSERMSG(const char *msg,BOOLINT after=FALSE);

   protected:

   lunascan SCANNER;
   lunacode CODE;

   void addLUNAtokens();

   private:

   int MAIN;

   int VAR_NUM;

   int parse_var_decl(BOOLINT loc,BOOLINT par,BOOLINT array,BOOLINT ref,BOOLINT stat,int *VAR_LOC_NUM=NULL);
   void parse_par_decl(int *PAR_LOC_NUM);
   void parse_func_decl(BOOLINT main);
   void parse_statement(int *VAR_LOC_NUM,int RET_ADDR);
   void parse_statement(BOOLINT index,int code_assign,int code_inc,int code_dec,int code_assign_idx,int code_inc_idx,int code_dec_idx);
   void parse_expression(BOOLINT comma=FALSE);
   void parse_expression(int push,int push_idx);
   };

#endif
