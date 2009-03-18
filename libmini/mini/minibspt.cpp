// (c) by Stefan Roettger

#include "minisort.h"

#include "minibspt.h"

// default constructor
minibsptree::minibsptree()
   {
   CONSTRUCTED=FALSE;

   PHASE=0;
   STEP=0;

   GOTEYE=FALSE;
   VOLDONE=FALSE;

   TREEDEBUG=FALSE;
   }

// destructor
minibsptree::~minibsptree() {}

// clear bsp tree
void minibsptree::clear()
   {
   TREE.setnull();
   MESH.setnull();

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

// preprocess entire input mesh
void minibsptree::preprocessall()
   {while (!preprocess());}

// preprocess input mesh one step at a time
BOOLINT minibsptree::preprocess()
   {
   unsigned int idx;

   if (!CONSTRUCTED)
      if (MESH.getsize()==0) CONSTRUCTED=TRUE;
      else
         switch (PHASE)
            {
            case 0:
               // phase #0: calculate the tetrahedral face remapping
               MAP.setsize(4*MESH.getsize());
               remap();

               PHASE++;

               break;
            case 1:
               // phase #1: insert each face of the tetrahedral input mesh into the bsp tree
               idx=MAP[STEP].map2;
               insert1(idx/4,idx%4);

               if (++STEP>=MAP.getsize())
                  {
                  STEP=0;
                  PHASE++;
                  }

               break;
            case 2:
               // phase #2: insert each tetrahedron of the input mesh into the bsp tree
               insert2(STEP);

               if (++STEP>=MESH.getsize())
                  {
                  STEP=0;
                  PHASE++;
                  }

               break;
            case 3:
               // phase #3: intersect each node of the bsp tree
               intersect(STEP);

               if (++STEP>=TREE.getsize())
                  {
                  STEP=0;
                  PHASE++;
                  }

               break;
            case 4:
               // phase #4: clean up
               MESH.setnull();
               MAP.setnull();

               CONSTRUCTED=TRUE;

               break;
            }

   return(CONSTRUCTED);
   }

// get preprocessing status
BOOLINT minibsptree::getstatus()
   {return(CONSTRUCTED);}

// comparison operator of face priorities
inline int operator < (const minibsptree::minibsptree_face &a,const minibsptree::minibsptree_face &b)
   {return(a.prio<b.prio);}

// calculate the tetrahedral face remapping
void minibsptree::remap()
   {
   unsigned int i;

   unsigned int swizzle;
   unsigned int idx;

   static const unsigned int prime=271;

   // calculate the coprime swizzle constant
   for (swizzle=prime; gcd(MAP.getsize(),swizzle)!=1; swizzle+=2);

   // initialize the tetrahedral face remapping
   for (i=0; i<MAP.getsize(); i++) MAP[i].map2=i;

   // calculate the priority of the tetrahedral faces
   for (i=0; i<MAP.getsize(); i++)
      {
      idx=(swizzle*i)%MAP.getsize(); // swizzle the actual index
      MAP[i].prio=idx; // store swizzled index as priority
      }

   // calculate the tetrahedral face remapping by sorting the face priorities
   shellsort<minibsptree_face>(MAP);
   }

// insert tetrahedron (phase #1)
void minibsptree::insert1(unsigned int idx,unsigned int face)
   {
   minihedron h;

   // get a tetrahedron
   h=MESH[idx];

   // insert one tetrahedral face as a dividing plane
   switch (face)
      {
      case 0: insert(0,h.vtx1,h.vtx2,h.vtx3,h.vtx4); break;
      case 1: insert(0,h.vtx1,h.vtx4,h.vtx2,h.vtx3); break;
      case 2: insert(0,h.vtx2,h.vtx4,h.vtx3,h.vtx1); break;
      case 3: insert(0,h.vtx3,h.vtx4,h.vtx1,h.vtx2); break;
      }
   }

// insert tetrahedron (phase #2)
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
   if (TREE.isnull()) append(minigeom_plane(v1,v2,v3,h));
   else
      {
      // determine the distance of the tetrahedral face vertices from the actual dividing plane
      d1=TREE[idx].plane.getdistance(v1);
      d2=TREE[idx].plane.getdistance(v2);
      d3=TREE[idx].plane.getdistance(v3);

      // check if the dividing plane is already existing
      if (FABS(d1)>minigeom_base::delta || FABS(d2)>minigeom_base::delta || FABS(d3)>minigeom_base::delta)
         {
         // check if the tetrahedral face intrudes into the left half space
         if (d1>minigeom_base::delta || d2>minigeom_base::delta || d3>minigeom_base::delta)
            if (TREE[idx].left!=0) insert(TREE[idx].left,v1,v2,v3,h); // insert recursively
            else
               {
               append(minigeom_plane(v1,v2,v3,h)); // append node to bsp tree
               TREE[idx].left=TREE.getsize()-1; // link left node from parent
               }

         // check if the tetrahedral face intrudes into the right half space
         if (d1<-minigeom_base::delta || d2<-minigeom_base::delta || d3<-minigeom_base::delta)
            if (TREE[idx].right!=0) insert(TREE[idx].right,v1,v2,v3,h); // insert recursively
            else
               {
               append(minigeom_plane(v1,v2,v3,h)); // append node to bsp tree
               TREE[idx].right=TREE.getsize()-1; // link right node from parent
               }
         }
      }
   }

