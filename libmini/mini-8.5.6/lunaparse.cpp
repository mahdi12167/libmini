// (c) by Stefan Roettger

#include "miniio.h"

#include "lunaparse.h"

// default constructor
lunaparse::lunaparse()
   {
   addLUNAtokens();

   VAR_NUM=0;
   }

// destructor
lunaparse::~lunaparse() {}

void lunaparse::addLUNAtokens()
   {
   scanner.addtoken("include",LUNA_INCLUDE);
   scanner.addtoken("var",LUNA_VAR);
   scanner.addtoken("par",LUNA_PAR);
   scanner.addtoken("array",LUNA_ARRAY);
   scanner.addtoken("ref",LUNA_REF);
   scanner.addtoken("byte",LUNA_BYTE);
   scanner.addtoken("main",LUNA_MAIN);
   scanner.addtoken("func",LUNA_FUNC);
   scanner.addtoken("if",LUNA_IF);
   scanner.addtoken("else",LUNA_ELSE);
   scanner.addtoken("while",LUNA_WHILE);
   scanner.addtoken("repeat",LUNA_REPEAT);
   scanner.addtoken("until",LUNA_UNTIL);
   scanner.addtoken("for",LUNA_FOR);
   scanner.addtoken("return",LUNA_RETURN);
   scanner.addtoken("warn",LUNA_WARN);
   scanner.addtoken(";",LUNA_NULL);
   scanner.addtoken("(",LUNA_PARENLEFT);
   scanner.addtoken(")",LUNA_PARENRIGHT);
   scanner.addtoken("{",LUNA_BRACELEFT);
   scanner.addtoken("}",LUNA_BRACERIGHT);
   scanner.addtoken("[",LUNA_BRACKETLEFT);
   scanner.addtoken("]",LUNA_BRACKETRIGHT);
   scanner.addtoken(":=",LUNA_ASSIGN);
   scanner.addtoken(",",LUNA_COMMA);
   scanner.addtoken("++",LUNA_INC);
   scanner.addtoken("--",LUNA_DEC);
   scanner.addtoken("size",LUNA_SIZE);
   scanner.addtoken("true",LUNA_TRUE);
   scanner.addtoken("false",LUNA_FALSE);
   scanner.addtoken("+",LUNA_ADD);
   scanner.addtoken("-",LUNA_SUB);
   scanner.addtoken("*",LUNA_MUL);
   scanner.addtoken("/",LUNA_DIV);
   scanner.addtoken("%",LUNA_MOD);
   scanner.addtoken("=",LUNA_EQ);
   scanner.addtoken("<>",LUNA_NEQ);
   scanner.addtoken("<",LUNA_LT);
   scanner.addtoken(">",LUNA_GT);
   scanner.addtoken("<=",LUNA_LE);
   scanner.addtoken(">=",LUNA_GE);
   scanner.addtoken("&",LUNA_AND);
   scanner.addtoken("|",LUNA_OR);
   scanner.addtoken("!",LUNA_NOT);
   scanner.addtoken("min",LUNA_MIN);
   scanner.addtoken("max",LUNA_MAX);
   scanner.addtoken("abs",LUNA_ABS);
   scanner.addtoken("sqr",LUNA_SQR);
   scanner.addtoken("sqrt",LUNA_SQRT);
   scanner.addtoken("exp",LUNA_EXP);
   scanner.addtoken("log",LUNA_LOG);
   scanner.addtoken("pow",LUNA_POW);
   scanner.addtoken("sin",LUNA_SIN);
   scanner.addtoken("cos",LUNA_COS);
   scanner.addtoken("tan",LUNA_TAN);
   scanner.addtoken("atan",LUNA_ATAN);
   scanner.addtoken("atan2",LUNA_ATAN2);
   }

