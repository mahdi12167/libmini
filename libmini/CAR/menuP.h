/* Includes */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver.h"
#include "menu.h"

/* modul-lokale Makro-Definitionen */
#define checkmenu()\
   if (!menu || (menu->type!=MEN_ROOT && menu->type!=MEN_SUB)) ERROR()

#define newmem()\
   if ((new=(menItem *)calloc(1,sizeof(menItem)))==NULL) ERROR()

#define lastitem()\
   if (menu->items==NULL)\
      {\
      menu->items=new;\
      actual=NULL;\
      }\
   else\
      {\
      actual=menu->items;\
      while (actual->next!=NULL) actual=actual->next;\
      actual->next=new;\
      }

/* modul-lokale Typ-Deklarationen */

/* modul-lokale Variablen-Deklarationen */
menItem *rootmenu;

/* modul-lokale Funktions-Deklarationen */
menItem *menCreateRootMenu(char *menutitle);
menItem *menAddSubmenu(char *submenutitle,int id,menItem *menu);
void    menAddItem(char *itemtitle,int id,menItem *menu,menType type);
void    menFreeMenu(menItem *item);
