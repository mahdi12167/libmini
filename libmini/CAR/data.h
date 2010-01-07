/* globale Makro-Definitionen */
#define CAR_L (3.7)
#define CAR_W (1.7)
#define CAR_H (0.8)
#define CAR_R (0.4)
#define CAR_S (0.1)

/* globale Typ-Deklarationen */
enum{origin=1,wireframe,cube,wheel,
     head,headcolor,head1,head2,head3,
     leftarmtop,leftarmtopcolor,leftarmbottom,leftarmbottomcolor,leftarm1,leftarm2,leftarm3,
     rightarmtop,rightarmtopcolor,rightarmbottom,rightarmbottomcolor,rightarm1,rightarm2,rightarm3,
     leftlegtop,leftlegtopcolor,leftlegbottom,leftlegbottomcolor,leftleg1,leftleg2,
     rightlegtop,rightlegtopcolor,rightlegbottom,rightlegbottomcolor,rightleg1,rightleg2,
     robot,steer,drive,red,car};

/* globale Variablen-Deklarationen */
extern float heada1,heada2,heada3,
             leftarma1,leftarma2,leftarma3,rightarma1,rightarma2,rightarma3,
             leftlega1,leftlega2,rightlega1,rightlega2,
             steera,rpm;

/* globale Funktions-Deklarationen */
void updateangle(float *angle,float spin,float min,float max);

void makelists(void);