void lunaparse::parseLUNA()
   {
   int addr;

   MAIN=-1;

   addr=code.getaddr();
   code.addcode(lunacode::CODE_RESERVE_VAR,lunacode::MODE_INT,code.getaddr());

   while (scanner.gettoken()!=lunascan::LUNA_END)
      if (scanner.gettoken()==LUNA_INCLUDE) parse_include();
      else
         {
         if (scanner.gettoken()==LUNA_VAR) parse_var_decl(FALSE,FALSE,FALSE,FALSE);
         else if (scanner.gettoken()==LUNA_ARRAY) parse_var_decl(FALSE,FALSE,TRUE,FALSE);
         else if (scanner.gettoken()==LUNA_REF) parse_var_decl(FALSE,FALSE,FALSE,TRUE);
         else if (scanner.gettoken()==LUNA_MAIN) parse_func_decl(TRUE);
         else if (scanner.gettoken()==LUNA_FUNC) parse_func_decl(FALSE);
         else
            {
            PARSERMSG("expected declaration");
            scanner.next();
            }

         if (scanner.gettoken()==LUNA_NULL) scanner.next();
         }

   code.addcodeat(addr,lunacode::CODE_RESERVE_VAR,lunacode::MODE_INT,VAR_NUM);

   if (MAIN!=-1) code.addcode(lunacode::CODE_JSR,lunacode::MODE_ANY,MAIN);

   scanner.freecode();
   }

void lunaparse::parse_include()
   {
   char *filename;

   char *code;
   unsigned int bytes;

   scanner.next();

   if (scanner.gettoken()!=lunascan::LUNA_STRING) PARSERMSG("expected string");

   filename=scanner.getstring();
   code=(char *)readfile(filename,&bytes);

   if (code==NULL)
      {
      PARSERMSG("file not found");
      scanner.next();
      }
   else
      {
      if ((code=(char *)realloc(code,bytes+1))==NULL) ERRORMSG();
      code[bytes]='\0';

      scanner.pushcode(code);
      free(code);
      }
   }

int lunaparse::parse_var_decl(BOOLINT loc,BOOLINT par,BOOLINT array,BOOLINT ref,int *VAR_LOC_NUM)
   {
   BOOLINT byte;

   int var_num=0;
   int var_loc_num=0;

   int pop_num;

   scanner.next();

   byte=FALSE;

   if (array || ref)
      if (scanner.gettoken()==LUNA_BYTE)
         {
         byte=TRUE;
         scanner.next();
         }

   if (scanner.gettoken()!=lunascan::LUNA_UNKNOWN) PARSERMSG("identifier already defined");
   else if (scanner.check_special()) PARSERMSG("bad identifier");
   else
      if (!array && !ref)
         if (!loc && !par)
            {
            var_num=VAR_NUM++;
            scanner.addtoken(scanner.getstring(),LUNA_VAR_GLB,var_num);
            }
         else
            {
            var_loc_num=(*VAR_LOC_NUM)++;
            scanner.addtoken(scanner.getstring(),LUNA_VAR_LOC,var_loc_num);
            }
      else if (!ref)
         if (!loc)
            {
            var_num=VAR_NUM++;
            scanner.addtoken(scanner.getstring(),LUNA_ARRAY_GLB,var_num);
            }
         else
            {
            var_loc_num=(*VAR_LOC_NUM)++;
            scanner.addtoken(scanner.getstring(),LUNA_ARRAY_LOC,var_loc_num);
            }
      else
         if (!loc)
            {
            var_num=VAR_NUM++;
            scanner.addtoken(scanner.getstring(),LUNA_REF_GLB,var_num);
            }
         else
            {
            var_loc_num=(*VAR_LOC_NUM)++;
            scanner.addtoken(scanner.getstring(),LUNA_REF_LOC,var_loc_num);
            }

   scanner.next();

   pop_num=-1;

   if (!array && !ref)
      {
      if (!par)
         if (scanner.gettoken()==LUNA_ASSIGN)
            {
            scanner.next();
            parse_expression();

            if (!loc)
               code.addcode(lunacode::CODE_POP_VAR,lunacode::MODE_ANY,var_num);
            else
               code.addcode(lunacode::CODE_POP_VAR_LOC,lunacode::MODE_ANY,var_loc_num);
            }

      if (par) pop_num=var_loc_num;
      }
   else if (!ref)
      if (scanner.gettoken()==LUNA_BRACKETLEFT)
         {
         scanner.next();

         parse_expression();

         if (!loc)
            if (!byte)
               code.addcode(lunacode::CODE_ALLOC_ARRAY,var_num);
            else
               code.addcode(lunacode::CODE_ALLOC_ARRAY_BYT,var_num);
         else
            if (!byte)
               code.addcode(lunacode::CODE_ALLOC_ARRAY_LOC,var_loc_num);
            else
               code.addcode(lunacode::CODE_ALLOC_ARRAY_BYT_LOC,var_loc_num);

         if (scanner.gettoken()!=LUNA_BRACKETRIGHT) PARSERMSG("expected matching bracket");
         scanner.next();
         }
      else
         if (!loc)
            if (!byte)
               code.addcode(lunacode::CODE_INIT_ARRAY,var_num);
            else
               code.addcode(lunacode::CODE_INIT_ARRAY_BYT,var_num);
         else
            if (!byte)
               code.addcode(lunacode::CODE_INIT_ARRAY_LOC,var_loc_num);
            else
               code.addcode(lunacode::CODE_INIT_ARRAY_BYT_LOC,var_loc_num);
   else
      {
      if (!par)
         if (!loc)
            if (!byte)
               code.addcode(lunacode::CODE_INIT_REF,var_num);
            else
               code.addcode(lunacode::CODE_INIT_REF_BYT,var_num);
         else
            if (!byte)
               code.addcode(lunacode::CODE_INIT_REF_LOC,var_loc_num);
            else
               code.addcode(lunacode::CODE_INIT_REF_BYT_LOC,var_loc_num);

      if (par)
         {
         if (!byte)
            code.addcode(lunacode::CODE_INIT_REF_LOC,lunacode::MODE_ANY,var_loc_num);
         else
            code.addcode(lunacode::CODE_INIT_REF_BYT_LOC,lunacode::MODE_ANY,var_loc_num);

         pop_num=var_loc_num;
         }
      }

   return(pop_num);
   }

