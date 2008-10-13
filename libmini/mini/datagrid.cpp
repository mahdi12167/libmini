// (c) by Stefan Roettger

#include "datagrid.h"

// default constructor
datagrid::datagrid()
   {
   CRS=minicoord::MINICOORD_ECEF;

   INVALID=FALSE;
   CONSTRUCTED=TRUE;

   DECOMPOSED=FALSE;

   PHASE=0;
   STEP=0;

   MTXPRE[0]=MTXPOST[0]=miniv4d(1.0,0.0,0.0);
   MTXPRE[1]=MTXPOST[1]=miniv4d(0.0,1.0,0.0);
   MTXPRE[2]=MTXPOST[2]=miniv4d(0.0,0.0,1.0);

   USEPRE=USEPOST=FALSE;
   }

// destructor
datagrid::~datagrid()
   {clear();}

// set coordinate system
void datagrid::setcrs(const minicoord::MINICOORD crs)
   {CRS=crs;}

// create data brick id
unsigned int datagrid::create(const unsigned int slot,
                              const BOOLINT flip,
                              const BOOLINT bbox)
   {
   unsigned int i;

   for (i=0; i<FLAG.getsize(); i++)
      if (!FLAG[i])
         {
         FLAG[i]=TRUE;
         SLOT[i]=slot;
         FLIP[i]=flip;
         BBOX[i]=bbox;
         REF[i]=NULL;
         DATA[i]=databuf();

         return(i);
         }

   FLAG.append(TRUE);
   SLOT.append(slot);
   FLIP.append(flip);
   BBOX.append(bbox);
   REF.append(NULL);
   DATA.append(databuf());

   return(FLAG.getsize()-1);
   }

