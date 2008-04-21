// (c) by Stefan Roettger

#include "datagrid.h"

// default constructor
datagrid::datagrid()
   {
   CONSTRUCTED=FALSE;

   CRS=minicoord::MINICOORD_ECEF;

   ID[0]=MTX[0]=miniv4d(1.0,0.0,0.0);
   ID[1]=MTX[1]=miniv4d(0.0,1.0,0.0);
   ID[2]=MTX[2]=miniv4d(0.0,0.0,1.0);

   IDENTITY=TRUE;

   INVALID=FALSE;
   }

// destructor
datagrid::~datagrid()
   {
   unsigned int i;

   for (i=0; i<FLAG.getsize(); i++) remove(i);
   }

// set coordinate system
void datagrid::setcrs(const minicoord::MINICOORD crs)
   {CRS=crs;}

// create data brick id
unsigned int datagrid::create(const unsigned int slot,
                              const BOOLINT flip)
   {
   unsigned int i;

   INVALID=TRUE;

   for (i=0; i<FLAG.getsize(); i++)
      if (!FLAG[i])
         {
         FLAG[i]=TRUE;
         SLOT[i]=slot;
         FLIP[i]=flip;

         return(i);
         }

   FLAG.append(TRUE);
   SLOT.append(slot);
   FLIP.append(flip);

   DATA.setsize(FLAG.getsize());

   return(FLAG.getsize()-1);
   }

// load data
void datagrid::load(const unsigned int id,
                    const databuf &buf)
   {
   if (FLAG[id])
      {
      DATA[id].release();
      DATA[id]=buf;
      }
   }

// remove data
void datagrid::remove(const unsigned int id)
   {
   if (FLAG[id])
      {
      INVALID=TRUE;

      DATA[id].release();
      FLAG[id]=FALSE;
      }
   }

// move data
void datagrid::move(const unsigned int id,
                    const float swx,const float swy,
                    const float nwx,const float nwy,
                    const float nex,const float ney,
                    const float sex,const float sey,
                    const float h0,const float dh,
                    const float t0,const float dt)
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

// apply matrix
void datagrid::applymtx(const miniv4d mtx[3])
   {
   MTX[0]=mtx[0];
   MTX[1]=mtx[1];
   MTX[2]=mtx[2];

   IDENTITY=(MTX[0]==ID[0] && MTX[1]==ID[1] && MTX[2]==ID[2]);
   }

// construct tetrahedral mesh from all data bricks
void datagrid::construct()
   {while (!preprocess());}

// preprocess tetrahedral mesh one step at a time
BOOLINT datagrid::preprocess()
   {
   BOOLINT status1,status2;

   decompose(); // decompose all data bricks into tetrahedra

   status1=BSPT1.getstatus(); // get processing status
   status2=BSPT1.preprocess(); // process bsp tree one step at a time

   if (!status1 && status2)
      {
      BSPT2=BSPT1; // copy preprocessed bsp tree
      UNSORTED=BSPT2.extract(); // extract a non-intrusive unsorted tetrahedral mesh from the bsp tree
      CONSTRUCTED=TRUE; // bsp tree was constructed at least once
      }

   return(status2);
   }

