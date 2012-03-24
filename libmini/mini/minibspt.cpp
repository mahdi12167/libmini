// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minibspt.h"

const double minibsptree::delta=1E-3;

// default constructor
minibsptree::minibsptree()
   {
   CONSTRUCTED=FALSE;

   PHASE=0;
   STEP=0;

   GOTEYE=FALSE;
   VOLDONE=FALSE;
   }

// destructor
minibsptree::~minibsptree() {}

// clear bsp tree
void minibsptree::clear()
   {
   TREE.clear();
   MESH.clear();

   CONSTRUCTED=FALSE;

   PHASE=0;
   STEP=0;

   GOTEYE=FALSE;
   VOLDONE=FALSE;
   }

// insert from tetrahedral mesh
void minibsptree::insert(const minimesh &mesh)
   {
   if (CONSTRUCTED || PHASE!=0) ERRORMSG();

   MESH.append(mesh);
   }

// insert from tetrahedral mesh with bounding box
void minibsptree::insertbbox(const minimesh &mesh)
   {
   miniv3d bbmin,bbmax;

   if (CONSTRUCTED || PHASE!=0) ERRORMSG();

   // store mesh for later insertion
   MESH.append(mesh);

   // calculate bounding box for immediate insertion
   if (mesh.getsize()>0)
      {
      // calculate bounding box
      mesh.getbbox(bbmin,bbmax);

      // insert left face triangles
      insert(0,miniv3d(bbmin.x,bbmin.y,bbmin.z),miniv3d(bbmin.x,bbmax.y,bbmin.z),miniv3d(bbmin.x,bbmin.y,bbmax.z),miniv3d(bbmax.x,bbmin.y,bbmin.z));
      insert(0,miniv3d(bbmin.x,bbmax.y,bbmax.z),miniv3d(bbmin.x,bbmax.y,bbmin.z),miniv3d(bbmin.x,bbmin.y,bbmax.z),miniv3d(bbmax.x,bbmax.y,bbmax.z));

      // insert right face triangles
      insert(0,miniv3d(bbmax.x,bbmax.y,bbmax.z),miniv3d(bbmax.x,bbmin.y,bbmax.z),miniv3d(bbmax.x,bbmax.y,bbmin.z),miniv3d(bbmin.x,bbmax.y,bbmax.z));
      insert(0,miniv3d(bbmax.x,bbmin.y,bbmin.z),miniv3d(bbmax.x,bbmin.y,bbmax.z),miniv3d(bbmax.x,bbmax.y,bbmin.z),miniv3d(bbmin.x,bbmin.y,bbmin.z));

      // insert front face triangles
      insert(0,miniv3d(bbmin.x,bbmin.y,bbmin.z),miniv3d(bbmax.x,bbmin.y,bbmin.z),miniv3d(bbmin.x,bbmax.y,bbmin.z),miniv3d(bbmin.x,bbmin.y,bbmax.z));
      insert(0,miniv3d(bbmax.x,bbmax.y,bbmin.z),miniv3d(bbmax.x,bbmin.y,bbmin.z),miniv3d(bbmin.x,bbmax.y,bbmin.z),miniv3d(bbmax.x,bbmax.y,bbmax.z));

      // insert back face triangles
      insert(0,miniv3d(bbmax.x,bbmax.y,bbmax.z),miniv3d(bbmin.x,bbmax.y,bbmax.z),miniv3d(bbmax.x,bbmin.y,bbmax.z),miniv3d(bbmax.x,bbmax.y,bbmin.z));
      insert(0,miniv3d(bbmin.x,bbmin.y,bbmax.z),miniv3d(bbmin.x,bbmax.y,bbmax.z),miniv3d(bbmax.x,bbmin.y,bbmax.z),miniv3d(bbmin.x,bbmin.y,bbmin.z));

      // insert bottom face triangles
      insert(0,miniv3d(bbmin.x,bbmin.y,bbmin.z),miniv3d(bbmax.x,bbmin.y,bbmin.z),miniv3d(bbmin.x,bbmin.y,bbmax.z),miniv3d(bbmin.x,bbmax.y,bbmin.z));
      insert(0,miniv3d(bbmax.x,bbmin.y,bbmax.z),miniv3d(bbmax.x,bbmin.y,bbmin.z),miniv3d(bbmin.x,bbmin.y,bbmax.z),miniv3d(bbmax.x,bbmax.y,bbmax.z));

      // insert top face triangles
      insert(0,miniv3d(bbmax.x,bbmax.y,bbmax.z),miniv3d(bbmin.x,bbmax.y,bbmax.z),miniv3d(bbmax.x,bbmax.y,bbmin.z),miniv3d(bbmax.x,bbmin.y,bbmax.z));
      insert(0,miniv3d(bbmin.x,bbmax.y,bbmin.z),miniv3d(bbmin.x,bbmax.y,bbmax.z),miniv3d(bbmax.x,bbmax.y,bbmin.z),miniv3d(bbmin.x,bbmin.y,bbmin.z));
      }
   }