void lunaparse::parse_par_decl(int *PAR_LOC_NUM)
   {
   int pop_var_loc_num;
   int pop_ref_loc_num;

   pop_var_loc_num=pop_ref_loc_num=-1;

   if (scanner.gettoken()==LUNA_PAR) pop_var_loc_num=parse_var_decl(TRUE,TRUE,FALSE,FALSE,PAR_LOC_NUM);
   else if (scanner.gettoken()==LUNA_REF) pop_ref_loc_num=parse_var_decl(TRUE,TRUE,FALSE,TRUE,PAR_LOC_NUM);
   else
      {
      PARSERMSG("expected parameter declaration");
      scanner.next();
      }

   if (scanner.gettoken()==LUNA_COMMA)
      {
      scanner.next();
      parse_par_decl(PAR_LOC_NUM);
      }

   if (pop_var_loc_num!=-1) code.addcode(lunacode::CODE_POP_VAR_LOC,lunacode::MODE_ANY,pop_var_loc_num);
   else if (pop_ref_loc_num!=-1) code.addcode(lunacode::CODE_POP_REF_LOC,lunacode::MODE_ANY,pop_ref_loc_num);
   }

void lunaparse::parse_func_decl(BOOLINT main)
   {
   int i;

   int VAR_LOC_NUM;

   int addr1,addr2,addr3;

   int serials;

   VAR_LOC_NUM=0;

   scanner.next();

   addr1=code.getaddr();
   code.addcode(lunacode::CODE_JMP,lunacode::MODE_INT,code.getaddr());

   addr2=code.getaddr();
   code.addcode(lunacode::CODE_JMP,lunacode::MODE_INT,code.getaddr());

   if (main)
      if (MAIN!=-1) PARSERMSG("main already defined");
      else MAIN=code.getaddr();
   else
      {
      if (scanner.gettoken()!=lunascan::LUNA_UNKNOWN) PARSERMSG("identifier already defined");
      else if (scanner.check_special()) PARSERMSG("bad identifier");
      else scanner.addtoken(scanner.getstring(),LUNA_FUNCTION,code.getaddr());

      scanner.next();
      }

   scanner.push();

   addr3=code.getaddr();
   code.addcode(lunacode::CODE_RESERVE_VAR_LOC,lunacode::MODE_INT,0);

   if (scanner.gettoken()!=LUNA_PARENLEFT) PARSERMSG("expected parameter list");
   scanner.next();

   if (scanner.gettoken()==LUNA_PARENRIGHT) scanner.next();
   else
      {
      parse_par_decl(&VAR_LOC_NUM);

      if (scanner.gettoken()!=LUNA_PARENRIGHT) PARSERMSG("expected matching paren");
      scanner.next();
      }

   parse_statement(&VAR_LOC_NUM,addr2);

   code.addcodeat(addr3,lunacode::CODE_RESERVE_VAR_LOC,lunacode::MODE_INT,VAR_LOC_NUM);

   code.addcodeat(addr2,lunacode::CODE_JMP,lunacode::MODE_INT,code.getaddr());

   serials=scanner.getserials();

   for (i=scanner.popserials(); i<serials; i++)
      if (scanner.gettoken(i)==LUNA_ARRAY_LOC)
         code.addcode(lunacode::CODE_FREE_ARRAY_LOC,lunacode::MODE_ANY,scanner.getinfo(i));

   code.addcode(lunacode::CODE_REMOVE_VAR_LOC,lunacode::MODE_INT,VAR_LOC_NUM);

   code.addcode(lunacode::CODE_RSR,lunacode::MODE_NONE);

   scanner.pop();

   code.addcodeat(addr1,lunacode::CODE_JMP,lunacode::MODE_INT,code.getaddr());
   }

