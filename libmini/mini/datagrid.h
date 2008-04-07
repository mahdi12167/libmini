// (c) by Stefan Roettger

#ifndef DATAGRID_H
#define DATAGRID_H

#include "minibase.h"

#include "miniv3d.h"
#include "miniv4d.h"

#include "minidyna.h"
#include "minimesh.h"
#include "miniwarp.h"

#include "database.h"

class datagrid
   {
   public:

   struct DATAGRID_PARAMS_STRUCT
      {
      // configurable parameters:

      minicoord::MINICOORD crs;
      };

   typedef DATAGRID_PARAMS_STRUCT DATAGRID_PARAMS;

   //! default constructor
   datagrid();

   //! destructor
   virtual ~datagrid();

   //! create data brick id
   unsigned int create(unsigned int slot, // data slot
                       BOOLINT flip=FALSE); // decomposition flip

   //! get parameters
   void get(DATAGRID_PARAMS &gparams);

   //! set parameters
   void set(DATAGRID_PARAMS &gparams);

   //! get parameters
   DATAGRID_PARAMS *get() {return(&GPARAMS);}

   //! set parameters
   void set(DATAGRID_PARAMS *gparams) {set(*gparams);}

   //! load data
   void load(unsigned int id, // data brick id
             const databuf &buf); // data buffer

   //! remove data
   void remove(unsigned int id); // data brick id

   //! move data
   void move(unsigned int id, // data brick id
             float swx,float swy, // SW corner of data brick
             float nwx,float nwy, // NW corner of data brick
             float nex,float ney, // NE corner of data brick
             float sex,float sey, // SE corner of data brick
             float h0,float dh,  // base elevation and height of data brick
             float t0,float dt); // time frame start and exposure time

   //! apply matrix
   void applymtx(const miniv4d mtx[3]);

   //! construct tetrahedral mesh from the data grid
   void construct();

   //! trigger pushing the mesh for a particular time step
   void trigger(double time);

   //! push the mesh for a particular time step
   virtual void push(const minimesh &mesh,double time);

   protected:

   DATAGRID_PARAMS GPARAMS;

   minidyna<BOOLINT> FLAG;
   minidyna<unsigned int> SLOT;
   minidyna<BOOLINT> FLIP;

   minidyna<databuf> DATA;

   private:

   minimesh MESH;
   minibspt BSPT;
   minimesh TETS;

   miniv4d ID[3],MTX[3];
   BOOLINT IDENTITY;

   BOOLINT INVALID;

   unsigned int datagrid::gcd(unsigned int a,unsigned int b);
   };

#endif
