// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "mininode_path.h"

// default constructor
mininode_geometry_path::mininode_geometry_path(double width)
   : mininode_geometry(0,3,0),
     wdt(width)
   {set_zscale();}

// constructor
mininode_geometry_path::mininode_geometry_path(ministring filename,double width)
   : mininode_geometry(0,3,0),
     wdt(width)
   {
   set_zscale();
   path.load(filename);
   recreate(width);
   }

// destructor
mininode_geometry_path::~mininode_geometry_path()
   {}

// recreate geometry with a particular band width
void mininode_geometry_path::recreate(double width)
   {
   *(mininode_geometry *)this=mininode_geometry_band_path(path,width);
   setZscale(zscale);
   }

// serialize node to string
ministring mininode_geometry_path::to_string()
   {
   ministring info("mininode_geometry_path");

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
   info.append(path.to_string());
   info.append(")");

   return(info);
   }

// deserialize node from string
BOOLINT mininode_geometry_path::from_string(ministring &info)
   {
   if (info.startswith("mininode_geometry_path("))
      {
      info=info.tail("mininode_geometry_path(");

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
      path.from_string(info);

      info=info.tail(")");

      recreate(wdt);

      return(TRUE);
      }

   return(FALSE);
   }
