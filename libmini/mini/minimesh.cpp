// (c) by Stefan Roettger

#include "minimesh.h"

// default constructor
minibspt::minibspt() {}

// destructor
minibspt::~minibspt() {}

// insert from tetrahedral mesh
void minibspt::insert(const minimesh &mesh)
   {
   unsigned int i;

   minitet tet;

   for (i=0; i<mesh.getsize(); i++)
      {
      tet=mesh[i];

      insert(tet.vtx[0],tet.vtx[1],tet.vtx[2],tet.vtx[3],tet.val);
      insert(tet.vtx[0],tet.vtx[3],tet.vtx[1],tet.vtx[2],tet.val);
      insert(tet.vtx[1],tet.vtx[3],tet.vtx[2],tet.vtx[0],tet.val);
      insert(tet.vtx[2],tet.vtx[3],tet.vtx[0],tet.vtx[1],tet.val);
      }
   }

// insert from triangular face
void minibspt::insert(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &p,const minitet::minival_type &val)
   {
   //!!
   }

// extract to tetrahedral mesh
void minibspt::extract(minimesh &mesh)
   {
   //!!
   }
