// (c) by Stefan Roettger

#include "datagrid.h"

// default constructor
datagrid::datagrid()
   {
   // configurable parameters:

   GPARAMS.crs=minicoord::MINICOORD_ECEF;

   // initialize state:

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

// create data brick id
unsigned int datagrid::create(const unsigned int slot,
                              const BOOLINT flip)
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

// construct tetrahedral mesh from the data grid
void datagrid::construct()
   {
   unsigned int i,j;

   unsigned int num,swizzle,act;

   minicoord::MINICOORD crs;

   minicoord vtx[8];
   miniv3d crd[8];

   miniv4d v;

   minitet tet;
   minival_array val;

   if (INVALID)
      {
      MESH.setnull();

      num=FLAG.getsize();

      // calculate swizzle constant
      for (swizzle=13; gcd(num,swizzle)!=1; swizzle+=2);

      // process all databuf objects
      for (i=0; i<num; i++)
         {
         act=(swizzle*i)%num;

         // check if object at actual swizzled position is valid
         if (FLAG[act])
            {
            // check coordinate system of actual databuf object
            if (DATA[act].crs==0) crs=minicoord::MINICOORD_LINEAR;
            else if (DATA[act].crs==1) crs=minicoord::MINICOORD_LLH;
            else if (DATA[act].crs==2) crs=minicoord::MINICOORD_UTM;
            else ERRORMSG();

            // determine corner vertices of actual object:

            vtx[0]=minicoord(miniv3d(DATA[act].swx,DATA[act].swy,DATA[act].h0),crs,DATA[act].zone,DATA[act].datum);
            vtx[1]=minicoord(miniv3d(DATA[act].nwx,DATA[act].nwy,DATA[act].h0),crs,DATA[act].zone,DATA[act].datum);
            vtx[2]=minicoord(miniv3d(DATA[act].nex,DATA[act].ney,DATA[act].h0),crs,DATA[act].zone,DATA[act].datum);
            vtx[3]=minicoord(miniv3d(DATA[act].sex,DATA[act].sey,DATA[act].h0),crs,DATA[act].zone,DATA[act].datum);

            vtx[4]=minicoord(miniv3d(DATA[act].swx,DATA[act].swy,DATA[act].h0+DATA[act].dh),crs,DATA[act].zone,DATA[act].datum);
            vtx[5]=minicoord(miniv3d(DATA[act].nwx,DATA[act].nwy,DATA[act].h0+DATA[act].dh),crs,DATA[act].zone,DATA[act].datum);
            vtx[6]=minicoord(miniv3d(DATA[act].nex,DATA[act].ney,DATA[act].h0+DATA[act].dh),crs,DATA[act].zone,DATA[act].datum);
            vtx[7]=minicoord(miniv3d(DATA[act].sex,DATA[act].sey,DATA[act].h0+DATA[act].dh),crs,DATA[act].zone,DATA[act].datum);

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
               if (GPARAMS.crs!=minicoord::MINICOORD_LINEAR)
                  for (j=0; j<8; j++) vtx[j].convert2(GPARAMS.crs);

            // multiply corner vertices with 4x3 matrix
            if (!IDENTITY)
               for (j=0; j<8; j++)
                  {
                  v=miniv4d(vtx[j].vec.x,vtx[j].vec.y,vtx[j].vec.z,1.0);
                  vtx[j].vec=miniv3d(MTX[0]*v,MTX[1]*v,MTX[2]*v);
                  }

            // check orientation of tetrahedral decomposition
            if (!FLIP[act])
               {
               // add the 4 corner tetrahedra of the actual databuf object to the mesh:

               val.setv(minival(SLOT[act],crd[0],crd[1],crd[3],crd[4]));
               tet=minitet(vtx[0].vec,vtx[1].vec,vtx[3].vec,vtx[4].vec,val);
               MESH.append(tet);

               val.setv(minival(SLOT[act],crd[2],crd[3],crd[1],crd[6]));
               tet=minitet(vtx[2].vec,vtx[3].vec,vtx[1].vec,vtx[6].vec,val);
               MESH.append(tet);

               val.setv(minival(SLOT[act],crd[7],crd[6],crd[4],crd[3]));
               tet=minitet(vtx[7].vec,vtx[6].vec,vtx[4].vec,vtx[3].vec,val);
               MESH.append(tet);

               val.setv(minival(SLOT[act],crd[5],crd[4],crd[6],crd[1]));
               tet=minitet(vtx[5].vec,vtx[4].vec,vtx[6].vec,vtx[1].vec,val);
               MESH.append(tet);

               // add the 5th center tetrahedron of the actual databuf object to the mesh:

               val.setv(minival(SLOT[act],crd[3],crd[1],crd[6],crd[4]));
               tet=minitet(vtx[3].vec,vtx[1].vec,vtx[6].vec,vtx[4].vec,val);
               MESH.append(tet);
               }
            else
               {
               // add the 4 corner tetrahedra of the actual databuf object to the mesh:

               val.setv(minival(SLOT[act],crd[3],crd[0],crd[2],crd[7]));
               tet=minitet(vtx[3].vec,vtx[0].vec,vtx[2].vec,vtx[7].vec,val);
               MESH.append(tet);

               val.setv(minival(SLOT[act],crd[1],crd[2],crd[0],crd[5]));
               tet=minitet(vtx[1].vec,vtx[2].vec,vtx[0].vec,vtx[5].vec,val);
               MESH.append(tet);

               val.setv(minival(SLOT[act],crd[4],crd[7],crd[5],crd[0]));
               tet=minitet(vtx[4].vec,vtx[7].vec,vtx[5].vec,vtx[0].vec,val);
               MESH.append(tet);

               val.setv(minival(SLOT[act],crd[6],crd[5],crd[7],crd[2]));
               tet=minitet(vtx[6].vec,vtx[5].vec,vtx[7].vec,vtx[2].vec,val);
               MESH.append(tet);

               // add the 5th center tetrahedron of the actual databuf object to the mesh:

               val.setv(minival(SLOT[act],crd[0],crd[5],crd[2],crd[7]));
               tet=minitet(vtx[0].vec,vtx[5].vec,vtx[2].vec,vtx[7].vec,val);
               MESH.append(tet);
               }
            }
         }

      BSPT.clear(); // clear the bsp tree
      BSPT.insert(MESH); // insert the entire tetrahedral mesh into bsp tree
      BSPT.extract(TETS); // extract a non-intrusive tetrahedral mesh from bsp tree
      BSPT.extract(POLY); // extract a non-intrusive polyhedral tree from bsp tree

      INVALID=FALSE;
      }
   }

// trigger pushing the mesh for a particular time step
void datagrid::trigger(const double time,const BOOLINT tree)
   {
   construct();

   if (!tree) push(TETS,time);
   else push(POLY,time);
   }

// push the mesh for a particular time step
void datagrid::push(const minimesh &mesh,const double time)
   {printf("pushing mesh of size %u for time step %g\n",mesh.getsize(),time);}

// push the tree for a particular time step
void datagrid::push(const minibspt &tree,const double time)
   {printf("pushing tree of size %u for time step %g\n",tree.getsize(),time);}

// greatest common divisor
unsigned int datagrid::gcd(const unsigned int a,const unsigned int b)
   {
   if (b==0) return(a);
   else return(gcd(b,a%b));
   }
