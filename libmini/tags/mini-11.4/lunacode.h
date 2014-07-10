// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef LUNACODE_H
#define LUNACODE_H

#include "minibase.h"

#include "mininoise.h"

class lunacode
   {
   public:

   enum
      {
      CODE_NOP=0,
      CODE_ADD,
      CODE_SUB,
      CODE_NEG,
      CODE_MUL,
      CODE_DIV,
      CODE_MOD,
      CODE_EQ,
      CODE_NEQ,
      CODE_LT,
      CODE_GT,
      CODE_LE,
      CODE_GE,
      CODE_AND,
      CODE_OR,
      CODE_XOR,
      CODE_NOT,
      CODE_MIN,
      CODE_MAX,
      CODE_ABS,
      CODE_FLOOR,
      CODE_CEIL,
      CODE_SQR,
      CODE_SQRT,
      CODE_EXP,
      CODE_LOG,
      CODE_POW,
      CODE_SIN,
      CODE_COS,
      CODE_TAN,
      CODE_ATAN,
      CODE_ATAN2,
      CODE_NOISE,
      CODE_NOISE2,
      CODE_PUSH,
      CODE_RESERVE_VAR,
      CODE_RESERVE_VAR_LOC,
      CODE_REMOVE_VAR_LOC,
      CODE_PUSH_VAR,
      CODE_PUSH_VAR_LOC,
      CODE_POP_VAR,
      CODE_POP_VAR_STAT,
      CODE_POP_VAR_LOC,
      CODE_INC_VAR,
      CODE_INC_VAR_LOC,
      CODE_DEC_VAR,
      CODE_DEC_VAR_LOC,
      CODE_ALLOC_ARRAY,
      CODE_ALLOC_ARRAY_LOC,
      CODE_ALLOC_ARRAY_BYT,
      CODE_ALLOC_ARRAY_BYT_LOC,
      CODE_INIT_ARRAY,
      CODE_INIT_ARRAY_LOC,
      CODE_INIT_ARRAY_BYT,
      CODE_INIT_ARRAY_BYT_LOC,
      CODE_FREE_ARRAY_LOC,
      CODE_SIZE_ARRAY,
      CODE_SIZE_ARRAY_LOC,
      CODE_INIT_REF,
      CODE_INIT_REF_LOC,
      CODE_INIT_REF_BYT,
      CODE_INIT_REF_BYT_LOC,
      CODE_SIZE_REF,
      CODE_SIZE_REF_LOC,
      CODE_PUSH_ARRAY,
      CODE_PUSH_ARRAY_LOC,
      CODE_PUSH_ARRAY_IDX,
      CODE_PUSH_ARRAY_LOC_IDX,
      CODE_POP_ARRAY_IDX,
      CODE_POP_ARRAY_LOC_IDX,
      CODE_INC_ARRAY_IDX,
      CODE_INC_ARRAY_LOC_IDX,
      CODE_DEC_ARRAY_IDX,
      CODE_DEC_ARRAY_LOC_IDX,
      CODE_PUSH_REF,
      CODE_PUSH_REF_LOC,
      CODE_PUSH_REF_IDX,
      CODE_PUSH_REF_LOC_IDX,
      CODE_POP_REF,
      CODE_POP_REF_LOC,
      CODE_POP_REF_IDX,
      CODE_POP_REF_LOC_IDX,
      CODE_INC_REF_IDX,
      CODE_INC_REF_LOC_IDX,
      CODE_DEC_REF_IDX,
      CODE_DEC_REF_LOC_IDX,
      CODE_JMP,
      CODE_JIF,
      CODE_JSR,
      CODE_RSR,
      CODE_WARN
      };

   enum
      {
      MODE_NONE=0,
      MODE_BYTE,
      MODE_SHORT,
      MODE_INT,
      MODE_FLOAT,
      MODE_ANY
      };

   enum
      {
      ITEM_NONE=0,
      ITEM_FLOAT,
      ITEM_ARRAY_FLOAT,
      ITEM_ARRAY_BYTE,
      ITEM_REF_FLOAT,
      ITEM_REF_BYTE
      };

   //! default constructor
   lunacode();

   //! destructor
   ~lunacode();

   void addcode(int code=CODE_NOP,int mode=MODE_NONE,int ival=0,float fval=0.0f);
   void addcodeat(int addr,int code=CODE_NOP,int mode=MODE_NONE,int ival=0,float fval=0.0f);
   void delcodeat(int addr);

   int getaddr();

   void init(BOOLINT init_static=TRUE);
   void execute();
   void pushvalue(float val);
   float popvalue();

   int geterrors() {return(ERRORS);}

   void print();
   void printcode(int code);

   void setdebug(BOOLINT on) {LUNADEBUG=on;}

   protected:

   int ERRORS;

   unsigned char *CODE;
   int CODESIZE,CODEMAX;

   struct LUNA_ITEM
      {
      int item;
      double val;

      void *array;
      unsigned int maxsize,size;

      unsigned int timeloc;

      int ref,refloc;
      };

   int *RETSTACK;
   int RETSTACKSIZE,RETSTACKMAX;

   LUNA_ITEM *VALSTACK;
   int VALSTACKSIZE,VALSTACKMAX;

   LUNA_ITEM *GLBVAR,*LOCVAR;

   int GLBVARSIZE,GLBVARMAX;
   int LOCVARSIZE,LOCVARMAX;

   void addbyte(int addr,int val);
   int getcode(int addr,int *code,int *mode,int *ival,float *fval);

   void allocate_stacks();
   void allocate_vars();

   void execmd(int code,int ival=0,float fval=0.0f);

   void checkarray(int ref,unsigned int idx);
   void checkarrayloc(int refloc,unsigned int idx);

   void *mallocarray(int item,unsigned int size);
   void *reallocarray(void *array,int item,unsigned int size,unsigned int orig);

   private:

   unsigned int NOISEMAXNUM;
   mininoise **NOISE;

   int NOISESIZE;
   int NOISESTART;

   BOOLINT LUNADEBUG;

   inline double mod(double a,double b);
   inline double noise(double x,double y,double z,unsigned int n=0);

   void CODEMSG(const char *msg);
   };

#endif