// decompose all data bricks into tetrahedra
void datagrid::decompose()
   {
   unsigned int i,j;

   unsigned int num;

   minicoord::MINICOORD crs;

   minicoord vtx[8];
   miniv3d crd[8];

   miniv4d v;

   minihedron h;
   minivals vals;

   minimesh mesh;

   if (INVALID)
      {
      num=FLAG.getsize();

      // process all databuf objects
      for (i=0; i<num; i++)
         {
         // check if object at actual position is valid
         if (FLAG[i])
            {
            // check coordinate system of actual databuf object
            crs=minicoord::MINICOORD_LINEAR;
            if (DATA[i].crs==databuf::DATABUF_CRS_LLH) crs=minicoord::MINICOORD_LLH;
            else if (DATA[i].crs==databuf::DATABUF_CRS_UTM) crs=minicoord::MINICOORD_UTM;

            // determine corner vertices of actual object:

            vtx[0]=minicoord(miniv3d(DATA[i].swx,DATA[i].swy,DATA[i].h0),crs,DATA[i].zone,DATA[i].datum);
            vtx[1]=minicoord(miniv3d(DATA[i].nwx,DATA[i].nwy,DATA[i].h0),crs,DATA[i].zone,DATA[i].datum);
            vtx[2]=minicoord(miniv3d(DATA[i].nex,DATA[i].ney,DATA[i].h0),crs,DATA[i].zone,DATA[i].datum);
            vtx[3]=minicoord(miniv3d(DATA[i].sex,DATA[i].sey,DATA[i].h0),crs,DATA[i].zone,DATA[i].datum);

            vtx[4]=minicoord(miniv3d(DATA[i].swx,DATA[i].swy,DATA[i].h0+DATA[i].dh),crs,DATA[i].zone,DATA[i].datum);
            vtx[5]=minicoord(miniv3d(DATA[i].nwx,DATA[i].nwy,DATA[i].h0+DATA[i].dh),crs,DATA[i].zone,DATA[i].datum);
            vtx[6]=minicoord(miniv3d(DATA[i].nex,DATA[i].ney,DATA[i].h0+DATA[i].dh),crs,DATA[i].zone,DATA[i].datum);
            vtx[7]=minicoord(miniv3d(DATA[i].sex,DATA[i].sey,DATA[i].h0+DATA[i].dh),crs,DATA[i].zone,DATA[i].datum);

            // determine data coordinates of actual object:

            crd[0]=miniv3d(0.0,0.0,0.0);
            crd[1]=miniv3d(0.0,1.0,0.0);
            crd[2]=miniv3d(1.0,1.0,0.0);
            crd[3]=miniv3d(1.0,0.0,0.0);

            crd[4]=miniv3d(0.0,0.0,1.0);
            crd[5]=miniv3d(0.0,1.0,1.0);
            crd[6]=miniv3d(1.0,1.0,1.0);
            crd[7]=miniv3d(1.0,0.0,1.0);

            // transform corner vertices
            if (crs!=minicoord::MINICOORD_LINEAR)
               if (CRS!=minicoord::MINICOORD_LINEAR)
                  for (j=0; j<8; j++) vtx[j].convert2(CRS);

            // multiply corner vertices with 4x3 matrix
            if (!IDENTITY)
               for (j=0; j<8; j++)
                  {
                  v=miniv4d(vtx[j].vec.x,vtx[j].vec.y,vtx[j].vec.z,1.0);
                  vtx[j].vec=miniv3d(MTX[0]*v,MTX[1]*v,MTX[2]*v);
                  }

            // check orientation of tetrahedral decomposition
            if (!FLIP[i])
               {
               // add the 4 corner tetrahedra of the actual databuf object to the mesh:

               vals.set(minival(SLOT[i],crd[0],crd[1],crd[3],crd[4],vtx[0].vec,vtx[1].vec,vtx[3].vec,vtx[4].vec));
               h=minihedron(vtx[0].vec,vtx[1].vec,vtx[3].vec,vtx[4].vec,vals);
               mesh.append(h);

               vals.set(minival(SLOT[i],crd[2],crd[3],crd[1],crd[6],vtx[2].vec,vtx[3].vec,vtx[1].vec,vtx[6].vec));
               h=minihedron(vtx[2].vec,vtx[3].vec,vtx[1].vec,vtx[6].vec,vals);
               mesh.append(h);

               vals.set(minival(SLOT[i],crd[7],crd[6],crd[4],crd[3],vtx[7].vec,vtx[6].vec,vtx[4].vec,vtx[3].vec));
               h=minihedron(vtx[7].vec,vtx[6].vec,vtx[4].vec,vtx[3].vec,vals);
               mesh.append(h);

               vals.set(minival(SLOT[i],crd[5],crd[4],crd[6],crd[1],vtx[5].vec,vtx[4].vec,vtx[6].vec,vtx[1].vec));
               h=minihedron(vtx[5].vec,vtx[4].vec,vtx[6].vec,vtx[1].vec,vals);
               mesh.append(h);

               // add the 5th center tetrahedron of the actual databuf object to the mesh:

               vals.set(minival(SLOT[i],crd[3],crd[1],crd[6],crd[4],vtx[3].vec,vtx[1].vec,vtx[6].vec,vtx[4].vec));
               h=minihedron(vtx[3].vec,vtx[1].vec,vtx[6].vec,vtx[4].vec,vals);
               mesh.append(h);
               }
            else
               {
               // add the 4 corner tetrahedra of the actual databuf object to the mesh:

               vals.set(minival(SLOT[i],crd[3],crd[0],crd[2],crd[7],vtx[3].vec,vtx[0].vec,vtx[2].vec,vtx[7].vec));
               h=minihedron(vtx[3].vec,vtx[0].vec,vtx[2].vec,vtx[7].vec,vals);
               mesh.append(h);

               vals.set(minival(SLOT[i],crd[1],crd[2],crd[0],crd[5],vtx[1].vec,vtx[2].vec,vtx[0].vec,vtx[5].vec));
               h=minihedron(vtx[1].vec,vtx[2].vec,vtx[0].vec,vtx[5].vec,vals);
               mesh.append(h);

               vals.set(minival(SLOT[i],crd[4],crd[7],crd[5],crd[0],vtx[4].vec,vtx[7].vec,vtx[5].vec,vtx[0].vec));
               h=minihedron(vtx[4].vec,vtx[7].vec,vtx[5].vec,vtx[0].vec,vals);
               mesh.append(h);

               vals.set(minival(SLOT[i],crd[6],crd[5],crd[7],crd[2],vtx[6].vec,vtx[5].vec,vtx[7].vec,vtx[2].vec));
               h=minihedron(vtx[6].vec,vtx[5].vec,vtx[7].vec,vtx[2].vec,vals);
               mesh.append(h);

               // add the 5th center tetrahedron of the actual databuf object to the mesh:

               vals.set(minival(SLOT[i],crd[0],crd[5],crd[2],crd[7],vtx[0].vec,vtx[5].vec,vtx[2].vec,vtx[7].vec));
               h=minihedron(vtx[0].vec,vtx[5].vec,vtx[2].vec,vtx[7].vec,vals);
               mesh.append(h);
               }
            }
         }

      BSPT1.clear(); // clear the bsp tree
      BSPT1.insert(mesh); // insert the entire tetrahedral mesh into the bsp tree

      INVALID=FALSE;
      }
   }

