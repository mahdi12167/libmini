// (c) by Stefan Roettger

#include "minigeom.h"

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

      insert(tet.vtx[0],tet.vtx[1],tet.vtx[2],tet.vtx[3],tet.val[0]);
      insert(tet.vtx[0],tet.vtx[3],tet.vtx[1],tet.vtx[2],tet.val[0]);
      insert(tet.vtx[1],tet.vtx[3],tet.vtx[2],tet.vtx[0],tet.val[0]);
      insert(tet.vtx[2],tet.vtx[3],tet.vtx[0],tet.vtx[1],tet.val[0]);
      }
   }

// insert from triangular face
void minibspt::insert(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &p,const minitet::minival &val)
   {
   minibspt_struct node;

   node.pnt=v1;

   node.nrm=(v2-v1)/(v3-v1);
   node.nrm.normalize();

   if (node.nrm*(p-v1)<0.0) node.nrm=-node.nrm;

   node.val=val;

   node.left=node.right=0;

   insert(0,v1,v2,v3,node);
   }

// insert node
void minibspt::insert(unsigned int idx,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const minibspt_struct &node)
   {
   double d1,d2,d3;
   BOOLINT f1,f2,f3;

   if (TREE.getsize()==0) TREE.append(node);
   else
      {
      d1=(v1-TREE[idx].pnt)*TREE[idx].nrm;
      d2=(v2-TREE[idx].pnt)*TREE[idx].nrm;
      d3=(v3-TREE[idx].pnt)*TREE[idx].nrm;

      if (d1>-minigeom_base::delta && d1<minigeom_base::delta &&
          d2>-minigeom_base::delta && d2<minigeom_base::delta &&
          d3>-minigeom_base::delta && d3<minigeom_base::delta)
         if (node.val.slot==TREE[idx].val.slot) return;

      f1=(d1>-minigeom_base::delta);
      f2=(d2>-minigeom_base::delta);
      f3=(d3>-minigeom_base::delta);

      if (f1 || f2 || f3)
         if (TREE[idx].left!=0) insert(TREE[idx].left,v1,v2,v3,node);
         else
            {
            TREE.append(node);
            TREE[idx].left=TREE.getsize()-1;
            }

      if (!f1 || !f2 || !f3)
         if (TREE[idx].right!=0) insert(TREE[idx].right,v1,v2,v3,node);
         else
            {
            TREE.append(node);
            TREE[idx].right=TREE.getsize()-1;
            }
      }
   }

// extract to tetrahedral mesh
void minibspt::extract(minimesh &mesh)
   {mesh.setsize(0);}