void lunaparse::parse_statement(int *VAR_LOC_NUM,int RET_ADDR)
   {
   int info;

   int addr1,addr2,addr3;

   if (scanner.gettoken()==LUNA_VAR_GLB)
      parse_statement(FALSE,
                      lunacode::CODE_POP_VAR,lunacode::CODE_INC_VAR,lunacode::CODE_DEC_VAR,
                      lunacode::CODE_NOP,lunacode::CODE_NOP,lunacode::CODE_NOP);
   else if (scanner.gettoken()==LUNA_VAR_LOC)
      parse_statement(FALSE,
                      lunacode::CODE_POP_VAR_LOC,lunacode::CODE_INC_VAR_LOC,lunacode::CODE_DEC_VAR_LOC,
                      lunacode::CODE_NOP,lunacode::CODE_NOP,lunacode::CODE_NOP);
   else if (scanner.gettoken()==LUNA_ARRAY_GLB)
      parse_statement(TRUE,
                      lunacode::CODE_NOP,lunacode::CODE_NOP,lunacode::CODE_NOP,
                      lunacode::CODE_POP_ARRAY_IDX,lunacode::CODE_INC_ARRAY_IDX,lunacode::CODE_DEC_ARRAY_IDX);
   else if (scanner.gettoken()==LUNA_ARRAY_LOC)
      parse_statement(TRUE,
                      lunacode::CODE_NOP,lunacode::CODE_NOP,lunacode::CODE_NOP,
                      lunacode::CODE_POP_ARRAY_LOC_IDX,lunacode::CODE_INC_ARRAY_LOC_IDX,lunacode::CODE_DEC_ARRAY_LOC_IDX);
   else if (scanner.gettoken()==LUNA_REF_GLB)
      parse_statement(TRUE,
                      lunacode::CODE_NOP,lunacode::CODE_NOP,lunacode::CODE_NOP,
                      lunacode::CODE_POP_REF_IDX,lunacode::CODE_INC_REF_IDX,lunacode::CODE_DEC_REF_IDX);
   else if (scanner.gettoken()==LUNA_REF_LOC)
      parse_statement(TRUE,
                      lunacode::CODE_NOP,lunacode::CODE_NOP,lunacode::CODE_NOP,
                      lunacode::CODE_POP_REF_LOC_IDX,lunacode::CODE_INC_REF_LOC_IDX,lunacode::CODE_DEC_REF_LOC_IDX);
   else if (scanner.gettoken()==LUNA_FUNCTION)
      {
      info=scanner.getinfo();

      scanner.next();

      if (scanner.gettoken()==LUNA_PARENLEFT)
         {
         parse_expression(TRUE);

         code.addcode(lunacode::CODE_JSR,lunacode::MODE_ANY,info);
         }
      else
         {
         PARSERMSG("expected parameter list");
         scanner.next();
         }
      }
   else if (scanner.gettoken()==LUNA_BRACELEFT)
      {
      scanner.next();

      scanner.push();

      while (scanner.gettoken()!=LUNA_BRACERIGHT)
         {
         if (scanner.gettoken()==lunascan::LUNA_END)
            {
            PARSERMSG("brace mismatch",TRUE);
            return;
            }

         if (scanner.gettoken()==LUNA_VAR) parse_var_decl(TRUE,FALSE,FALSE,FALSE,VAR_LOC_NUM);
         else if (scanner.gettoken()==LUNA_ARRAY) parse_var_decl(TRUE,FALSE,TRUE,FALSE,VAR_LOC_NUM);
         else if (scanner.gettoken()==LUNA_REF) parse_var_decl(TRUE,FALSE,FALSE,TRUE,VAR_LOC_NUM);
         else if (scanner.gettoken()==LUNA_FUNC) parse_func_decl(FALSE);
         else parse_statement(VAR_LOC_NUM,RET_ADDR);
         }

      scanner.pop();

      scanner.next();
      }
   else if (scanner.gettoken()==LUNA_IF)
      {
      scanner.next();

      if (scanner.gettoken()==LUNA_PARENLEFT) parse_expression();
      else
         {
         PARSERMSG("expected expression");
         scanner.next();
         }

      addr1=code.getaddr();
      code.addcode(lunacode::CODE_JIF,lunacode::MODE_INT,code.getaddr());

      parse_statement(VAR_LOC_NUM,RET_ADDR);

      if (scanner.gettoken()==LUNA_ELSE)
         {
         addr2=code.getaddr();
         code.addcode(lunacode::CODE_JMP,lunacode::MODE_INT,code.getaddr());

         code.addcodeat(addr1,lunacode::CODE_JIF,lunacode::MODE_INT,code.getaddr());

         scanner.next();
         parse_statement(VAR_LOC_NUM,RET_ADDR);

         code.addcodeat(addr2,lunacode::CODE_JMP,lunacode::MODE_INT,code.getaddr());
         }
      else
         code.addcodeat(addr1,lunacode::CODE_JIF,lunacode::MODE_INT,code.getaddr());
      }
   else if (scanner.gettoken()==LUNA_WHILE)
      {
      scanner.next();

      addr1=code.getaddr();

      if (scanner.gettoken()==LUNA_PARENLEFT) parse_expression();
      else
         {
         PARSERMSG("expected expression");
         scanner.next();
         }

      addr2=code.getaddr();
      code.addcode(lunacode::CODE_JIF,lunacode::MODE_INT,code.getaddr());

      parse_statement(VAR_LOC_NUM,RET_ADDR);

      code.addcode(lunacode::CODE_JMP,lunacode::MODE_ANY,addr1);

      code.addcodeat(addr2,lunacode::CODE_JIF,lunacode::MODE_INT,code.getaddr());
      }
   else if (scanner.gettoken()==LUNA_REPEAT)
      {
      scanner.next();

      addr1=code.getaddr();

      parse_statement(VAR_LOC_NUM,RET_ADDR);

      if (scanner.gettoken()!=LUNA_UNTIL) PARSERMSG("expected matching UNTIL");
      scanner.next();

      if (scanner.gettoken()==LUNA_PARENLEFT) parse_expression();
      else
         {
         PARSERMSG("expected expression");
         scanner.next();
         }

      code.addcode(lunacode::CODE_JIF,lunacode::MODE_ANY,addr1);
      }
   else if (scanner.gettoken()==LUNA_FOR)
      {
      scanner.next();

      if (scanner.gettoken()!=LUNA_PARENLEFT) PARSERMSG("expected parameter list");
      scanner.next();

      parse_statement(VAR_LOC_NUM,RET_ADDR);

      if (scanner.gettoken()!=LUNA_COMMA) PARSERMSG("expected comma");
      scanner.next();

      addr1=code.getaddr();

      parse_expression();

      addr2=code.getaddr();
      code.addcode(lunacode::CODE_JIF,lunacode::MODE_INT,code.getaddr());

      if (scanner.gettoken()!=LUNA_COMMA) PARSERMSG("expected comma");
      scanner.next();

      addr3=code.getaddr();

      parse_statement(VAR_LOC_NUM,RET_ADDR);

      code.addcode(lunacode::CODE_JMP,lunacode::MODE_ANY,addr1);

      if (scanner.gettoken()!=LUNA_PARENRIGHT) PARSERMSG("expected matching paren");
      scanner.next();

      parse_statement(VAR_LOC_NUM,RET_ADDR);

      code.addcode(lunacode::CODE_JMP,lunacode::MODE_ANY,addr3);

      code.addcodeat(addr2,lunacode::CODE_JIF,lunacode::MODE_INT,code.getaddr());
      }
   else if (scanner.gettoken()==LUNA_RETURN)
      {
      scanner.next();
      if (scanner.gettoken()==LUNA_PARENLEFT) parse_expression();

      code.addcode(lunacode::CODE_JMP,lunacode::MODE_ANY,RET_ADDR);
      }
   else if (scanner.gettoken()==LUNA_WARN)
      {
      code.addcode(lunacode::CODE_WARN,lunacode::MODE_ANY,scanner.getline());

      scanner.next();
      }
   else
      {
      PARSERMSG("expected statement");
      scanner.next();
      }

   if (scanner.gettoken()==LUNA_NULL) scanner.next();
   }

