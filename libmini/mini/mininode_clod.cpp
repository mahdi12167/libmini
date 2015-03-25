// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "mininode_clod.h"

// default constructor
mininode_geometry_clod::mininode_geometry_clod()
   : mininode_geometry(0,3,0),
     miniCLOD()
   {set_zscale();}

// destructor
mininode_geometry_clod::~mininode_geometry_clod()
   {}

// set path
void mininode_geometry_clod::set(const minipath &path)
   {miniCLOD::set(path);}

// read path
void mininode_geometry_clod::read(const std::string &csv,
                                  double max_delta,
                                  double max_length,
                                  double min_accuracy,
                                  int orb)
   {
   miniCLOD::read(csv,
                  max_delta,
                  max_length,
                  min_accuracy,
                  orb);
   }

// load path
void mininode_geometry_clod::load(ministring filename,
             double max_delta,
             double max_length,
             double min_accuracy,
             int orb)
   {
   miniCLOD::load(filename,
                  max_delta,
                  max_length,
                  min_accuracy,
                  orb);
   }

// create geometry from actual view point
void mininode_geometry_clod::create(double maxdev,double atdist,
                                    double maxwidth,
                                    double minv,double maxv,double sat,double val,
                                    double weight,double start,
                                    int update)
   {
   miniCLOD::create(mininode_culling::peek_view(),
                    maxdev,atdist,
                    maxwidth,
                    minv,maxv,sat,val,
                    weight,start,
                    update);
   }

// pre-traversal method
void mininode_geometry_clod::traverse_pre()
   {
   create_inc(mininode_culling::peek_view()); // update path incrementally
   mininode_geometry::traverse_pre(); // traverse base class
   }

// post-updated callback
void mininode_geometry_clod::updated(const std::vector<mini3D::joint_struct> &points)
   {
   *(mininode_geometry *)this=mininode_geometry_band(points);
   setZscale(zscale);
   }

// serialize node to string
ministring mininode_geometry_clod::to_string()
   {
   ministring info("mininode_geometry_clod");

   info.append("(");
   info.append_int(use_color);
   info.append(",");
   info.append(rgba.to_string());
   info.append(",");
   info.append_int(shown);
   info.append(",");
   info.append(name);
   info.append(",");
   info.append_float(zscale);
   info.append(",");
   info.append(getpath()->to_string());
   info.append(")");

   return(info);
   }

// deserialize node from string
BOOLINT mininode_geometry_clod::from_string(ministring &info)
   {
   if (info.startswith("mininode_geometry_clod("))
      {
      info=info.tail("mininode_geometry_clod(");

      use_color=info.prefix(",").value_int();
      info=info.tail(",");
      rgba.from_string(info);
      info=info.tail(",");
      shown=info.prefix(",").value_int();
      info=info.tail(",");
      name=info.prefix(",");
      info=info.tail(",");
      zscale=info.prefix(",").value();
      info=info.tail(",");
      getpath()->from_string(info);

      info=info.tail(")");

      return(TRUE);
      }

   return(FALSE);
   }
