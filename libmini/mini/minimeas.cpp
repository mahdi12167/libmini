// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minimeas.h"

// serialization
ministring minimeas::to_string() const
   {
   ministring info("minimeas");

   info.append("(");
   info.append(minicoord::to_string());
   info.append(",");
   info.append_double(accuracy);
   info.append(",");
   info.append_double(velocity);
   info.append(",");
   info.append_double(heading);
   info.append(")");

   return(info);
   }

// deserialization
void minimeas::from_string(ministring &info)
   {
   if (info.startswith("minimeas"))
      {
      info=info.tail("minimeas(");
      minicoord::from_string(info);
      info=info.tail(",");
      accuracy=info.prefix(",").value();
      info=info.tail(",");
      velocity=info.prefix(",").value();
      info=info.tail(",");
      heading=info.prefix(")").value();
      info=info.tail(")");
      }
   }