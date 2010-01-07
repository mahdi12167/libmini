#include "dataP.h"

/* make all display lists */
void makelists()
   {
   int i;

   matrix M=M_1;

   heada1=0.0; heada2=0.0; heada3=0.0;
   leftarma1=0.0; leftarma2=-30.0; leftarma3=0.0;
   rightarma1=0.0; rightarma2=-30.0; rightarma3=0.0;
   leftlega1=-90.0; leftlega2=20.0;
   rightlega1=-90.0; rightlega2=20.0;
   steera=0.0; rpm=0.0;

   beginlist(origin,FALSE);
      setcolor(1.0,0.0,0.0);
      drawline(0.0,0.0,0.0,1.0,0.0,0.0);
      setcolor(0.0,1.0,0.0);
      drawline(0.0,0.0,0.0,0.0,1.0,0.0);
      setcolor(0.0,0.0,1.0);
      drawline(0.0,0.0,0.0,0.0,0.0,-1.0);
   endlist();

   beginlist(wireframe,FALSE);
      drawrectangle(1.0,1.0,1.0,-2.0,0.0,0.0,0.0,0.0,-2.0);
      drawrectangle(1.0,-1.0,1.0,-2.0,0.0,0.0,0.0,0.0,-2.0);
      drawline(1.0,1.0,1.0,1.0,-1.0,1.0);
      drawline(-1.0,1.0,1.0,-1.0,-1.0,1.0);
      drawline(-1.0,1.0,-1.0,-1.0,-1.0,-1.0);
      drawline(1.0,1.0,-1.0,1.0,-1.0,-1.0);
   endlist();

   beginlist(cube,FALSE);
      fillrectangle(1.0,1.0,1.0,0.0,-2.0,0.0,-2.0,0.0,0.0);
      fillrectangle(1.0,-1.0,-1.0,0.0,2.0,0.0,-2.0,0.0,0.0);
      fillrectangle(1.0,1.0,1.0,0.0,0.0,-2.0,0.0,-2.0,0.0);
      fillrectangle(-1.0,1.0,-1.0,0.0,0.0,2.0,0.0,-2.0,0.0);
      fillrectangle(1.0,1.0,-1.0,0.0,0.0,2.0,-2.0,0.0,0.0);
      fillrectangle(1.0,-1.0,1.0,0.0,0.0,-2.0,-2.0,0.0,0.0);
   endlist();

   beginlist(wheel,FALSE);
      push();
         for (i=0; i<8; i++)
            {
            setcolor(0.1,0.1,0.1);
            fillrectangle(1.0,1.0,FSQRT2-1.0,-2.0,0.0,0.0,0.0,0.0,2.0*(1.0-FSQRT2));
            filltriangle(0.7,0.0,0.0,1.0,0.0,0.0,0.3,0.3,0.3,
                         1.0,1.0,FSQRT2-1.0,1.0,0.0,0.0,0.1,0.1,0.1,
                         1.0,1.0,1.0-FSQRT2,1.0,0.0,0.0,0.1,0.1,0.1);
            filltriangle(-0.7,0.0,0.0,-1.0,0.0,0.0,0.3,0.3,0.3,
                         -1.0,1.0,1.0-FSQRT2,-1.0,0.0,0.0,0.1,0.1,0.1,
                         -1.0,1.0,FSQRT2-1.0,-1.0,0.0,0.0,0.1,0.1,0.1);
            setcolor(0.7,0.7,0.7);
            drawline(0.8,0.0,0.0,1.0,1.0/1.5,(FSQRT2-1.0)/1.5);
            drawline(-0.8,0.0,0.0,-1.0,1.0/1.5,(FSQRT2-1.0)/1.5);
            drawline(1.0,1.0/1.5,(FSQRT2-1.0)/1.5,1.0,1.0/1.5,(1.0-FSQRT2)/1.5);
            drawline(-1.0,1.0/1.5,(FSQRT2-1.0)/1.5,-1.0,1.0/1.5,(1.0-FSQRT2)/1.5);
            rotatex(45.0);
            }
      pop();
   endlist();

   beginlist(head1,FALSE);
      rotatex(heada1); /* [-15,20] */
   endlist();

   beginlist(head2,FALSE);
      rotatey(heada2); /* [-40,40] */
   endlist();

   beginlist(head3,FALSE);
      rotatez(heada3); /* [-30,30] */
   endlist();

   beginlist(headcolor,FALSE);
   endlist();

   beginlist(head,TRUE);
      push();
         push();
            translate(0.0,0.45,0.0);
            scale(0.5,1.0,0.6);
            setcolor(0.5,0.1,0.1);
            uselist(headcolor);
            uselist(cube); /* Hals */
         pop();
         translate(0.0,0.9,0.1);
         uselist(head2);
         uselist(head1);
         uselist(head3);
         push();
            translate(0.0,1.15,0.0);
            scale(1.2,1.35,1.2);
            setcolor(0.7,0.1,0.1);
            uselist(headcolor);
            uselist(cube); /* Schaedel */
         pop();
         push();
            translate(-0.5,1.55,1.0);
            push();
               rotatez(-1.0);
               scale(0.35,0.05,0.25);
               setcolor(0.05,0.05,0.0);
               uselist(cube); /* linke Augenbraue */
            pop();
            translate(0.0,-0.2,-0.03);
            scale(0.25,0.08,0.25);
            push();
               rotatez(45.0);
               setcolor(0.75,0.75,0.75);
               uselist(cube); /* linkes Auge */
            pop();
            scale(0.35,0.7,1.1);
            setcolor(0.0,0.0,0.2);
            uselist(cube); /* linke Pupille */
         pop();
         push();
            translate(0.5,1.55,1.0);
            push();
               rotatez(1.0);
               scale(0.35,0.05,0.25);
               setcolor(0.05,0.05,0.0);
               uselist(cube); /* rechte Augenbraue */
            pop();
            translate(0.0,-0.2,-0.03);
            scale(0.25,0.08,0.25);
            push();
               rotatez(45.0);
               setcolor(0.75,0.75,0.75);
               uselist(cube); /* rechtes Auge */
            pop();
            scale(0.35,0.7,1.1);
            setcolor(0.0,0.0,0.2);
            uselist(cube); /* rechte Pupille */
         pop();
         push();
            translate(0.0,0.9,1.07);
            rotatex(-25.0);
            scale(0.15,0.3,0.25);
            setcolor(0.3,0.0,0.0);
            uselist(headcolor);
            uselist(cube); /* Nase */
         pop();
         push();
            translate(-0.219,0.3,0.97);
            rotatez(-15.0);
            scale(0.25,0.08,0.25);
            setcolor(0.2,0.0,0.0);
            uselist(cube); /* linker Mundwinkel */
         pop();
         push();
            translate(0.219,0.3,0.97);
            rotatez(15.0);
            scale(0.25,0.08,0.25);
            setcolor(0.2,0.0,0.0);
            uselist(cube); /* rechter Mundwinkel */
         pop();
      pop();
   endlist();

   beginlist(leftarm1,FALSE);
      rotatez(leftarma1); /* [-3,100] */
   endlist();

   beginlist(leftarm2,FALSE);
      rotatex(leftarma2); /* [-200,60] */
   endlist();

   beginlist(leftarm3,FALSE);
      rotatex(leftarma3); /* [-160,0] */
   endlist();

   beginlist(leftarmtopcolor,FALSE);
   endlist();

   beginlist(leftarmbottomcolor,FALSE);
   endlist();

   beginlist(leftarmtop,TRUE);
      push();
         push();
            translate(-0.33,-1.75,0.0);
            scale(0.33,1.8,0.5);
            setcolor(0.5,0.5,0.0);
            uselist(leftarmtopcolor);
            uselist(cube); /* linker Oberarm */
         pop();
         translate(-0.33,-3.3,0.0);
         uselist(leftarmbottom);
      pop();
   endlist();

   beginlist(leftarmbottom,TRUE);
      push();
         uselist(leftarm3);
         push();
            translate(0.0,-1.75,0.0);
            scale(0.32,1.8,0.49);
            setcolor(0.5,0.5,0.0);
            uselist(leftarmbottomcolor);
            uselist(cube); /* linker Unterarm */
         pop();
         push();
            translate(0.0,-3.2,0.0);
            scale(0.4,0.45,0.6);
            setcolor(0.25,0.25,0.0);
            uselist(cube); /* linke Hand */
         pop();
      pop();
   endlist();

   beginlist(rightarm1,FALSE);
      rotatez(rightarma1); /* [-100,3] */
   endlist();

   beginlist(rightarm2,FALSE);
      rotatex(rightarma2); /* [-200,60] */
   endlist();

   beginlist(rightarm3,FALSE);
      rotatex(rightarma3); /* [-160,0] */
   endlist();

   beginlist(rightarmtopcolor,FALSE);
   endlist();

   beginlist(rightarmbottomcolor,FALSE);
   endlist();

   beginlist(rightarmtop,TRUE);
      push();
         push();
            translate(0.33,-1.75,0.0);
            scale(0.33,1.8,0.5);
            setcolor(0.5,0.5,0.0);
            uselist(rightarmtopcolor);
            uselist(cube); /* rechter Oberarm */
         pop();
         translate(0.33,-3.3,0.0);
         uselist(rightarmbottom);
      pop();
   endlist();

   beginlist(rightarmbottom,TRUE);
      push();
         uselist(rightarm3);
         push();
            translate(0.0,-1.75,0.0);
            scale(0.32,1.8,0.49);
            setcolor(0.5,0.5,0.0);
            uselist(rightarmbottomcolor);
            uselist(cube); /* rechter Unterarm */
         pop();
         push();
            translate(0.0,-3.2,0.0);
            scale(0.4,0.45,0.6);
            setcolor(0.25,0.25,0.0);
            uselist(cube); /* rechte Hand */
         pop();
      pop();
   endlist();

   beginlist(leftleg1,FALSE);
      rotatex(leftlega1); /* [-120,30] */
   endlist();

   beginlist(leftleg2,FALSE);
      rotatex(leftlega2); /* [0,120] */
   endlist();

   beginlist(leftlegtopcolor,FALSE);
   endlist();

   beginlist(leftlegbottomcolor,FALSE);
   endlist();

   beginlist(leftlegtop,TRUE);
      push();
         push();
            translate(0.0,-1.5,0.0);
            scale(0.7,2.2,0.9);
            setcolor(0.0,0.5,0.0);
            uselist(leftlegtopcolor);
            uselist(cube); /* linker Oberschenkel */
         pop();
         translate(0.0,-3.5,0.0);
         uselist(leftlegbottom);
      pop();
   endlist();

   beginlist(leftlegbottom,TRUE);
      push();
         uselist(leftleg2);
         push();
            translate(0.0,-2.0,0.0);
            scale(0.69,2.2,0.89);
            setcolor(0.0,0.5,0.0);
            uselist(leftlegbottomcolor);
            uselist(cube); /* linker Unterschenkel */
         pop();
         push();
            translate(0.0,-4.15,0.6);
            scale(0.75,0.23,1.7);
            setcolor(0.05,0.05,0.0);
            uselist(cube); /* linker Fuss */
         pop();
      pop();
   endlist();

   beginlist(rightleg1,FALSE);
      rotatex(rightlega1); /* [-120,30] */
   endlist();

   beginlist(rightleg2,FALSE);
      rotatex(rightlega2); /* [0,120] */
   endlist();

   beginlist(rightlegtopcolor,FALSE);
   endlist();

   beginlist(rightlegbottomcolor,FALSE);
   endlist();

   beginlist(rightlegtop,TRUE);
      push();
         push();
            translate(0.0,-1.5,0.0);
            scale(0.7,2.2,0.9);
            setcolor(0.0,0.5,0.0);
            uselist(rightlegtopcolor);
            uselist(cube); /* rechter Oberschenkel */
         pop();
         translate(0.0,-3.5,0.0);
         uselist(rightlegbottom);
      pop();
   endlist();

   beginlist(rightlegbottom,TRUE);
      push();
         uselist(rightleg2);
         push();
            translate(0.0,-2.0,0.0);
            scale(0.69,2.2,0.89);
            setcolor(0.0,0.5,0.0);
            uselist(rightlegbottomcolor);
            uselist(cube); /* rechter Unterschenkel */
         pop();
         push();
            translate(0.0,-4.15,0.6);
            scale(0.75,0.23,1.7);
            setcolor(0.05,0.05,0.0);
            uselist(cube); /* rechter Fuss */
         pop();
      pop();
   endlist();

   beginlist(robot,FALSE);
      push();
         scale(0.1,0.1,0.1);
         translate(0.0,11.11,0.0);
         push();
            translate(0.0,3.0,0.0);
            uselist(head2);
            uselist(head1);
            uselist(head3);
            uselist(head);
         pop();
         push();
            scale(2.0,3.0,1.4);
            setcolor(0.0,0.0,1.0);
            uselist(cube); /* Rumpf */
         pop();
         push();
            translate(-2.0,2.5,0.0);
            uselist(leftarm2);
            uselist(leftarm1);
            rotatez(-3.0);
            uselist(leftarmtop);
         pop();
         push();
            translate(2.0,2.5,0.0);
            uselist(rightarm2);
            uselist(rightarm1);
            rotatez(3.0);
            uselist(rightarmtop);
         pop();
         push();
            translate(0.0,-3.3,0.0);
            scale(0.3,0.8,0.3);
            setcolor(0.0,0.5,0.0);
            uselist(cube); /* Sack */
         pop();
         push();
            translate(-1.0,-3.3,0.0);
            uselist(leftleg1);
            uselist(leftlegtop);
         pop();
         push();
            translate(1.0,-3.3,0.0);
            uselist(rightleg1);
            uselist(rightlegtop);
         pop();
      pop();
   endlist();

   beginlist(steer,FALSE);
      rotatey(steera); /* [-30.0,30.0] */
   endlist();

   beginlist(drive,FALSE);
      rotatex(rpm); /* [0.0,360.0) */
   endlist();

   beginlist(red,FALSE);
      setcolor(1.0,0.0,0.0);
   endlist();

   beginlist(car,FALSE);
      push();
         translate(0.0,CAR_R,0.0);
         push();
            M.M[2][3]=0.1;
            multiply(&M);
            push();
               translate(0.0,CAR_H/2.0,0.0);
               scale(CAR_W/2.0,CAR_H/2.0,CAR_L/2.0);
               setcolor(1.0,1.0,0.0);
               /* Karosserie */
               fillrectangle(1.0,-1.0,1.0,0.0,0.0,-2.0,-2.0,0.0,0.0);
               fillrectangle(1.0,1.0,1.0,0.0,-2.0,0.0,-2.0,0.0,0.0);
               fillrectangle(1.0,-1.0,-1.0,0.0,2.0,0.0,-2.0,0.0,0.0);
               fillrectangle(1.0,1.0,1.0,0.0,0.0,-2.0,0.0,-2.0,0.0);
               fillrectangle(-1.0,1.0,-1.0,0.0,0.0,2.0,0.0,-2.0,0.0);
               fillrectangle(1.0,1.0,1.0,-2.0,0.0,0.0,0.0,0.0,-1.0);
               fillrectangle(1.0,1.0,-1.0,0.0,0.0,0.4,-2.0,0.0,0.0);
               scale(0.99,1.0,1.0);
               translate(0.0,0.01,0.0);
               fillrectangle(1.0,-1.0,0.0,-2.0,0.0,0.0,0.0,0.0,-0.6);
               fillrectangle(-1.0,1.0,0.0,0.0,0.0,-0.6,0.0,-2.0,0.0);
               fillrectangle(1.0,1.0,-0.6,0.0,0.0,0.6,0.0,-2.0,0.0);
               fillrectangle(1.0,1.0,0.0,-2.0,0.0,0.0,0.0,-2.0,0.0);
               fillrectangle(1.0,-1.0,-0.6,-2.0,0.0,0.0,0.0,2.0,0.0);
               /* Sitzbank */
               fillrectangle(1.0,-0.5,-0.4,-2.0,0.0,0.0,0.0,0.0,-0.2);
               fillrectangle(1.0,-1.0,-0.4,-2.0,0.0,0.0,0.0,0.5,0.0);
               /* Radkaesten */
               fillrectangle(-1.0,0.1,-0.4,0.0,0.0,-0.2,0.2,0.0,0.0);
               fillrectangle(-1.0,-1.0,-0.4,0.0,1.1,0.0,0.2,0.0,0.0);
               fillrectangle(-0.8,0.1,-0.4,0.0,0.0,-0.2,0.0,-1.1,0.0);
               fillrectangle(1.0,0.1,-0.4,-0.2,0.0,0.0,0.0,0.0,-0.2);
               fillrectangle(1.0,-1.0,-0.4,-0.2,0.0,0.0,0.0,1.1,0.0);
               fillrectangle(0.8,0.1,-0.4,0.0,-1.1,0.0,0.0,0.0,-0.2);
               setcolor(BLACK);
               /* Windschutzscheibe */
               drawline(-1.0,1.0,0.0,-1.0,2.0,-CAR_H/CAR_L);
               drawline(1.0,1.0,0.0,1.0,2.0,-CAR_H/CAR_L);
               drawline(-1.0,2.0,-CAR_H/CAR_L,1.0,2.0,-CAR_H/CAR_L);
            pop();
            push();
               translate(-CAR_W/3.0,CAR_H/1.5,CAR_L/2.0);
               scale(0.15,0.1,0.01);
               setcolor(WHITE);
               uselist(cube); /* linker Scheinwerfer */
            pop();
            push();
               translate(CAR_W/3.0,CAR_H/1.5,CAR_L/2.0);
               scale(0.15,0.1,0.01);
               setcolor(WHITE);
               uselist(cube); /* rechter Scheinwerfer */
            pop();
            push();
               translate(-CAR_W/3.0,CAR_H/1.5,-CAR_L/2.0);
               scale(0.07,0.05,0.01);
               uselist(red);
               uselist(cube); /* linkes Bremslicht */
            pop();
            push();
               translate(CAR_W/3.0,CAR_H/1.5,-CAR_L/2.0);
               scale(0.07,0.05,0.01);
               uselist(red);
               uselist(cube); /* rechtes Bremslicht */
            pop();
            push();
               translate(-CAR_W/2.0-CAR_R/3.0,0.01,CAR_L/2.0-2.0*CAR_R);
               uselist(steer);
               uselist(drive);
               scale(CAR_R/3.0,1.2*CAR_R,1.2*CAR_R);
               uselist(wheel); /* linkes Vorderrad */
            pop();
            push();
               translate(CAR_W/2.0+CAR_R/3.0,0.01,CAR_L/2.0-2.0*CAR_R);
               uselist(steer);
               uselist(drive);
               scale(CAR_R/3.0,1.2*CAR_R,1.2*CAR_R);
               uselist(wheel); /* rechtes Vorderrad */
            pop();
            push();
               translate(-CAR_W/2.0,0.03,-CAR_L/2.0+1.5*CAR_R);
               uselist(drive);
               scale(CAR_R/3.0,CAR_R,CAR_R);
               uselist(wheel); /* linkes Hinterrad */
            pop();
            push();
               translate(CAR_W/2.0,0.03,-CAR_L/2.0+1.5*CAR_R);
               uselist(drive);
               scale(CAR_R/3.0,CAR_R,CAR_R);
               uselist(wheel); /* rechtes Hinterrad */
            pop();
         pop();
         push();
            translate(-CAR_W/4.0,0.15,0.5);
            rotatex(30.0);
            setcolor(BLACK);
            /* Lenkrad */
            drawline(0.0,0.0,0.0,0.0,0.0,-1.0);
            translate(0.0,0.0,-1.0);
            rotatex(-90.0);
            uselist(steer);
            rotatex(90.0);
            scale(0.4,0.4,1.0);
            drawline(0.5,(1.0-FSQRT2)/2.0,0.0,0.5,(FSQRT2-1.0)/2.0,0.0);
            drawline(0.5,(FSQRT2-1.0)/2.0,0.0,(FSQRT2-1.0)/2.0,0.5,0.0);
            drawline((FSQRT2-1.0)/2.0,0.5,0.0,(1.0-FSQRT2)/2.0,0.5,0.0);
            drawline((1.0-FSQRT2)/2.0,0.5,0.0,-0.5,(FSQRT2-1.0)/2.0,0.0);
            drawline(-0.5,(FSQRT2-1.0)/2.0,0.0,-0.5,(1.0-FSQRT2)/2.0,0.0);
            drawline(-0.5,(1.0-FSQRT2)/2.0,0.0,(1.0-FSQRT2)/2.0,-0.5,0.0);
            drawline((1.0-FSQRT2)/2.0,-0.5,0.0,(FSQRT2-1.0)/2.0,-0.5,0.0);
            drawline((FSQRT2-1.0)/2.0,-0.5,0.0,0.5,(1.0-FSQRT2)/2.0,0.0);
            drawline(-0.2,0.0,0.0,0.2,0.0,0.0);
            drawline(-0.2,0.0,0.0,-0.5,(1.0-FSQRT2)/2.0,0.0);
            drawline(0.2,0.0,0.0,0.5,(1.0-FSQRT2)/2.0,0.0);
         pop();
         push();
            translate(-CAR_W/4.0,-0.5,-CAR_L/4.0);
            uselist(robot);
         pop();
      pop();
   endlist();
   }
