// (c) by Stefan Roettger

#include "miniwarp.h"

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
unsigned int datagrid::create(BOOLINT flip)
   {
   unsigned int i;

   INVALID=TRUE;

   for (i=0; i<SLOT.getsize(); i++)
      if (!SLOT[i]) return(i);

   SLOT.append(TRUE);
   FLIP.append(flip);
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

   minicoord::MINICOORD crs;
   minicoord crd[8];

   minitet tet;

   if (INVALID)
      {
      MESH.setsize(0);

      for (i=0; i<SLOT.getsize(); i++)
         if (SLOT[i])
            {
            if (DATA[i].crs==0) crs=minicoord::MINICOORD_LINEAR;
            else if (DATA[i].crs==1) crs=minicoord::MINICOORD_LLH;
            else if (DATA[i].crs==2) crs=minicoord::MINICOORD_UTM;
            else ERRORMSG();

            crd[0]=minicoord(miniv3d(DATA[i].swx,DATA[i].swy,DATA[i].h0),crs,DATA[i].zone,DATA[i].datum);
            crd[1]=minicoord(miniv3d(DATA[i].nwx,DATA[i].nwy,DATA[i].h0),crs,DATA[i].zone,DATA[i].datum);
            crd[2]=minicoord(miniv3d(DATA[i].nex,DATA[i].ney,DATA[i].h0),crs,DATA[i].zone,DATA[i].datum);
            crd[3]=minicoord(miniv3d(DATA[i].sex,DATA[i].sey,DATA[i].h0),crs,DATA[i].zone,DATA[i].datum);

            crd[4]=minicoord(miniv3d(DATA[i].swx,DATA[i].swy,DATA[i].h0+DATA[i].dh),crs,DATA[i].zone,DATA[i].datum);
            crd[5]=minicoord(miniv3d(DATA[i].nwx,DATA[i].nwy,DATA[i].h0+DATA[i].dh),crs,DATA[i].zone,DATA[i].datum);
            crd[6]=minicoord(miniv3d(DATA[i].nex,DATA[i].ney,DATA[i].h0+DATA[i].dh),crs,DATA[i].zone,DATA[i].datum);
            crd[7]=minicoord(miniv3d(DATA[i].sex,DATA[i].sey,DATA[i].h0+DATA[i].dh),crs,DATA[i].zone,DATA[i].datum);

            if (crs!=minicoord::MINICOORD_LINEAR)
               for (j=0; j<8; j++) crd[j].convert2(minicoord::MINICOORD_ECEF);

            if (!FLIP[i])
               {
               tet.vtx[0]=crd[0].vec;
               tet.vtx[1]=crd[1].vec;
               tet.vtx[2]=crd[3].vec;
               tet.vtx[3]=crd[4].vec;

               MESH.append(tet);

               tet.vtx[0]=crd[2].vec;
               tet.vtx[1]=crd[3].vec;
               tet.vtx[2]=crd[1].vec;
               tet.vtx[3]=crd[6].vec;

               MESH.append(tet);

               tet.vtx[0]=crd[7].vec;
               tet.vtx[1]=crd[6].vec;
               tet.vtx[2]=crd[4].vec;
               tet.vtx[3]=crd[3].vec;

               MESH.append(tet);

               tet.vtx[0]=crd[5].vec;
               tet.vtx[1]=crd[4].vec;
               tet.vtx[2]=crd[6].vec;
               tet.vtx[3]=crd[1].vec;

               MESH.append(tet);

               tet.vtx[0]=crd[3].vec;
               tet.vtx[1]=crd[1].vec;
               tet.vtx[2]=crd[6].vec;
               tet.vtx[3]=crd[4].vec;

               MESH.append(tet);
               }
            else
               {
               tet.vtx[0]=crd[3].vec;
               tet.vtx[1]=crd[0].vec;
               tet.vtx[2]=crd[2].vec;
               tet.vtx[3]=crd[7].vec;

               MESH.append(tet);

               tet.vtx[0]=crd[1].vec;
               tet.vtx[1]=crd[2].vec;
               tet.vtx[2]=crd[0].vec;
               tet.vtx[3]=crd[5].vec;

               MESH.append(tet);

               tet.vtx[0]=crd[4].vec;
               tet.vtx[1]=crd[7].vec;
               tet.vtx[2]=crd[5].vec;
               tet.vtx[3]=crd[0].vec;

               MESH.append(tet);

               tet.vtx[0]=crd[6].vec;
               tet.vtx[1]=crd[5].vec;
               tet.vtx[2]=crd[7].vec;
               tet.vtx[3]=crd[2].vec;

               MESH.append(tet);

               tet.vtx[0]=crd[0].vec;
               tet.vtx[1]=crd[5].vec;
               tet.vtx[2]=crd[2].vec;
               tet.vtx[3]=crd[7].vec;

               MESH.append(tet);
               }
            }

      INVALID=FALSE;
      }
   }
