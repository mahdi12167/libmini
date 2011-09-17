// (c) by Stefan Roettger

#include "viewer.h"

void Viewer::render_ecef_geometry()
   {
   // render plain globe for z-values:

   static miniglobe globe;

   static const int gltess=32;
   static const double glfactor=0.999;

   globe.settess(gltess);
   globe.setscale(glfactor);

   disableRGBAwriting();
   globe.render();
   enableRGBAwriting();

   // render ecef z-axis:

   linewidth(2);
   enablelinesmooth();

   static const miniv3d zacolor(0.25,0.25,0.5);

   color(zacolor);
   renderline(miniv3d(0.0,0.0,-1.1*miniearth::EARTH_radius),
              miniv3d(0.0,0.0,-miniearth::EARTH_radius));
   renderline(miniv3d(0.0,0.0,miniearth::EARTH_radius),
              miniv3d(0.0,0.0,1.1*miniearth::EARTH_radius));

   // render equator:

   linewidth(1);
   disableZwriting();

   mtxproj();
   mtxpush();
   mtxscale(0.95,0.95,0.95); // prevent Z-fighting

   static const int eqlines=100;
   static const miniv3d eqcolor(0.25,0.25,0.25);

   color(eqcolor);
   for (int i=0; i<=eqlines; i++)
      {
      minicoord c(miniv3d((double)i/eqlines*360*3600,0.0,0.0),minicoord::MINICOORD_LLH);
      c.convert2(minicoord::MINICOORD_ECEF);

      static minicoord c0;

      if (i>0) renderline(c0.vec,c.vec);
      c0=c;
      }

   mtxpop();
   mtxmodel();

   enableZwriting();

   disablelinesmooth();
   }