// get number of inserted nodes
unsigned int minibsptree::getnodes()
   {return(TREE.getsize());}

// preprocess entire input mesh
void minibsptree::preprocessall()
   {while (!preprocess());}

// preprocess input mesh one step at a time
BOOLINT minibsptree::preprocess()
   {
   if (!CONSTRUCTED)
      if (MESH.getsize()==0) CONSTRUCTED=TRUE;
      else
         switch (PHASE)
            {
            case 0:
               // phase #0: insert each face of the tetrahedral input mesh into the bsp tree
               insert1(STEP/4,STEP%4);

               if (++STEP>=4*MESH.getsize())
                  {
                  STEP=0;
                  PHASE++;
                  }

               break;
            case 1:
               // phase #1: insert each tetrahedron of the input mesh into the bsp tree
               insert2(STEP);

               if (++STEP>=MESH.getsize())
                  {
                  STEP=0;
                  PHASE++;
                  }

               break;
            case 2:
               // phase #2: intersect each node of the bsp tree
               intersect(STEP);

               if (++STEP>=TREE.getsize())
                  {
                  STEP=0;
                  PHASE++;
                  }

               break;
            case 3:
               // phase #3: clean up
               MESH.clear();

               CONSTRUCTED=TRUE;

               break;
            }

   return(CONSTRUCTED);
   }

// get preprocessing status
BOOLINT minibsptree::getstatus()
   {return(CONSTRUCTED);}

// insert tetrahedron (step #1)
void minibsptree::insert1(unsigned int idx,unsigned int face)
   {
   minihedron h;

   // get a tetrahedron
   h=MESH[idx];

   // insert one tetrahedral face as a dividing plane
   // the faces attaching to the fourth vertex are inserted first
   // therefore it is the origin of the first three dividing planes
   switch (face)
      {
      case 0: insert(0,h.vtx1,h.vtx4,h.vtx2,h.vtx3); break;
      case 1: insert(0,h.vtx2,h.vtx4,h.vtx3,h.vtx1); break;
      case 2: insert(0,h.vtx3,h.vtx4,h.vtx1,h.vtx2); break;
      case 3: insert(0,h.vtx1,h.vtx2,h.vtx3,h.vtx4); break;
      }
   }

// insert tetrahedron (step #2)
void minibsptree::insert2(unsigned int idx)
   {
   minihedron h;

   // get a tetrahedron
   h=MESH[idx];

   // assign the data coordinates of the tetrahedron to the bsp tree
   setvals(0,h.vtx1,h.vtx2,h.vtx3,h.vtx4,h.vals);
   }

// insert tetrahedral face as a dividing plane
void minibsptree::insert(unsigned int idx,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &h)
   {
   double d1,d2,d3;

   // check if the bsp tree is empty
   if (TREE.empty()) append(minigeom_plane<Scalar>(v1,v2,v3,h));
   else
      {
      // determine the distance of the tetrahedral face vertices from the actual dividing plane
      d1=TREE[idx].plane.getdistance(v1);
      d2=TREE[idx].plane.getdistance(v2);
      d3=TREE[idx].plane.getdistance(v3);

      // check if the tetrahedral face intrudes into the left half space
      if (d1>delta || d2>delta || d3>delta)
         if (TREE[idx].left!=0) insert(TREE[idx].left,v1,v2,v3,h); // insert recursively
         else
            {
            append(minigeom_plane<Scalar>(v1,v2,v3,h)); // append node to bsp tree
            TREE[idx].left=TREE.getsize()-1; // link left node from parent
            }

      // check if the tetrahedral face intrudes into the right half space
      if (d1<-delta || d2<-delta || d3<-delta)
         if (TREE[idx].right!=0) insert(TREE[idx].right,v1,v2,v3,h); // insert recursively
         else
            {
            append(minigeom_plane<Scalar>(v1,v2,v3,h)); // append node to bsp tree
            TREE[idx].right=TREE.getsize()-1; // link right node from parent
            }
      }
   }

// assign data coordinates to tetrahedral nodes
void minibsptree::setvals(unsigned int idx,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &v4,const minivals &vals)
   {
   double d1,d2,d3,d4;

   // check if the bsp tree is empty
   if (!TREE.empty())
      {
      // determine the distance of the tetrahedral vertices from the actual dividing plane
      d1=TREE[idx].plane.getdistance(v1);
      d2=TREE[idx].plane.getdistance(v2);
      d3=TREE[idx].plane.getdistance(v3);
      d4=TREE[idx].plane.getdistance(v4);

      // check if the tetrahedon intrudes into the left half space
      if (d1>delta || d2>delta || d3>delta || d4>delta)
         if (TREE[idx].left!=0) setvals(TREE[idx].left,v1,v2,v3,v4,vals); // insert recursively
         else TREE[idx].leftvals.append(vals); // append data coordinates

      // check if the tetrahedon intrudes into the right half space
      if (d1<-delta || d2<-delta || d3<-delta || d4<-delta)
         if (TREE[idx].right!=0) setvals(TREE[idx].right,v1,v2,v3,v4,vals); // insert recursively
         else TREE[idx].rightvals.append(vals); // append data coordinates
      }
   }

