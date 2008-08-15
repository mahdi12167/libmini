// (c) by Stefan Roettger

#include "datagrid.h"

// default constructor
datagrid::datagrid()
   {
   CRS=minicoord::MINICOORD_ECEF;

   INVALID=FALSE;
   DONE=FALSE;

   PHASE=0;
   STEP=0;

   CONSTRUCTED=FALSE;

   MTXPRE[0]=MTXPOST[0]=miniv4d(1.0,0.0,0.0);
   MTXPRE[1]=MTXPOST[1]=miniv4d(0.0,1.0,0.0);
   MTXPRE[2]=MTXPOST[2]=miniv4d(0.0,0.0,1.0);
   }

// destructor
datagrid::~datagrid()
   {clear();}

// set coordinate system
void datagrid::setcrs(const minicoord::MINICOORD crs)
   {CRS=crs;}

// create data brick id
unsigned int datagrid::create(const unsigned int slot,
                              const BOOLINT flip)
   {
   unsigned int i;

   for (i=0; i<FLAG.getsize(); i++)
      if (!FLAG[i])
         {
         FLAG[i]=TRUE;
         SLOT[i]=slot;
         FLIP[i]=flip;

         REF[i]=NULL;

         return(i);
         }

   FLAG.append(TRUE);
   SLOT.append(slot);
   FLIP.append(flip);

   REF.append(NULL);

   DATA.setsize(FLAG.getsize());

   return(FLAG.getsize()-1);
   }

// load data
void datagrid::load(const unsigned int id,
                    const databuf &buf)
   {
   if (FLAG[id])
      {
      if (DATA[id].missing()) INVALID=TRUE;
      else
         {
         if (DATA[id].swx!=buf.swx || DATA[id].swy!=buf.swy ||
             DATA[id].nwx!=buf.nwx || DATA[id].nwy!=buf.nwy ||
             DATA[id].nex!=buf.nex || DATA[id].ney!=buf.ney ||
             DATA[id].sex!=buf.sex || DATA[id].sey!=buf.sey ||
             DATA[id].h0!=buf.h0 || DATA[id].dh!=buf.dh) INVALID=TRUE;

         DATA[id].release();
         }

      DATA[id]=buf;
      }
   }

