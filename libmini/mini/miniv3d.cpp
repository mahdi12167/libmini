// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "miniv3d.h"

#include "miniv3f.h"
#include "miniv4d.h"
#include "miniv4f.h"

#include "ministring.h"

// conversion constructors
miniv3d::miniv3d(const miniv3f &v) {x=(double)v.x; y=(double)v.y; z=(double)v.z;}
miniv3d::miniv3d(const miniv4d &v) {x=v.x; y=v.y; z=v.z;}
miniv3d::miniv3d(const miniv4f &v) {x=(double)v.x; y=(double)v.y; z=(double)v.z;}

// string cast operator
miniv3d::operator ministring() const {return((ministring)"(" + x + "," + y + "," + z + ")");}

// serialization
ministring miniv3d::to_string() const
   {
   ministring info("miniv3d");

   info.append("(");
   info.append_double(x);
   info.append(",");
   info.append_double(y);
   info.append(",");
   info.append_double(z);
   info.append(")");

   return(info);
   }

// deserialization
void miniv3d::from_string(ministring info)
   {
   if (info.startswith("miniv3d"))
      {
      info=info.tail("miniv3d(");
      info=info.head(")");

      x=info.prefix(",").value();
      info=info.tail(",");
      y=info.prefix(",").value();
      info=info.tail(",");
      z=info.prefix(",").value();
      }
   }