// assign slot
void datagrid::assign(const unsigned int id,
                      const unsigned int slot)
   {
   if (FLAG[id])
      {
      if (!DATA[id].missing())
         if (SLOT[id]!=slot) INVALID=TRUE;

      SLOT[id]=slot;
      }
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
      {
      if (!DATA[id].missing())
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

// specify pre matrix
void datagrid::specmtxpre(const miniv4d mtx[3])
   {
   INVALID=TRUE;

   MTXPRE[0]=mtx[0];
   MTXPRE[1]=mtx[1];
   MTXPRE[2]=mtx[2];
   }

// enable usage of the pre matrix
void datagrid::usemtxpre(const BOOLINT enable)
   {USEPRE=enable;}

// specify post matrix
void datagrid::specmtxpost(const miniv4d mtx[3])
   {
   MTXPOST[0]=mtx[0];
   MTXPOST[1]=mtx[1];
   MTXPOST[2]=mtx[2];
   }

// enable usage of the post matrix
void datagrid::usemtxpost(const BOOLINT enable)
   {USEPOST=enable;}

// construct tetrahedral mesh from all data bricks
void datagrid::construct()
   {while (!preprocess());}

// preprocess tetrahedral mesh one step at a time
BOOLINT datagrid::preprocess()
   {
   BOOLINT status1,status2;

   if (INVALID)
      {
      INVALID=FALSE; // preprocessing has started
      CONSTRUCTED=FALSE; // preprocessing has not yet finished

      DECOMPOSED=FALSE; // decomposition has not yet finished

      PHASE=0;
      STEP=0;
      }

   // decompose all data bricks into tetrahedra
   if (decompose())
      {
      status1=BSPT.getstatus(); // get processing status
      status2=BSPT.preprocess(); // process bsp tree one step at a time

      if (!status1 && status2)
         {
         UNSORTED=BSPT.extract(); // extract a non-intrusive unsorted tetrahedral mesh from the bsp tree
         CONSTRUCTED=TRUE; // preprocessing has finished
         }
      }

   return(CONSTRUCTED);
   }

// decompose all data bricks into tetrahedra
BOOLINT datagrid::decompose()
   {
   if (!DECOMPOSED)
      switch (PHASE)
         {
         case 0:
            // phase #0: reset
            BSPT.clear();

            PHASE++;

            break;
         case 1:
            // phase #1: decompose one data brick into 5 tetrahedra
            MESH=decompose(STEP);

            PHASE++;

            break;
         case 2:
            // phase #2: insert the tetrahedral mesh into the bsp tree
            if (BBOX[STEP]) BSPT.insertbbox(MESH);
            else BSPT.insert(MESH);

            if (++STEP>=FLAG.getsize())
               {
               STEP=0;
               PHASE++;
               }
            else PHASE--;

            break;
         case 3:
            // phase #3: clean up
            MESH.setnull();

            STEP=0;
            PHASE=0;

            DECOMPOSED=TRUE;

            break;
         }

   return(DECOMPOSED);
   }

minimesh datagrid::decompose(unsigned int idx)
   {
   unsigned int i;

   minicoord::MINICOORD crs;

   minicoord vtx[8];
   miniv3d crd[8];

   miniv4d v;

   minivals vals;

   minimesh mesh;

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

         // multiply corner vertices with pre matrix
         if (USEPRE)
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
            mesh.append(minihedron(vtx[0].vec,vtx[1].vec,vtx[3].vec,vtx[4].vec,vals));

            vals.set(minival(SLOT[idx],idx,crd[2],crd[3],crd[1],crd[6],vtx[2].vec,vtx[3].vec,vtx[1].vec,vtx[6].vec));
            mesh.append(minihedron(vtx[2].vec,vtx[3].vec,vtx[1].vec,vtx[6].vec,vals));

            vals.set(minival(SLOT[idx],idx,crd[7],crd[6],crd[4],crd[3],vtx[7].vec,vtx[6].vec,vtx[4].vec,vtx[3].vec));
            mesh.append(minihedron(vtx[7].vec,vtx[6].vec,vtx[4].vec,vtx[3].vec,vals));

            vals.set(minival(SLOT[idx],idx,crd[5],crd[4],crd[6],crd[1],vtx[5].vec,vtx[4].vec,vtx[6].vec,vtx[1].vec));
            mesh.append(minihedron(vtx[5].vec,vtx[4].vec,vtx[6].vec,vtx[1].vec,vals));

            // add the 5th center tetrahedron of the actual databuf object to the mesh:

            vals.set(minival(SLOT[idx],idx,crd[3],crd[1],crd[6],crd[4],vtx[3].vec,vtx[1].vec,vtx[6].vec,vtx[4].vec));
            mesh.append(minihedron(vtx[3].vec,vtx[1].vec,vtx[6].vec,vtx[4].vec,vals));
            }
         else
            {
            // add the 4 corner tetrahedra of the actual databuf object to the mesh:

            vals.set(minival(SLOT[idx],idx,crd[3],crd[0],crd[2],crd[7],vtx[3].vec,vtx[0].vec,vtx[2].vec,vtx[7].vec));
            mesh.append(minihedron(vtx[3].vec,vtx[0].vec,vtx[2].vec,vtx[7].vec,vals));

            vals.set(minival(SLOT[idx],idx,crd[1],crd[2],crd[0],crd[5],vtx[1].vec,vtx[2].vec,vtx[0].vec,vtx[5].vec));
            mesh.append(minihedron(vtx[1].vec,vtx[2].vec,vtx[0].vec,vtx[5].vec,vals));

            vals.set(minival(SLOT[idx],idx,crd[4],crd[7],crd[5],crd[0],vtx[4].vec,vtx[7].vec,vtx[5].vec,vtx[0].vec));
            mesh.append(minihedron(vtx[4].vec,vtx[7].vec,vtx[5].vec,vtx[0].vec,vals));

            vals.set(minival(SLOT[idx],idx,crd[6],crd[5],crd[7],crd[2],vtx[6].vec,vtx[5].vec,vtx[7].vec,vtx[2].vec));
            mesh.append(minihedron(vtx[6].vec,vtx[5].vec,vtx[7].vec,vtx[2].vec,vals));

            // add the 5th center tetrahedron of the actual databuf object to the mesh:

            vals.set(minival(SLOT[idx],idx,crd[0],crd[5],crd[2],crd[7],vtx[0].vec,vtx[5].vec,vtx[2].vec,vtx[7].vec));
            mesh.append(minihedron(vtx[0].vec,vtx[5].vec,vtx[2].vec,vtx[7].vec,vals));
            }
      }

   return(mesh);
   }

