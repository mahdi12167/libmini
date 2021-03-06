// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIBSPT_H
#define MINIBSPT_H

#include "ostream"

#include "minibase.h"

#include "miniv3d.h"

#include "minidyna.h"
#include "minigeom.h"

#include "minimesh.h"

//! binary space partition tree
class minibsptree
   {
   protected:

   static const double delta;

   public:

   //! default constructor
   minibsptree();

   //! destructor
   ~minibsptree();

   //! clear bsp tree
   void clear();

   //! insert from tetrahedral mesh
   void insert(const minimesh &mesh);

   //! insert from tetrahedral mesh with bounding box
   void insertbbox(const minimesh &mesh);

   //! get number of inserted nodes
   unsigned int getnodes();

   //! preprocess entire input mesh
   void preprocessall();

   //! preprocess input mesh one step at a time
   BOOLINT preprocess();

   //! get preprocessing status
   BOOLINT getstatus();

   //! process input mesh and extract tetrahedral output mesh
   minimesh extract();

   //! process input mesh and extract sorted tetrahedral output mesh
   minimesh extract(const miniv3d &eye,const double minradius=0.0,const double maxradius=MAXFLOAT);

   //! get volume of tetrahedralized polyhedra
   double getvolume();

   private:

   typedef minimesh::Scalar Scalar;

   struct minibsptree_node
      {
      minigeom_plane<Scalar> plane;
      unsigned int left,right;

      minivals leftvals,rightvals;
      minigeom_polyhedron<Scalar> poly;
      minimesh leftmesh,rightmesh;
      };

   minidyna<minibsptree_node> TREE;
   BOOLINT CONSTRUCTED;

   minimesh MESH;
   unsigned int PHASE,STEP;

   miniv3d EYE;
   double MINR,MAXR;
   BOOLINT GOTEYE;

   minimesh COLLECT;

   double VOL;
   BOOLINT VOLDONE;

   void insert1(unsigned int idx,unsigned int face);
   void insert2(unsigned int idx);

   void insert(unsigned int idx,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &h);
   inline void append(const minigeom_plane<Scalar> &plane);

   void setvals(unsigned int idx,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &v4,const minivals &vals);

   void intersect(unsigned int idx);

   void collect(const unsigned int idx);

   friend inline std::ostream& operator << (std::ostream &out,const minibsptree &tree);
   friend inline std::ostream& operator << (std::ostream &out,const minibsptree_node &node);
   };

//! stream output
inline std::ostream& operator << (std::ostream &out,const minibsptree &tree)
   {
   unsigned int i;

   out << "minibsptree[" << std::endl;

   for (i=0; i<tree.TREE.getsize(); i++) out << i << ": " << tree.TREE[i] << std::endl;

   out << "]" << std::endl;

   return(out);
   }

//! stream output
inline std::ostream& operator << (std::ostream &out,const minibsptree::minibsptree_node &node)
   {
   out << "node( plane=" << node.plane << ", left=" << node.left << ", right=" << node.right;
   if (node.leftvals.getsize()>0) out << ", leftvals=" << node.leftvals;
   if (node.rightvals.getsize()>0) out << ", rightvals=" << node.rightvals;
   out << " )";

   return(out);
   }

#endif