// remove data
void datagrid::remove(const unsigned int id)
   {
   if (FLAG[id])
      {
      if (!DATA[id].missing())
         {
         INVALID=TRUE;
         DATA[id].release();
         }

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
      if (!DATA[id].missing())
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

// reference to layer
void datagrid::reference(const unsigned int id,
                         minilayer *layer)
   {
   if (FLAG[id])
      {
      if (!DATA[id].missing())
         if (REF[id]!=layer) INVALID=TRUE;

      REF[id]=layer;
      }
   }

// clear all data bricks
void datagrid::clear()
   {
   unsigned int i;

   for (i=0; i<FLAG.getsize(); i++) remove(i);
   }

// check if any valid bricks are present
BOOLINT datagrid::isclear()
   {
   unsigned int i;

   for (i=0; i<FLAG.getsize(); i++)
      if (FLAG[i])
         if (!DATA[i].missing()) return(FALSE);

   return(TRUE);
   }

// apply pre matrix
void datagrid::applymtx(const miniv4d mtx[3])
   {
   INVALID=TRUE;

   MTXPRE[0]=mtx[0];
   MTXPRE[1]=mtx[1];
   MTXPRE[2]=mtx[2];
   }

// specify post matrix
void datagrid::specmtx(const miniv4d mtx[3])
   {
   MTXPOST[0]=mtx[0];
   MTXPOST[1]=mtx[1];
   MTXPOST[2]=mtx[2];
   }

// construct tetrahedral mesh from all data bricks
void datagrid::construct()
   {while (!preprocess());}

// preprocess tetrahedral mesh one step at a time
BOOLINT datagrid::preprocess()
   {
   BOOLINT status1,status2;

   // decompose all data bricks into tetrahedra
   if (!decompose()) status2=FALSE;
   else
      {
      status1=BSPT1.getstatus(); // get processing status
      status2=BSPT1.preprocess(); // process bsp tree one step at a time

      if (!status1 && status2)
         {
         BSPT2=BSPT1; // copy preprocessed bsp tree
         UNSORTED=BSPT2.extract(); // extract a non-intrusive unsorted tetrahedral mesh from the bsp tree
         CONSTRUCTED=TRUE; // bsp tree was constructed at least once
         }
      }

   return(status2);
   }

// decompose all data bricks into tetrahedra
BOOLINT datagrid::decompose()
   {
   if (INVALID)
      if (isclear())
         {
         BSPT1.clear();

         INVALID=FALSE;
         DONE=TRUE;
         }
      else
         switch (PHASE)
            {
            case 0:
               // phase #0: reset the mesh
               MESH.setnull();

               DONE=FALSE;

               PHASE++;

               break;
            case 1:
               // phase #1: decompose one data brick into 5 tetrahedra
               decompose(STEP);

               if (++STEP>=FLAG.getsize())
                  {
                  STEP=0;
                  PHASE++;
                  }

               break;
            case 2:
               // phase #2: build the bsp tree
               BSPT1.clear(); // clear the bsp tree
               BSPT1.insert(MESH); // insert the entire tetrahedral mesh into the bsp tree
               MESH.setnull(); // release the mesh

               INVALID=FALSE;
               DONE=TRUE;

               PHASE=0;
               STEP=0;

               break;
            }

   return(DONE);
   }

void datagrid::decompose(unsigned int idx)
   {
   unsigned int i;

   minicoord::MINICOORD crs;

   minicoord vtx[8];
   miniv3d crd[8];

   miniv4d v;

   minivals vals;

   // check if object at actual position is valid
   if (FLAG[idx])
      if (!DATA[idx].missing())
         {
         // check coordinate system of actual databuf object
         crs=minicoord::MINICOORD_LINEAR;
         if (DATA[idx].crs==databuf::DATABUF_CRS_LLH) crs=minicoord::MINICOORD_LLH;
         else if (DATA[idx].crs==databuf::DATABUF_CRS_UTM) crs=minicoord::MINICOORD_UTM;

         // determine corner vertices of actual object:

         vtx[0]=minicoord(miniv3d(DATA[idx].swx,DATA[idx].swy,DATA[idx].h0),crs,DATA[idx].zone,DATA[idx].datum);
         vtx[1]=minicoord(miniv3d(DATA[idx].nwx,DATA[idx].nwy,DATA[idx].h0),crs,DATA[idx].zone,DATA[idx].datum);
         vtx[2]=minicoord(miniv3d(DATA[idx].nex,DATA[idx].ney,DATA[idx].h0),crs,DATA[idx].zone,DATA[idx].datum);
         vtx[3]=minicoord(miniv3d(DATA[idx].sex,DATA[idx].sey,DATA[idx].h0),crs,DATA[idx].zone,DATA[idx].datum);

         vtx[4]=minicoord(miniv3d(DATA[idx].swx,DATA[idx].swy,DATA[idx].h0+DATA[idx].dh),crs,DATA[idx].zone,DATA[idx].datum);
         vtx[5]=minicoord(miniv3d(DATA[idx].nwx,DATA[idx].nwy,DATA[idx].h0+DATA[idx].dh),crs,DATA[idx].zone,DATA[idx].datum);
         vtx[6]=minicoord(miniv3d(DATA[idx].nex,DATA[idx].ney,DATA[idx].h0+DATA[idx].dh),crs,DATA[idx].zone,DATA[idx].datum);
         vtx[7]=minicoord(miniv3d(DATA[idx].sex,DATA[idx].sey,DATA[idx].h0+DATA[idx].dh),crs,DATA[idx].zone,DATA[idx].datum);

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
               for (i=0; i<8; i++) vtx[i].convert2(CRS);

         // multiply corner vertices with 4x3 matrix
         for (i=0; i<8; i++)
            {
            v=miniv4d(vtx[i].vec.x,vtx[i].vec.y,vtx[i].vec.z,1.0);
            vtx[i].vec=miniv3d(MTXPRE[0]*v,MTXPRE[1]*v,MTXPRE[2]*v);
            }

         // check reference to layer
         if (REF[idx]!=NULL)
            for (i=0; i<8; i++) vtx[i]=REF[idx]->map_t2g(vtx[i]);

         // check orientation of tetrahedral decomposition
         if (!FLIP[idx])
            {
            // add the 4 corner tetrahedra of the actual databuf object to the mesh:

            vals.set(minival(SLOT[idx],idx,crd[0],crd[1],crd[3],crd[4],vtx[0].vec,vtx[1].vec,vtx[3].vec,vtx[4].vec));
            MESH.append(minihedron(vtx[0].vec,vtx[1].vec,vtx[3].vec,vtx[4].vec,vals));

            vals.set(minival(SLOT[idx],idx,crd[2],crd[3],crd[1],crd[6],vtx[2].vec,vtx[3].vec,vtx[1].vec,vtx[6].vec));
            MESH.append(minihedron(vtx[2].vec,vtx[3].vec,vtx[1].vec,vtx[6].vec,vals));

            vals.set(minival(SLOT[idx],idx,crd[7],crd[6],crd[4],crd[3],vtx[7].vec,vtx[6].vec,vtx[4].vec,vtx[3].vec));
            MESH.append(minihedron(vtx[7].vec,vtx[6].vec,vtx[4].vec,vtx[3].vec,vals));

            vals.set(minival(SLOT[idx],idx,crd[5],crd[4],crd[6],crd[1],vtx[5].vec,vtx[4].vec,vtx[6].vec,vtx[1].vec));
            MESH.append(minihedron(vtx[5].vec,vtx[4].vec,vtx[6].vec,vtx[1].vec,vals));

            // add the 5th center tetrahedron of the actual databuf object to the mesh:

            vals.set(minival(SLOT[idx],idx,crd[3],crd[1],crd[6],crd[4],vtx[3].vec,vtx[1].vec,vtx[6].vec,vtx[4].vec));
            MESH.append(minihedron(vtx[3].vec,vtx[1].vec,vtx[6].vec,vtx[4].vec,vals));
            }
         else
            {
            // add the 4 corner tetrahedra of the actual databuf object to the mesh:

            vals.set(minival(SLOT[idx],idx,crd[3],crd[0],crd[2],crd[7],vtx[3].vec,vtx[0].vec,vtx[2].vec,vtx[7].vec));
            MESH.append(minihedron(vtx[3].vec,vtx[0].vec,vtx[2].vec,vtx[7].vec,vals));

            vals.set(minival(SLOT[idx],idx,crd[1],crd[2],crd[0],crd[5],vtx[1].vec,vtx[2].vec,vtx[0].vec,vtx[5].vec));
            MESH.append(minihedron(vtx[1].vec,vtx[2].vec,vtx[0].vec,vtx[5].vec,vals));

            vals.set(minival(SLOT[idx],idx,crd[4],crd[7],crd[5],crd[0],vtx[4].vec,vtx[7].vec,vtx[5].vec,vtx[0].vec));
            MESH.append(minihedron(vtx[4].vec,vtx[7].vec,vtx[5].vec,vtx[0].vec,vals));

            vals.set(minival(SLOT[idx],idx,crd[6],crd[5],crd[7],crd[2],vtx[6].vec,vtx[5].vec,vtx[7].vec,vtx[2].vec));
            MESH.append(minihedron(vtx[6].vec,vtx[5].vec,vtx[7].vec,vtx[2].vec,vals));

            // add the 5th center tetrahedron of the actual databuf object to the mesh:

            vals.set(minival(SLOT[idx],idx,crd[0],crd[5],crd[2],crd[7],vtx[0].vec,vtx[5].vec,vtx[2].vec,vtx[7].vec));
            MESH.append(minihedron(vtx[0].vec,vtx[5].vec,vtx[2].vec,vtx[7].vec,vals));
            }
      }
   }

// trigger pushing the mesh for a particular time step
void datagrid::trigger(const double time)
   {
   if (!CONSTRUCTED) construct(); // construct the bsp tree at least once
   push(UNSORTED,time,MTXPOST); // push the static unsorted mesh
   }

// trigger pushing the mesh for a particular time step and eye point
void datagrid::trigger(const double time,
                       const minicoord &eye,const miniv3d &dir,
                       const float nearp,const float farp,const float fovy,const float aspect,
                       const double maxradius)
   {
   minicoord ep,epd;
   miniv3d ed;

   float factor1,factor2;

   static const double scale=1000.0;

   ep=eye;
   ed=dir;

   // transform eye point
   if (ep.type!=minicoord::MINICOORD_LINEAR)
      if (CRS!=minicoord::MINICOORD_LINEAR)
         {
         epd=ep+minicoord(scale*ed,ep.type,ep.utm_zone,ep.utm_datum);

         ep.convert2(CRS);
         epd.convert2(CRS);

         ed=(epd-ep).vec;
         ed.normalize();
         }

   // compute safety factors
   factor1=fsqrt(1.0f+fsqr(ftan(fovy/2.0f*RAD))*(1.0f+fsqr(aspect)));
   factor2=1.01f;

   if (!CONSTRUCTED) construct(); // construct the bsp tree at least once
   SORTED=BSPT2.extract(ep.vec,factor1*nearp,maxradius); // extract a non-intrusive sorted tetrahedral mesh from the bsp tree
   push(SORTED,time,MTXPOST,ep.vec,ed,factor2*nearp,farp,fovy,aspect); // push the dynamic sorted mesh
   }

// push the mesh for a particular time step
void datagrid::push(const minimesh &mesh,
                    const double time,
                    const miniv4d mtx[3])
   {
   printf("pushing mesh of size %u for time step %g with offset=(%g,%g,%g)\n",
          mesh.getsize(),time,mtx[0].w,mtx[1].w,mtx[2].w);
   }

// push the mesh for a particular time step and eye point
void datagrid::push(const minimesh &mesh,
                    const double time,
                    const miniv4d mtx[3],
                    const minicoord &eye,const miniv3d &dir,
                    const float nearp,const float farp,const float fovy,const float aspect)
   {
   printf("pushing mesh of size %u for time step %g with offset=(%g,%g,%g)\n",
          mesh.getsize(),time,mtx[0].w,mtx[1].w,mtx[2].w);

   printf("view parameters: eye=(%g,%g,%g) dir=(%g,%g,%g) nearp=%g farp=%g fovy=%g aspect=%g\n",
          eye.vec.x,eye.vec.y,eye.vec.z,dir.x,dir.y,dir.z,nearp,farp,fovy,aspect);
   }