void lunaparse::parse_statement(BOOLINT index,
                                int code_assign,int code_inc,int code_dec,
                                int code_assign_idx,int code_inc_idx,int code_dec_idx)
   {
   int info;

   info=scanner.getinfo();

   scanner.next();

   if (index)
      {
      if (scanner.gettoken()!=LUNA_BRACKETLEFT) PARSERMSG("expected array index");

      scanner.next();

      parse_expression();

      if (scanner.gettoken()!=LUNA_BRACKETRIGHT) PARSERMSG("expected matching bracket");
      scanner.next();
      }

   if (scanner.gettoken()==LUNA_ASSIGN)
      {
      scanner.next();
      parse_expression();

      if (!index) code.addcode(code_assign,lunacode::MODE_ANY,info);
      else code.addcode(code_assign_idx,lunacode::MODE_ANY,info);
      }
   else if (scanner.gettoken()==LUNA_INC)
      {
      if (!index) code.addcode(code_inc,lunacode::MODE_ANY,info);
      else code.addcode(code_inc_idx,lunacode::MODE_ANY,info);

      scanner.next();
      }
   else if (scanner.gettoken()==LUNA_DEC)
      {
      if (!index) code.addcode(code_dec,lunacode::MODE_ANY,info);
      else code.addcode(code_dec_idx,lunacode::MODE_ANY,info);

      scanner.next();
      }
   else
      {
      PARSERMSG("expected statement");
      scanner.next();
      }
   }

