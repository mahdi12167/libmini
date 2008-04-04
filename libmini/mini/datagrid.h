// (c) by Stefan Roettger

#ifndef DATAGRID_H
#define DATAGRID_H

#include "minibase.h"

#include "minidyna.h"
#include "minimesh.h"

#include "database.h"

class datagrid
   {
   public:

   //! default constructor
   datagrid();

   //! destructor
   ~datagrid();

   //! create data brick id
   unsigned int create(BOOLINT flip=FALSE);

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

   //! construct tetrahedral mesh from the data grid
   void construct();

   protected:

   private:

   minidyna<BOOLINT> SLOT;
   minidyna<BOOLINT> FLIP;
   minidyna<databuf> DATA;

   minimesh MESH;

   BOOLINT INVALID;
   };

#endif
