/* globale Makro-Definitionen */
#define MEN_MAXSTRINGLENGTH (10)
#define MEN_NOID            (-1)

/* globale Typ-Deklarationen */
typedef enum{MEN_ROOT,MEN_SUB,MEN_LEAF,MEN_RETURN} menType;

typedef struct _menItem
   {
   struct _menItem *next;                      /* naechster Menueeintrag */
   struct _menItem *prev;                      /* vorheriger Menueeintrag */
   struct _menItem *items;                     /* Elemente dieses Menues */
   struct _menItem *upper;                     /* oberhalb liegendes Menue */
   menType         type;                       /* Typ des Menueknotens */
   char            title[MEN_MAXSTRINGLENGTH]; /* Titel des Menueknotens */
   int             id;                         /* id des Menueknotens */
   }
menItem;

/* globale Variablen-Deklarationen */

/* globale Funktions-Deklarationen */
menItem *menInit(void);
void    menDestroy(void);