// assign data coordinates to tetrahedral nodes
void minibsptree::setvals(unsigned int idx,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &v4,const minivals &vals)
   {
   double d1,d2,d3,d4;

   // check if the bsp tree is empty
   if (!TREE.isnull())
      {
      // determine the distance of the tetrahedral vertices from the actual dividing plane
      d1=TREE[idx].plane.getdistance(v1);
      d2=TREE[idx].plane.getdistance(v2);
      d3=TREE[idx].plane.getdistance(v3);
      d4=TREE[idx].plane.getdistance(v4);

      // check if the tetrahedon intrudes into the left half space
      if (d1>minigeom_base::delta || d2>minigeom_base::delta || d3>minigeom_base::delta || d4>minigeom_base::delta)
         if (TREE[idx].left!=0) setvals(TREE[idx].left,v1,v2,v3,v4,vals); // insert recursively
         else TREE[idx].leftvals.append(vals); // append data coordinates

      // check if the tetrahedon intrudes into the right half space
      if (d1<-minigeom_base::delta || d2<-minigeom_base::delta || d3<-minigeom_base::delta || d4<-minigeom_base::delta)
         if (TREE[idx].right!=0) setvals(TREE[idx].right,v1,v2,v3,v4,vals); // insert recursively
         else TREE[idx].rightvals.append(vals); // append data coordinates
      }
   }

// append node to bsp tree
void minibsptree::append(const minigeom_plane &plane)
   {
   minibsptree_node node;

   node.plane=plane;
   node.left=node.right=0;

   TREE.append(node);
   }

// intersect one node of the bsp tree
void minibsptree::intersect(unsigned int idx)
   {
   minigeom_plane plane;
   minigeom_polyhedron poly;

   // intersect the left half space
   if (TREE[idx].left!=0)
      {
      // add the actual half space to the left child's polyhedron
      TREE[TREE[idx].left].poly=TREE[idx].poly;
      TREE[TREE[idx].left].poly.intersect(TREE[idx].plane);
      }

   // intersect the right half space
   if (TREE[idx].right!=0)
      {
      // invert the actual half space
      plane=TREE[idx].plane;
      plane.invert();

      // add the actual inverted half space to the right child's polyhedron
      TREE[TREE[idx].right].poly=TREE[idx].poly;
      TREE[TREE[idx].right].poly.intersect(plane);
      }

   // break the left half space into a set of connected tetrahedra
   if (TREE[idx].left==0 && TREE[idx].leftvals.getsize()>0)
      {
      // add the actual half space to the node's polyhedron
      poly=TREE[idx].poly;
      poly.intersect(TREE[idx].plane);

      // calculate the connected tetrahedra
      TREE[idx].leftmesh.append(poly);

      // propagate the embedded data coordinates
      TREE[idx].leftmesh.setvals(TREE[idx].leftvals);
      TREE[idx].leftvals.setnull();
      }

   // break the right half space into a set of connected tetrahedra
   if (TREE[idx].right==0 && TREE[idx].rightvals.getsize()>0)
      {
      // invert the actual half space
      plane=TREE[idx].plane;
      plane.invert();

      // add the actual inverted half space to the node's polyhedron
      poly=TREE[idx].poly;
      poly.intersect(plane);

      // calculate the connected tetrahedra
      TREE[idx].rightmesh.append(poly);

      // propagate the embedded data coordinates
      TREE[idx].rightmesh.setvals(TREE[idx].rightvals);
      TREE[idx].rightvals.setnull();
      }

   // clear the actual polyhedron
   TREE[idx].poly.clear();
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

   // debug output
   if (TREEDEBUG) std::cout << mesh;

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
   COLLECT.setnull();
   collect(0);

   // debug output
   if (TREEDEBUG) std::cout << COLLECT;

   return(COLLECT);
   }

// collect tetrahedra by descending the bsp tree
// also sort tetrahedra with respect to the eye point
// also cull tetrahedra with respect to the visibility radius
void minibsptree::collect(const unsigned int idx)
   {
   double dist;

   // check if the bsp tree is empty
   if (!TREE.isnull())
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