void lunaparse::parse_expression(BOOLINT comma)
   {
   int info;

   int op;
   int args;

   if (scanner.gettoken()==lunascan::LUNA_VALUE)
      {
      code.addcode(lunacode::CODE_PUSH,lunacode::MODE_FLOAT,0,scanner.getvalue());

      scanner.next();
      }
   else if (scanner.gettoken()==LUNA_VAR_GLB)
      {
      code.addcode(lunacode::CODE_PUSH_VAR,lunacode::MODE_ANY,scanner.getinfo());

      scanner.next();
      }
   else if (scanner.gettoken()==LUNA_VAR_LOC)
      {
      code.addcode(lunacode::CODE_PUSH_VAR_LOC,lunacode::MODE_ANY,scanner.getinfo());

      scanner.next();
      }
   else if (scanner.gettoken()==LUNA_ARRAY_GLB) parse_expression(lunacode::CODE_PUSH_ARRAY,lunacode::CODE_PUSH_ARRAY_IDX);
   else if (scanner.gettoken()==LUNA_ARRAY_LOC) parse_expression(lunacode::CODE_PUSH_ARRAY_LOC,lunacode::CODE_PUSH_ARRAY_LOC_IDX);
   else if (scanner.gettoken()==LUNA_REF_GLB) parse_expression(lunacode::CODE_PUSH_REF,lunacode::CODE_PUSH_REF_IDX);
   else if (scanner.gettoken()==LUNA_REF_LOC) parse_expression(lunacode::CODE_PUSH_REF_LOC,lunacode::CODE_PUSH_REF_LOC_IDX);
   else if (scanner.gettoken()==LUNA_FUNCTION)
      {
      info=scanner.getinfo();

      scanner.next();

      if (scanner.gettoken()==LUNA_PARENLEFT)
         {
         parse_expression(TRUE);

         code.addcode(lunacode::CODE_JSR,lunacode::MODE_ANY,info);
         }
      else
         {
         PARSERMSG("expected parameter list");
         scanner.next();
         }
      }
   else if (scanner.gettoken()==LUNA_PARENLEFT)
      {
      scanner.next();

      op=lunacode::CODE_NOP;

      switch (scanner.gettoken())
         {
         case LUNA_ADD: op=lunacode::CODE_ADD; break;
         case LUNA_SUB: op=lunacode::CODE_SUB; break;
         case LUNA_MUL: op=lunacode::CODE_MUL; break;
         case LUNA_DIV: op=lunacode::CODE_DIV; break;
         case LUNA_MOD: op=lunacode::CODE_MOD; break;
         case LUNA_EQ: op=lunacode::CODE_EQ; break;
         case LUNA_NEQ: op=lunacode::CODE_NEQ; break;
         case LUNA_LT: op=lunacode::CODE_LT; break;
         case LUNA_GT: op=lunacode::CODE_GT; break;
         case LUNA_LE: op=lunacode::CODE_LE; break;
         case LUNA_GE: op=lunacode::CODE_GE; break;
         case LUNA_AND: op=lunacode::CODE_AND; break;
         case LUNA_OR: op=lunacode::CODE_OR; break;
         case LUNA_NOT: op=lunacode::CODE_NOT; break;
         }

      if (op!=lunacode::CODE_NOP) scanner.next();

      args=0;

      while (scanner.gettoken()!=LUNA_PARENRIGHT)
         {
         if (comma && op==lunacode::CODE_NOP && args>0)
            {
            if (scanner.gettoken()!=LUNA_COMMA) PARSERMSG("expected comma");
            scanner.next();
            }

         if (scanner.gettoken()==lunascan::LUNA_END)
            {
            PARSERMSG("paren mismatch",TRUE);
            return;
            }

         parse_expression();

         args++;
         }

      if (op!=lunacode::CODE_NOP)
         code.addcode(op,lunacode::MODE_NONE);

      scanner.next();
      }
   else if (scanner.gettoken()==LUNA_SIZE)
      {
      scanner.next();

      if (scanner.gettoken()!=LUNA_PARENLEFT) PARSERMSG("expected parameter");
      scanner.next();

      if (scanner.gettoken()==LUNA_ARRAY_GLB)
         code.addcode(lunacode::CODE_SIZE_ARRAY,lunacode::MODE_ANY,scanner.getinfo());
      else if (scanner.gettoken()==LUNA_ARRAY_LOC)
         code.addcode(lunacode::CODE_SIZE_ARRAY_LOC,lunacode::MODE_ANY,scanner.getinfo());
      else if (scanner.gettoken()==LUNA_REF_GLB)
         code.addcode(lunacode::CODE_SIZE_REF,lunacode::MODE_ANY,scanner.getinfo());
      else if (scanner.gettoken()==LUNA_REF_LOC)
         code.addcode(lunacode::CODE_SIZE_REF_LOC,lunacode::MODE_ANY,scanner.getinfo());
      else PARSERMSG("expected array");

      scanner.next();

      if (scanner.gettoken()!=LUNA_PARENRIGHT) PARSERMSG("expected matching paren");
      scanner.next();
      }
   else if (scanner.gettoken()==LUNA_TRUE)
      {
      code.addcode(lunacode::CODE_PUSH,lunacode::MODE_BYTE,1);

      scanner.next();
      }
   else if (scanner.gettoken()==LUNA_FALSE)
      {
      code.addcode(lunacode::CODE_PUSH,lunacode::MODE_BYTE,0);

      scanner.next();
      }
   else
      {
      op=lunacode::CODE_NOP;

      switch (scanner.gettoken())
         {
         case LUNA_MIN: op=lunacode::CODE_MIN; break;
         case LUNA_MAX: op=lunacode::CODE_MAX; break;
         case LUNA_ABS: op=lunacode::CODE_ABS; break;
         case LUNA_SQR: op=lunacode::CODE_SQR; break;
         case LUNA_SQRT: op=lunacode::CODE_SQRT; break;
         case LUNA_EXP: op=lunacode::CODE_EXP; break;
         case LUNA_LOG: op=lunacode::CODE_LOG; break;
         case LUNA_POW: op=lunacode::CODE_POW; break;
         case LUNA_SIN: op=lunacode::CODE_SIN; break;
         case LUNA_COS: op=lunacode::CODE_COS; break;
         case LUNA_TAN: op=lunacode::CODE_TAN; break;
         case LUNA_ATAN: op=lunacode::CODE_ATAN; break;
         case LUNA_ATAN2: op=lunacode::CODE_ATAN2; break;
         }

      if (op!=lunacode::CODE_NOP)
         {
         scanner.next();

         if (scanner.gettoken()==LUNA_PARENLEFT)
            {
            parse_expression(TRUE);

            code.addcode(op,lunacode::MODE_NONE);
            }
         else
            {
            PARSERMSG("expected parameter list");
            scanner.next();
            }
         }
      else
         {
         PARSERMSG("expected expression");
         scanner.next();
         }
      }
   }

