// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minibase.h"

#include "datacalc.h"

// default constructor
datacalc::datacalc()
   {
   MODE=1;
   doregister();
   }

// destructor
datacalc::~datacalc()
   {deregister();}

// register with databuf objects
void datacalc::doregister()
   {databuf::setinterpreter(mystaticparser,this,mystaticinterpreter);}

// deregister with databuf objects
void datacalc::deregister()
   {databuf::setinterpreter(NULL,NULL,NULL);}

// parser hook
void datacalc::myparser(unsigned int implformat,const char *code,int bytes,const char *path,databuf *obj)
   {
   if (obj==NULL) ERRORMSG();

   if (implformat>0)
      {
      MODE=implformat;
      if (implformat==1) parser.setcode(code,bytes,path);
      }
   }

// interpreter hook
void datacalc::myinterpreter(float *value,int comps,float x,float y,float z,float t,databuf *obj)
   {
   if (obj==NULL) ERRORMSG();

   if (comps==1)
      switch (MODE)
         {
         // generic evaluation
         case 1:
            parser.init();
            parser.pushvalue(x);
            parser.pushvalue(y);
            parser.pushvalue(z);
            parser.pushvalue(t);
            parser.execute();
            *value=parser.popvalue();
            break;
         // hard-coded sphere
         case 2: *value=2.0f*fsqrt(x*x+y*y+z*z); break;
         // hard-coded half sphere
         case 3: *value=(z<0.0f)?1.0f:2.0f*fsqrt(x*x+y*y+z*z); break;
         // undefined
         default: ERRORMSG();
         }
   else if (comps==3 || comps==4)
      {
      if (MODE!=1) ERRORMSG();

      parser.init();

      parser.pushvalue(x);
      parser.pushvalue(y);
      parser.pushvalue(z);
      parser.pushvalue(t);

      parser.execute();

      if (comps==4) value[3]=parser.popvalue();

      value[2]=parser.popvalue();
      value[1]=parser.popvalue();
      value[0]=parser.popvalue();
      }
   else ERRORMSG();
   }

// static parser hook
void datacalc::mystaticparser(unsigned int implformat,const char *code,int bytes,const char *path,databuf *obj,void *data)
   {
   datacalc *calc=(datacalc *)data;
   calc->myparser(implformat,code,bytes,path,obj);
   }

// static interpreter hook
void datacalc::mystaticinterpreter(float *value,int comps,float x,float y,float z,float t,databuf *obj,void *data)
   {
   datacalc *calc=(datacalc *)data;
   calc->myinterpreter(value,comps,x,y,z,t,obj);
   }
