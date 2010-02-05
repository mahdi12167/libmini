// (c) by Stefan Roettger

#ifndef PLOT_H
#define PLOT_H

// open a window for 2D plotting
void plot_openwindow(int *argc,char *argv[], // main arguments
                     int width,int height, // window size
                     float r,float g,float b, // background color
                     void (*render)(double time)); // render function

// set plot line attributes such as color and line width
void plot_color(const float r,const float g,const float b);
void plot_linewidth(const int w);

// plot line from (x1,y1) to (x2,y2) in normalized (x,y) coordinates
// with the origin at the bottom left corner
void plot_line(const float x1,const float y1,
               const float x2,const float y2);

#endif
