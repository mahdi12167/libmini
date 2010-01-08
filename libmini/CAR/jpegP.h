/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include "jpeglib.h"
#include "jpeg.h"

/* modul-lokale Makro-Definitionen */
#define ERROR()\
   {\
   fprintf(stderr,"fatal error in %s at line %d!\n",__FILE__,__LINE__);\
   exit(EXIT_FAILURE);\
   }

/* modul-lokale Typ-Deklarationen */

/* modul-lokale Variablen-Deklarationen */

/* modul-lokale Funktions-Deklarationen */
