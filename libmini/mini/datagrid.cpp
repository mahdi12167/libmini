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

   for (i=0; i<FLAG.getsize(); i++) remove(i);
   }

// create data brick id
unsigned int datagrid::create(unsigned int slot,
                              BOOLINT flip)
   {
   unsigned int i;

   INVALID=TRUE;

   for (i=0; i<FLAG.getsize(); i++)
      if (!FLAG[i]) return(i);

   FLAG.append(TRUE);
   SLOT.append(slot);
   FLIP.append(flip);

   DATA.setsize(FLAG.getsize());

   return(FLAG.getsize()-1);
   }

// load data
void datagrid::load(unsigned int id,
                    const databuf &buf)
   {
   if (FLAG[id])
      {
      DATA[id].release();
      DATA[id]=buf;
      }
   }

// remove data
void datagrid::remove(unsigned int id)
   {
   if (FLAG[id])
      {
      INVALID=TRUE;

      DATA[id].release();
      FLAG[id]=FALSE;
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
   if (FLAG[id])
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

   minicoord vtx[8];
   miniv3d crd[8];

   minitet tet;

   if (INVALID)
      {
      MESH.setsize(0);

      for (i=0; i<FLAG.getsize(); i++)
         if (FLAG[i])
            {
            if (DATA[i].crs==0) crs=minicoord::MINICOORD_LINEAR;
            else if (DATA[i].crs==1) crs=minicoord::MINICOORD_LLH;
            else if (DATA[i].crs==2) crs=minicoord::MINICOORD_UTM;
            else ERRORMSG();

            vtx[0]=minicoord(miniv3d(DATA[i].swx,DATA[i].swy,DATA[i].h0),crs,DATA[i].zone,DATA[i].datum);
            vtx[1]=minicoord(miniv3d(DATA[i].nwx,DATA[i].nwy,DATA[i].h0),crs,DATA[i].zone,DATA[i].datum);
            vtx[2]=minicoord(miniv3d(DATA[i].nex,DATA[i].ney,DATA[i].h0),crs,DATA[i].zone,DATA[i].datum);
            vtx[3]=minicoord(miniv3d(DATA[i].sex,DATA[i].sey,DATA[i].h0),crs,DATA[i].zone,DATA[i].datum);

            vtx[4]=minicoord(miniv3d(DATA[i].swx,DATA[i].swy,DATA[i].h0+DATA[i].dh),crs,DATA[i].zone,DATA[i].datum);
            vtx[5]=minicoord(miniv3d(DATA[i].nwx,DATA[i].nwy,DATA[i].h0+DATA[i].dh),crs,DATA[i].zone,DATA[i].datum);
            vtx[6]=minicoord(miniv3d(DATA[i].nex,DATA[i].ney,DATA[i].h0+DATA[i].dh),crs,DATA[i].zone,DATA[i].datum);
            vtx[7]=minicoord(miniv3d(DATA[i].sex,DATA[i].sey,DATA[i].h0+DATA[i].dh),crs,DATA[i].zone,DATA[i].datum);

            crd[0]=miniv3d(0,0,0);
            crd[1]=miniv3d(0,1,0);
            crd[2]=miniv3d(1,1,0);
            crd[3]=miniv3d(1,0,0);

            crd[4]=miniv3d(0,0,1);
            crd[5]=miniv3d(0,1,1);
            crd[6]=miniv3d(1,1,1);
            crd[7]=miniv3d(1,0,1);

            if (crs!=minicoord::MINICOORD_LINEAR)
               for (j=0; j<8; j++) vtx[j].convert2(minicoord::MINICOORD_ECEF);

            tet.val.setsize(1);
            tet.val[0].slot=SLOT[i];

            if (!FLIP[i])
               {
               tet.vtx[0]=vtx[0].vec;
               tet.val[0].crd[0]=crd[0];
               tet.vtx[1]=vtx[1].vec;
               tet.val[0].crd[1]=crd[1];
               tet.vtx[2]=vtx[3].vec;
               tet.val[0].crd[2]=crd[3];
               tet.vtx[3]=vtx[4].vec;
               tet.val[0].crd[3]=crd[4];

               MESH.append(tet);

               tet.vtx[0]=vtx[2].vec;
               tet.val[0].crd[0]=crd[2];
               tet.vtx[1]=vtx[3].vec;
               tet.val[0].crd[1]=crd[3];
               tet.vtx[2]=vtx[1].vec;
               tet.val[0].crd[2]=crd[1];
               tet.vtx[3]=vtx[6].vec;
               tet.val[0].crd[3]=crd[6];

               MESH.append(tet);

               tet.vtx[0]=vtx[7].vec;
               tet.val[0].crd[0]=crd[7];
               tet.vtx[1]=vtx[6].vec;
               tet.val[0].crd[1]=crd[6];
               tet.vtx[2]=vtx[4].vec;
               tet.val[0].crd[2]=crd[4];
               tet.vtx[3]=vtx[3].vec;
               tet.val[0].crd[3]=crd[3];

               MESH.append(tet);

               tet.vtx[0]=vtx[5].vec;
               tet.val[0].crd[0]=crd[5];
               tet.vtx[1]=vtx[4].vec;
               tet.val[0].crd[1]=crd[4];
               tet.vtx[2]=vtx[6].vec;
               tet.val[0].crd[2]=crd[6];
               tet.vtx[3]=vtx[1].vec;
               tet.val[0].crd[3]=crd[1];

               MESH.append(tet);

               tet.vtx[0]=vtx[3].vec;
               tet.val[0].crd[0]=crd[3];
               tet.vtx[1]=vtx[1].vec;
               tet.val[0].crd[1]=crd[1];
               tet.vtx[2]=vtx[6].vec;
               tet.val[0].crd[2]=crd[6];
               tet.vtx[3]=vtx[4].vec;
               tet.val[0].crd[3]=crd[4];

               MESH.append(tet);
               }
            else
               {
               tet.vtx[0]=vtx[3].vec;
               tet.val[0].crd[0]=crd[3];
               tet.vtx[1]=vtx[0].vec;
               tet.val[0].crd[1]=crd[0];
               tet.vtx[2]=vtx[2].vec;
               tet.val[0].crd[2]=crd[2];
               tet.vtx[3]=vtx[7].vec;
               tet.val[0].crd[3]=crd[7];

               MESH.append(tet);

               tet.vtx[0]=vtx[1].vec;
               tet.val[0].crd[0]=crd[1];
               tet.vtx[1]=vtx[2].vec;
               tet.val[0].crd[1]=crd[2];
               tet.vtx[2]=vtx[0].vec;
               tet.val[0].crd[2]=crd[0];
               tet.vtx[3]=vtx[5].vec;
               tet.val[0].crd[3]=crd[5];

               MESH.append(tet);

               tet.vtx[0]=vtx[4].vec;
               tet.val[0].crd[0]=crd[4];
               tet.vtx[1]=vtx[7].vec;
               tet.val[0].crd[1]=crd[7];
               tet.vtx[2]=vtx[5].vec;
               tet.val[0].crd[2]=crd[5];
               tet.vtx[3]=vtx[0].vec;
               tet.val[0].crd[3]=crd[0];

               MESH.append(tet);

               tet.vtx[0]=vtx[6].vec;
               tet.val[0].crd[0]=crd[6];
               tet.vtx[1]=vtx[5].vec;
               tet.val[0].crd[1]=crd[5];
               tet.vtx[2]=vtx[7].vec;
               tet.val[0].crd[2]=crd[7];
               tet.vtx[3]=vtx[2].vec;
               tet.val[0].crd[3]=crd[2];

               MESH.append(tet);

               tet.vtx[0]=vtx[0].vec;
               tet.val[0].crd[0]=crd[0];
               tet.vtx[1]=vtx[5].vec;
               tet.val[0].crd[1]=crd[5];
               tet.vtx[2]=vtx[2].vec;
               tet.val[0].crd[2]=crd[2];
               tet.vtx[3]=vtx[7].vec;
               tet.val[0].crd[3]=crd[7];

               MESH.append(tet);
               }
            }

      BSPT.insert(MESH);
      BSPT.extract(TETS);

      INVALID=FALSE;
      }
   }

// trigger pushing the mesh for a particular time step
void datagrid::trigger(double time)
   {
   construct();
   push(TETS,time);
   }

// push the mesh for a particular time step
void datagrid::push(const minimesh &mesh,double time) {}
