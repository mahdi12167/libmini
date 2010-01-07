/* Includes */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "driver.h"
#include "data.h"

/* modul-lokale Makro-Definitionen */

/* modul-lokale Typ-Deklarationen */

/* modul-lokale Variablen-Deklarationen */
float heada1,heada2,heada3,
      leftarma1,leftarma2,leftarma3,rightarma1,rightarma2,rightarma3,
      leftlega1,leftlega2,rightlega1,rightlega2,
      steera,rpm;

/* modul-lokale Funktions-Deklarationen */
void updateangle(float *angle,float spin,float min,float max)
   {if (*angle+spin>=min && *angle+spin<=max) *angle+=spin;}
