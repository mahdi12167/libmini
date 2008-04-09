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

   //! get parameters
   void get(DATAGRID_PARAMS &gparams);

   //! set parameters
   void set(DATAGRID_PARAMS &gparams);

   //! get parameters
   DATAGRID_PARAMS *get() {return(&GPARAMS);}

   //! set parameters
   void set(DATAGRID_PARAMS *gparams) {set(*gparams);}

   //! create data brick id
   unsigned int create(const unsigned int slot, // data slot
                       const BOOLINT flip=FALSE); // decomposition flip

   //! load data
   void load(const unsigned int id, // data brick id
             const databuf &buf); // data buffer

   //! remove data
   void remove(const unsigned int id); // data brick id

   //! move data
   void move(const unsigned int id, // data brick id
             const float swx,const float swy, // SW corner of data brick
             const float nwx,const float nwy, // NW corner of data brick
             const float nex,const float ney, // NE corner of data brick
             const float sex,const float sey, // SE corner of data brick
             const float h0,const float dh,  // base elevation and height of data brick
             const float t0,const float dt); // time frame start and exposure time

   //! apply matrix
   void applymtx(const miniv4d mtx[3]);

   //! construct tetrahedral mesh from the data grid
   void construct();

   //! trigger pushing the mesh for a particular time step
   void trigger(const double time);

   //! trigger pushing the mesh for a particular time step and eye point
   void trigger(const double time,const miniv3d &eye);

   //! push the mesh for a particular time step
   virtual void push(const minimesh &mesh,const double time);

   protected:

   DATAGRID_PARAMS GPARAMS;

   minidyna<BOOLINT> FLAG;
   minidyna<unsigned int> SLOT;
   minidyna<BOOLINT> FLIP;

   minidyna<databuf> DATA;

   private:

   minimesh MESH;
   minibsptree BSPT;
   minimesh UNSORTED;
   minimesh SORTED;

   miniv4d ID[3],MTX[3];
   BOOLINT IDENTITY;

   BOOLINT INVALID;

   unsigned int gcd(const unsigned int a,const unsigned int b);
   };

#endif