void lunaparse::parse_expression(int push,int push_idx)
   {
   int info;

   info=scanner.getinfo();

   scanner.next();

   if (scanner.gettoken()==LUNA_BRACKETLEFT)
      {
      scanner.next();

      parse_expression();

      code.addcode(push_idx,lunacode::MODE_ANY,info);

      if (scanner.gettoken()!=LUNA_BRACKETRIGHT) PARSERMSG("expected matching bracket");
      scanner.next();
      }
   else
      code.addcode(push,lunacode::MODE_ANY,info);
   }

void lunaparse::print()
   {
   if (scanner.getcode()==NULL) return;

   printf("%s",scanner.getcode());
   }

void lunaparse::printtokens()
   {
   while (scanner.gettoken()!=lunascan::LUNA_END)
      if (scanner.gettoken()==LUNA_INCLUDE) parse_include();
      else
         {
         printtoken();
         scanner.next();
         }

   scanner.freecode();
   }

void lunaparse::printtoken()
   {
   switch (scanner.gettoken())
      {
      case lunascan::LUNA_UNKNOWN: printf("identifier: %s\n",scanner.getstring()); break;
      case lunascan::LUNA_VALUE: printf("value: %f\n",scanner.getvalue()); break;
      case lunascan::LUNA_STRING: printf("string: %s\n",scanner.getstring()); break;
      default: printf("token: %d #%d\n",scanner.gettoken(),scanner.getserial()); break;
      }
   }

void lunaparse::PARSERMSG(char *msg,BOOLINT after)
   {
   if (scanner.getline()>0)
      if (after && scanner.popline()>0) fprintf(stderr,"parser error in column %d of line %d after line %d: %s\n",scanner.getcol(),scanner.getline(),scanner.popline(),msg);
      else fprintf(stderr,"parser error in column %d of line %d: %s\n",scanner.getcol(),scanner.getline(),msg);
   else fprintf(stderr,"parser error: %s\n",msg);
   }
