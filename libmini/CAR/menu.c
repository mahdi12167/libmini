#include "menuP.h"

/* erzeuge das Wurzelmenue */
menItem *menCreateRootMenu(char *menutitle)
   {
   menItem *new;

   newmem();

   new->next=NULL;
   new->prev=NULL;
   new->items=NULL;
   new->upper=NULL;
   new->type=MEN_ROOT;
   new->id=MEN_NOID;
   strncpy(new->title,menutitle,MEN_MAXSTRINGLENGTH);

   return(new);
   }

/* erzeuge ein Untermenue */
menItem *menAddSubmenu(char *submenutitle,int id,menItem *menu)
   {
   menItem *actual,*new;

   checkmenu();
   newmem();
   lastitem();

   new->next=NULL;
   new->prev=actual;
   new->items=NULL;
   new->upper=menu;
   new->type=MEN_SUB;
   new->id=id;
   strncpy(new->title,submenutitle,MEN_MAXSTRINGLENGTH);

   return(new);
   }

/* erzeuge einen Menueeintrag */
void menAddItem(char *itemtitle,int id,menItem *menu,menType type)
   {
   menItem *actual,*new;

   checkmenu();
   newmem();
   lastitem();

   new->next=NULL;
   new->prev=actual;
   new->items=NULL;
   new->upper=menu;
   if (type==MEN_LEAF || type==MEN_RETURN) new->type=type;
   else ERROR();
   new->id=id;
   strncpy(new->title,itemtitle,MEN_MAXSTRINGLENGTH);
   }

/* loesche ein Menue samt allen Untermenues */
void menFreeMenu(menItem *item)
   {
   switch(item->type)
      {
      case MEN_ROOT   :
      case MEN_SUB    : if (item->items!=NULL) menFreeMenu(item->items);
                        if (item->next!=NULL) menFreeMenu(item->next);
                        free(item);
                        break;
      case MEN_LEAF   :
      case MEN_RETURN : if (item->next!=NULL) menFreeMenu(item->next);
                        free(item);
                        break;
      default         : ERROR();
      }
   }

/* erzeuge das Wurzelmenue [mit allen Untermenues] */
menItem *menInit()
   {
   rootmenu=menCreateRootMenu("CAR");
   menAddItem("Flip",Eflip,rootmenu,MEN_LEAF);
   menAddItem("Warp",Ewarp,rootmenu,MEN_LEAF);
   menAddItem("Quit",Equit,rootmenu,MEN_RETURN);

   return(rootmenu);
   }

/* loesche das Wurzelmenue samt allen Untermenues */
void menDestroy()
   {menFreeMenu(rootmenu);}
