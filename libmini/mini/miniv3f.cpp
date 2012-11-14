// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "miniv3f.h"

#include "miniv3d.h"
#include "miniv4f.h"
#include "miniv4d.h"

#include "ministring.h"

// conversion constructors
miniv3f::miniv3f(const miniv3d &v) {x=(float)v.x; y=(float)v.y; z=(float)v.z;}
miniv3f::miniv3f(const miniv4f &v) {x=v.x; y=v.y; z=v.z;}
miniv3f::miniv3f(const miniv4d &v) {x=(float)v.x; y=(float)v.y; z=(float)v.z;}

// string cast operator
miniv3f::operator ministring() const {return((ministring)"(" + x + "," + y + "," + z + ")");}

// serialization
ministring miniv3f::to_string() const
   {
   ministring info("miniv3f");

   info.append("(");
   info.append_float(x);
   info.append(",");
   info.append_float(y);
   info.append(",");
   info.append_float(z);
   info.append(")");

   return(info);
   }

// deserialization
void miniv3f::from_string(ministring info)
   {
   if (info.startswith("miniv3f"))
      {
      info=info.tail("miniv3f(");
      info=info.head(")");

      x=info.prefix(",").value();
      info=info.tail(",");
      y=info.prefix(",").value();
      info=info.tail(",");
      z=info.prefix(",").value();
      }
   }