// check if the grid is empty
BOOLINT datagrid::isempty()
   {
   if (INVALID) construct(); // construct the bsp tree
   return(UNSORTED.getsize()==0); // check extracted mesh size
   }

// check if the grid is below sea level
BOOLINT datagrid::isbelowsealevel()
   {
   unsigned int i;

   for (i=0; i<FLAG.getsize(); i++)
      if (FLAG[i])
         if (!DATA[i].missing())
            if (DATA[i].crs!=databuf::DATABUF_CRS_LINEAR)
               if (DATA[i].h0<0.0f) return(TRUE);

   return(FALSE);
   }

// trigger pushing the mesh for a particular time step
void datagrid::trigger(const double time)
   {
   if (INVALID) construct(); // construct the bsp tree
   push_post(UNSORTED,time); // push the static unsorted mesh
   }

// trigger pushing the mesh for a particular time step and eye point
void datagrid::trigger(const double time,
                       const minicoord &eye,const miniv3d &dir,
                       const float nearp,const float farp,const float fovy,const float aspect,
                       const double maxradius,
                       const int zcliptexid)
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
   factor2=1.1f;

   // extract view-dependent mesh
   if (INVALID) construct(); // construct the bsp tree
   SORTED=BSPT.extract(ep.vec,factor1*factor2*nearp,maxradius); // extract a non-intrusive sorted tetrahedral mesh from the bsp tree
   push_post(SORTED,time,ep.vec,ed,factor2*nearp,farp,fovy,aspect,zcliptexid); // push the dynamic sorted mesh
   }

// push the mesh for a particular time step
void datagrid::push_post(const minimesh &mesh,
                         const double time)
   {
   minimesh m;

   if (USEPOST)
      {
      m=mesh;
      m.multiply(MTXPOST); // multiply mesh with post matrix
      push(m,time);
      }
   else push(mesh,time);
   }

// push the mesh for a particular time step and eye point
void datagrid::push_post(minimesh &mesh,
                         const double time,
                         const miniv3d &eye,const miniv3d &dir,
                         const float nearp,const float farp,const float fovy,const float aspect,
                         const int zcliptexid)
   {
   miniv4d v;
   miniv3d e,d;

   miniv4d invtra[3];

   double scale;

   if (USEPOST)
      {
      mesh.multiply(MTXPOST); // multiply mesh with post matrix

      v=miniv4d(eye,1.0);
      e=miniv3d(MTXPOST[0]*v,MTXPOST[1]*v,MTXPOST[2]*v);

      inv_mtx(invtra,MTXPOST);
      tra_mtx(invtra,invtra);

      v=miniv4d(dir,1.0);
      d=miniv3d(invtra[0]*v,invtra[1]*v,invtra[2]*v);
      d.normalize();

      scale=pow(det_mtx(MTXPOST),1.0/3);

      push(mesh,time,e,d,nearp*scale,farp*scale,fovy,aspect,scale,zcliptexid);
      }
   else push(mesh,time,eye,dir,nearp,farp,fovy,aspect,1.0f,zcliptexid);
   }

// push the mesh for a particular time step
void datagrid::push(const minimesh &mesh,
                    const double time)
   {
   printf("pushing mesh of size %u for time step %g\n",
          mesh.getsize(),time);
   }

// push the mesh for a particular time step and eye point
void datagrid::push(const minimesh &mesh,
                    const double time,
                    const miniv3d &eye,const miniv3d &dir,
                    const float nearp,const float farp,const float fovy,const float aspect,
                    const float scale,
                    const int zcliptexid)
   {
   printf("pushing mesh of size %u for time step %g\n",
          mesh.getsize(),time);

   printf("view parameters: eye=(%g,%g,%g) dir=(%g,%g,%g) nearp=%g farp=%g fovy=%g aspect=%g scale=%g zcliptexid=%d\n",
          eye.x,eye.y,eye.z,dir.x,dir.y,dir.z,nearp,farp,fovy,aspect,scale,zcliptexid);
   }

// get a particular data brick
const databuf *datagrid::getdata(const unsigned int id)
   {
   if (FLAG[id])
      if (!DATA[id].missing()) return(&DATA[id]);

   return(NULL);
   }
