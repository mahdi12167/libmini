// (c) by Stefan Roettger

#include "minibase.h"

#include "minilod.h"

datacalc minilod::CALC;

// default constructor
minilod::minilod(float offsetlat,float offsetlon,
                 float scalex,float scaley,float scaleelev)
   {
   OFFSETLAT=offsetlat;
   OFFSETLON=offsetlon;

   SCALEX=scalex;
   SCALEY=scaley;
   SCALEELEV=scaleelev;

   BRICKS=NULL;
   VOLS=NULL;

   BNUM=BMAX=0;
   VNUM=VMAX=0;

   CONFIGURE_BRICKPASSES=1;
   CONFIGURE_BRICKOFFSET=0.0f;
   }

// destructor
minilod::~minilod()
   {
   unsigned int i;

   if (BRICKS!=NULL)
      {
      for (i=0; i<BNUM; i++)
         {
         free(BRICKS[i].bname);
         delete[] BRICKS[i].brick;
         }

      delete[] BRICKS;
      }

   if (VOLS!=NULL) delete[] VOLS;
   }

// add minibrick volume
void minilod::addbrick(char *brickname,
                       float brad,
                       unsigned int lods,
                       float stagger)
   {
   unsigned int i;

   minibrickdata *newbricks;

   float dist;

   if (brickname==NULL) brickname="Data.db";

   if (brad<0.0f || stagger<=1.0f) ERRORMSG();

   if (brad==0.0f) lods=1;

   if (BRICKS==NULL)
      {
      BMAX=1;
      BRICKS=new minibrickdata[BMAX];
      }
   else if (BNUM>=BMAX)
      {
      newbricks=new minibrickdata[2*BMAX];
      for (i=0; i<BNUM; i++) newbricks[i]=BRICKS[i];

      delete[] BRICKS;
      BRICKS=newbricks;
      BMAX*=2;
      }

   BRICKS[BNUM].bname=strdup(brickname);
   BRICKS[BNUM].brick=new minibrick[lods];

   BRICKS[BNUM].brad=brad;
   BRICKS[BNUM].lods=lods;
   BRICKS[BNUM].stagger=stagger;

   for (i=0; i<lods; i++)
      {
      // set brick pager
      BRICKS[BNUM].brick[i].setloader(DBavailable_callback,&BRICKS[BNUM],DBload_callback,
                                      OFFSETLAT,OFFSETLON,SCALEX,SCALEY,SCALEELEV);

      // set iso spectrum
      BRICKS[BNUM].brick[i].addiso(0.5f,1.0f,1.0f,1.0f,1.0f);

      // set render method
      BRICKS[BNUM].brick[i].configure_renderpasses(CONFIGURE_BRICKPASSES);

      // calculate staggered distance
      dist=0.999f*brad*fpow(stagger,i);

      // extract meshes
      BRICKS[BNUM].brick[i].setdistance(dist);
      BRICKS[BNUM].brick[i].extract(0.0f,0.0f,0.0f,-brad,dist,90.0f,1.0f);
      BRICKS[BNUM].brick[i].release();
      }

   BNUM++;
   }

// add volume at specific location
void minilod::addvolume(unsigned int index,
                        float midx,float midy,float basez,
                        float dx,float dy,float de,
                        float r,float g,float b,float a)
   {
   unsigned int i;

   miniloddata *newvols;

   if (index>=BNUM) ERRORMSG();

   if (VOLS==NULL)
      {
      VMAX=1;
      VOLS=new miniloddata[VMAX];
      }
   else if (VNUM>=VMAX)
      {
      newvols=new miniloddata[2*VMAX];
      for (i=0; i<VNUM; i++) newvols[i]=VOLS[i];

      delete[] VOLS;
      VOLS=newvols;
      VMAX*=2;
      }

   VOLS[VNUM].index=index;

   VOLS[VNUM].x=midx;
   VOLS[VNUM].y=midy;
   VOLS[VNUM].e=basez;

   VOLS[VNUM].dx=dx;
   VOLS[VNUM].dy=dy;
   VOLS[VNUM].de=de;

   VOLS[VNUM].r=r;
   VOLS[VNUM].g=g;
   VOLS[VNUM].b=b;
   VOLS[VNUM].a=a;

   VNUM++;
   }