// append node to bsp tree
void minibsptree::append(const minigeom_plane<Scalar> &plane)
   {
   minibsptree_node node;

   node.plane=plane;
   node.left=node.right=0;

   TREE.append(node);
   }

// intersect one node of the bsp tree
void minibsptree::intersect(unsigned int idx)
   {
   minigeom_plane<Scalar> plane;
   minigeom_polyhedron<Scalar> left,right;

   // compute the left polyhedron
   plane=TREE[idx].plane;
   left=right=TREE[idx].poly;
   left.intersect(plane);

   // compute the right polyhedron
   plane.invert();
   right.intersect(plane);

   // clear the actual polyhedron
   TREE[idx].poly.clear();

   // propagate left polyhedron to the left half space
   if (TREE[idx].left!=0) TREE[TREE[idx].left].poly=left;

   // propagate right polyhedron to the right half space
   if (TREE[idx].right!=0) TREE[TREE[idx].right].poly=right;

   // break the left polyhedron into a set of connected tetrahedra
   if (TREE[idx].left==0 && TREE[idx].leftvals.getsize()>0)
      {
      // calculate the connected tetrahedra
      TREE[idx].leftmesh.append(left);

      // propagate the embedded data coordinates
      TREE[idx].leftmesh.setvals(TREE[idx].leftvals);
      TREE[idx].leftvals.clear();
      }

   // break the right polyhedron into a set of connected tetrahedra
   if (TREE[idx].right==0 && TREE[idx].rightvals.getsize()>0)
      {
      // calculate the connected tetrahedra
      TREE[idx].rightmesh.append(right);

      // propagate the embedded data coordinates
      TREE[idx].rightmesh.setvals(TREE[idx].rightvals);
      TREE[idx].rightvals.clear();
      }
   }

// extract tetrahedral mesh
minimesh minibsptree::extract()
   {
   unsigned int i;

   minimesh mesh;

   // preprocess the input mesh
   preprocessall();

   // append each tetrahedralized node to the output mesh
   for (i=0; i<TREE.getsize(); i++)
      {
      if (TREE[i].left==0) mesh.append(TREE[i].leftmesh);
      if (TREE[i].right==0) mesh.append(TREE[i].rightmesh);
      }

   return(mesh);
   }

// extract sorted tetrahedral mesh
minimesh minibsptree::extract(const miniv3d &eye,const double minradius,const double maxradius)
   {
   // preprocess the input mesh
   preprocessall();

   if (GOTEYE)
      if (eye==EYE && minradius==MINR && maxradius==MAXR) return(COLLECT); // return the previously collected mesh

   EYE=eye;
   MINR=minradius;
   MAXR=maxradius;
   GOTEYE=TRUE;

   // sort and append each tetrahedralized node to the output mesh
   COLLECT.clear();
   collect(0);

   return(COLLECT);
   }

// collect tetrahedra by descending the bsp tree
// also sort tetrahedra with respect to the eye point
// also cull tetrahedra with respect to the visibility radius
void minibsptree::collect(const unsigned int idx)
   {
   double dist;

   // check if the bsp tree is empty
   if (!TREE.empty())
      {
      // calculate the distance of the eye point to the dividing plane
      dist=TREE[idx].plane.getdistance(EYE);

      // check which half space includes the eye point
      if (dist<0.0)
         {
         // collect the left half space
         if (dist+MAXR>0.0)
            if (TREE[idx].left==0) COLLECT.append(TREE[idx].leftmesh.sort(EYE,dist+MINR>0.0)); // sort left mesh
            else collect(TREE[idx].left); // descend

         // collect the right half space
         if (TREE[idx].right==0) COLLECT.append(TREE[idx].rightmesh.sort(EYE,TRUE)); // sort right mesh
         else collect(TREE[idx].right); // descend
         }
      else
         {
         // collect the right half space
         if (dist-MAXR<0.0)
            if (TREE[idx].right==0) COLLECT.append(TREE[idx].rightmesh.sort(EYE,dist-MINR<0.0)); // sort right mesh
            else collect(TREE[idx].right); // descend

         // collect the left half space
         if (TREE[idx].left==0) COLLECT.append(TREE[idx].leftmesh.sort(EYE,TRUE)); // sort left mesh
         else collect(TREE[idx].left);
         }
      }
   }

// get volume of tetrahedralized polyhedra
double minibsptree::getvolume()
   {
   unsigned int i;

   // preprocess the input mesh
   preprocessall();

   if (VOLDONE) return(VOL);

   VOL=0.0;

   // get volume of each tetrahedralized node of the output mesh
   for (i=0; i<TREE.getsize(); i++)
      {
      if (TREE[i].left==0) VOL+=TREE[i].leftmesh.getvolume();
      if (TREE[i].right==0) VOL+=TREE[i].rightmesh.getvolume();
      }

   VOLDONE=TRUE;

   return(VOL);
   }
