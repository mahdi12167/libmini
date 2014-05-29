// (c) by Stefan Roettger, licensed under LGPL 2.1

/*

This module parses the RPN-style language LUNA according to the following EBNF grammar:

LUNA        ::= { include | declaration | function }
include     ::= "include" <string>
declaration ::= vars_decl | array_decl | ref_decl [ ";" ]
var_decl    ::= "var" <var-id> [ "=" | ":=" expression ]
par_decl    ::= "par" <var-id>
array_decl  ::= "array" [ "byte" ] <array-id> [ "[" expression "]" ]
ref_decl    ::= "ref" [ "byte" ] <ref-id>
vars_decl   ::= var_decl { "," var_decl }
pars_decl   ::= par_decl | ref_decl { "," par_decl | ref_decl }
function    ::= "main" | ( "func" <func-id> ) "(" [ pars_decl ] ")"
                "{" { declaration | statement } "}"
statement   ::= ( <var-id> ( "=" | ":=" expression ) | "++" | "--" ) |
                ( <array-id> | <ref-id> "[" expression "]" ( "=" | ":=" expression ) | "++" | "--" ) |
                ( <func-id> "(" [ expression { "," expression } ] ")" ) |
                ( "{" { declaration | statement } "}" ) |
                ( "if" "(" expression ")" statement [ "else" statement ] ) |
                ( "while" "(" expression ")" statement ) |
                ( "repeat" statement "until" "(" expression ")" ) |
                ( "for" "(" statement "," expression "," statement ")" statement ) |
                ( "return" [ "(" expression ")" ] ) |
                ( "warn" )
                [ ";" ]
expression  ::= logic-op
logic-op    ::= comparison { "&" | "|" | "^" | "and" | "or" | "xor" comparison }
comparison  ::= term { "==" | "!=" | "<>" | "<" | ">" | "<=" | ">=" term }
term        ::= factor { "+" | "-" factor }
factor      ::= unary-op { "*" | "/" | "%" | "mod" unary-op }
unary-op    ::= [ "-" | "!" | "not" ] value
value       ::= ( ["-"]<float-val> ) |
                ( <var-id> ) |
                ( <array-id> | <ref-id> [ "[" expression "]" ] ) |
                ( <func-id> | alpha-op "(" [ expression { "," expression } ] ")" ) |
                ( "(" operator { expression } ")" ) |
                ( "(" [ expression { "," expression } ] ")" ) |
                ( "size" "(" array-id | ref-id ")" ) |
                ( "true" | "false" )
operator    ::= "+" | "-" | "*" | "/" | "%" |
                "==" | "!=" | "<>" | "<" | ">" | "<=" | ">=" |
                "&" | "|" | "^" | "!"
alpha-op    ::= "min" | "max" | "abs" | "floor" | "ceil" |
                "sqr" | "sqrt" | "exp" | "log" | "pow" |
                "sin" | "cos" | "tan" | "atan" | "atan2" |
                "noise" | "noise2"

The simplest LUNA program possible is:

   main()
      {
      return(0);
      }

The following LUNA program calculates the nth factorial number (recursively):

   func fac(par n)
      {
      if (n>1) return(n*fac(n-1));
      else return(1);
      }

   main(par n)
      {
      return(fac(n));
      }

The following LUNA program calculates the nth fibonacci number (iteratively):

   func fib(par n)
      {
      var i;

      var a,b,c;

      a=1;
      b=1;

      for (i=1, i<n, i++)
         {
         c=a+b;
         a=b;
         b=c;
         }

      return(b);
      }

   main(par n)
      {
      return(fib(n));
      }

Example use case:

   // code to be executed
   const char code[]="main() {return(0);}";

   // compile code
   lunaparse parser;
   parser.setcode(code);
   int errors=parser.parseLUNA();

   // check compile errors
   if (errors==0)
      {
      // execute compiled code
      parser.getcode()->init();
      parser.getcode()->execute();

      // get main return value
      double value=parser.getcode()->popvalue();

      // get execution errors
      int errors=parser.getcode()->geterrors();
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
      LUNA_XOR,
      LUNA_NOT,
      LUNA_MIN,
      LUNA_MAX,
      LUNA_ABS,
      LUNA_FLOOR,
      LUNA_CEIL,
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
      LUNA_NOISE,
      LUNA_NOISE2,
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
   lunaparse(BOOLINT luna=TRUE);

   //! destructor
   ~lunaparse();

   //! set the code to be parsed
   void setcode(const char *code) {SCANNER.setcode(code);}
   void setcode(const char *code,int bytes) {SCANNER.setcode(code,bytes);}

   //! set the paths to be searched when including code
   void setpath(const char *path,const char *altpath=NULL);

   int parseLUNA();
   int parseEXPR(const char *expr);

   void print();
   void printtokens();
   void printtoken();

   lunascan *getscanner() {return(&SCANNER);}
   lunacode *getcode() {return(&CODE);}

   void parse_include(const char *path=NULL,const char *altpath=NULL);
   BOOLINT include(const char *file,const char *path=NULL,const char *altpath=NULL);

   void PARSERMSG(const char *msg,BOOLINT after=FALSE);

   int geterrors() {return(ERRORS);}

   protected:

   int ERRORS;

   char *PATH;
   char *ALTPATH;

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
   void parse_var(BOOLINT index,int code_assign,int code_inc,int code_dec,int code_assign_idx,int code_inc_idx,int code_dec_idx);
   void parse_func();
   void parse_block(int *VAR_LOC_NUM,int RET_ADDR);
   void parse_if(int *VAR_LOC_NUM,int RET_ADDR);
   void parse_while(int *VAR_LOC_NUM,int RET_ADDR);
   void parse_repeat(int *VAR_LOC_NUM,int RET_ADDR);
   void parse_for(int *VAR_LOC_NUM,int RET_ADDR);
   void parse_expression();
   void parse_logicop();
   void parse_comparison();
   void parse_term();
   void parse_factor();
   void parse_unaryop();
   void parse_value();
   void parse_prefix_ops();
   BOOLINT parse_alpha_ops();
   void parse_var_index(int push,int push_idx);
   };

#endif