// render the volumes
void minilod::render(float ex,float ey,float ez,
                     float farp,
                     float fovy,float aspect,
                     int phase)
   {
   int ph;

   unsigned int i,j;

   minibrickdata *brk;
   miniloddata *vol;

   minibrick *brick;

   float dist;
   int lod;

   // configure bricks
   for (i=0; i<BNUM; i++)
      for (j=0; j<BRICKS[i].lods; j++)
         BRICKS[i].brick[j].configure_stripeoffset(CONFIGURE_BRICKOFFSET);

   // render volumes
   for (ph=MINIBRICK_FIRST_RENDER_PHASE; ph<=MINIBRICK_LAST_RENDER_PHASE; ph++)
      if (phase==MINIBRICK_ONE_RENDER_PHASE || ph==phase)
         for (i=0; i<VNUM; i++)
            {
            // get actual volume
            vol=&VOLS[i];

            // get indexed brick
            brk=&BRICKS[vol->index];

            // calculate distance
            dist=fsqr((vol->x+OFFSETLON)*SCALEX-ex)+fsqr((vol->y+OFFSETLAT)*SCALEY-ez)+fsqr(vol->e*SCALEELEV-ey);
            dist/=fsqr(brk->brad);

            // calculate lod
            if (dist<=1.0f) lod=0;
            else lod=ftrc(flog(dist)/flog(brk->stagger)/2.0f+0.5f);
            if ((unsigned int)lod>=brk->lods) lod=(unsigned int)(brk->lods-1);

            // get actual brick lod
            brick=&brk->brick[lod];

            // set position
            brick->resetpos(vol->x,vol->y,vol->e,vol->dx,vol->dy,vol->de);

            // set color
            brick->addiso(0.5f,vol->r,vol->g,vol->b,vol->a);

            // render mesh
            brick->render(ex,ey,ez,0.0f,farp,fovy,aspect,0.0f,ph);
            }
   }

// minibrick callback for availability
int minilod::DBavailable_callback(int col,int row,int lod,void *data)
   {
   if (col!=0 || row!=0) ERRORMSG();
   return((unsigned int)lod<=((minibrickdata *)data)->lods);
   }

// minibrick callback for loading
void minilod::DBload_callback(int col,int row,int lod,databuf *volume,void *data)
   {
   int i;

   minibrickdata *brickdata=(minibrickdata *)data;

   unsigned int xs,ys,zs;

   if (col!=0 || row!=0) ERRORMSG();

   // register implicit calculator
   CALC.doregister();

   // load volume
   volume->loaddata(brickdata->bname);

   // deregister implicit calculator
   CALC.deregister();

   // calculate resampling size
   for (xs=3; xs<volume->xsize; xs=2*xs-1);
   for (ys=3; ys<volume->ysize; ys=2*ys-1);
   for (zs=3; zs<volume->zsize; zs=2*zs-1);

   // resample volume
   volume->resampledata(xs,ys,zs);

   // get new size
   xs=volume->xsize;
   ys=volume->ysize;
   zs=volume->zsize;

   // calculate size of LOD
   for (i=0; i<lod; i++)
      {
      xs=xs/2+1;
      ys=ys/2+1;
      zs=zs/2+1;
      }

   // resample to size of LOD
   volume->resampledata(xs,ys,zs);
   }

// configuring:

void minilod::configure_brickpasses(int brickpasses)
   {CONFIGURE_BRICKPASSES=brickpasses;}

void minilod::configure_brickoffset(float brickoffset)
   {CONFIGURE_BRICKOFFSET=brickoffset;}
