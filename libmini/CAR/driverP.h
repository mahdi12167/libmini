/* Includes */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "driver.h"
#include "XWin.h"
#include "OGL.h"

/* modul-lokale Makro-Definitionen */
#define INITIAL_WIDTH  (600)
#define INITIAL_HEIGHT (INITIAL_WIDTH*9/16)

/* modul-lokale Typ-Deklarationen */

/* modul-lokale Variablen-Deklarationen */
char output='O';

int viewportwidth=INITIAL_WIDTH,
    viewportheight=INITIAL_HEIGHT;

vertex v_0=c_0;
matrix M_0=C_0,M_1=C_1;

int fanstate;

float mx,my,mz,
      lx,ly,lz;

/* modul-lokale Funktions-Deklarationen */