// trigger pushing the mesh for a particular time step
void datagrid::trigger(const double time)
   {
   if (!CONSTRUCTED) construct(); // construct the bsp tree at least once
   push(UNSORTED,time); // push the static unsorted mesh
   }

// trigger pushing the mesh for a particular time step and eye point
void datagrid::trigger(const double time,const minicoord &eye,const double radius)
   {
   minicoord ep;

   miniv4d v;
   miniv3d e;

   ep=eye;

   // transform eye point
   if (ep.type!=minicoord::MINICOORD_LINEAR)
      if (CRS!=minicoord::MINICOORD_LINEAR) ep.convert2(CRS);

   // multiply eye point with 4x3 matrix
   if (!IDENTITY)
      {
      v=miniv4d(ep.vec.x,ep.vec.y,ep.vec.z,1.0);
      e=miniv3d(MTX[0]*v,MTX[1]*v,MTX[2]*v);
      }

   if (!CONSTRUCTED) construct(); // construct the bsp tree at least once
   SORTED=BSPT2.extract(e,radius); // extract a non-intrusive sorted tetrahedral mesh from the bsp tree
   push(SORTED,time); // push the dynamic sorted mesh
   }

// push the mesh for a particular time step
void datagrid::push(const minimesh &mesh,const double time)
   {printf("pushing mesh of size %u for time step %g\n",mesh.getsize(),time);}
