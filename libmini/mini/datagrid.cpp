// (c) by Stefan Roettger

#include "datagrid.h"

// default constructor
datagrid::datagrid()
   {INVALID=FALSE;}

// destructor
datagrid::~datagrid()
   {
   unsigned int i;

   for (i=0; i<SLOT.getsize(); i++) remove(i);
   }

// create data brick id
unsigned int datagrid::create()
   {
   unsigned int i;

   INVALID=TRUE;

   for (i=0; i<SLOT.getsize(); i++)
      if (!SLOT[i]) return(i);

   SLOT.append(TRUE);
   DATA.setsize(SLOT.getsize());

   return(SLOT.getsize()-1);
   }

// load data
void datagrid::load(unsigned int id,
                    const databuf &buf)
   {
   if (SLOT[id])
      {
      DATA[id].release();
      DATA[id]=buf;
      }
   }

// remove data
void datagrid::remove(unsigned int id)
   {
   if (SLOT[id])
      {
      INVALID=TRUE;

      DATA[id].release();
      SLOT[id]=FALSE;
      }
   }

// move data
void datagrid::move(unsigned int id,
                    float swx,float swy,
                    float nwx,float nwy,
                    float nex,float ney,
                    float sex,float sey,
                    float h0,float dh,
                    float t0,float dt)
   {
   if (SLOT[id])
      {
      if (DATA[id].swx!=swx || DATA[id].swy!=swy ||
          DATA[id].nwx!=nwx || DATA[id].nwy!=nwy ||
          DATA[id].nex!=nex || DATA[id].ney!=ney ||
          DATA[id].sex!=sex || DATA[id].sey!=sey ||
          DATA[id].h0!=h0 || DATA[id].dh!=dh) INVALID=TRUE;

      DATA[id].swx=swx;
      DATA[id].swy=swy;
      DATA[id].nwx=nwx;
      DATA[id].nwy=nwy;
      DATA[id].nex=nex;
      DATA[id].ney=ney;
      DATA[id].sex=sex;
      DATA[id].sey=sey;

      DATA[id].h0=h0;
      DATA[id].dh=dh;

      DATA[id].t0=t0;
      DATA[id].dt=dt;
      }
   }

// construct tetrahedral mesh from the data grid
void datagrid::construct()
   {
   unsigned int i,j;

   minitet tet;

   if (INVALID)
      {
      MESH.setsize(0);

      for (i=0; i<SLOT.getsize(); i++)
         if (SLOT[i])
            {
            for (j=0; j<4; j++) tet.vtx[j]=miniv3d(0.0);

            MESH.append(tet);
            }

      INVALID=FALSE;
      }
   }
