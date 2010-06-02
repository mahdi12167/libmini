// (c) by Stefan Roettger

#ifndef PLOT_H
#define PLOT_H

// open a window for 2D plotting
void plot_openwindow(int *argc,char *argv[], // main arguments
                     int width,int height, // window size
                     float r,float g,float b, // background color
                     void (*render)(double time), // render function
                     BOOLINT (*keypress)(unsigned char key,float x,float y), // keypress function
                     BOOLINT continuous, // render continuously
                     float fps=50.0f); // target frame rate

// get window dimensions
int get_winwidth();
int get_winheight();

// set plot line attributes such as color and line width
void plot_color(const float r,const float g,const float b);
void plot_linewidth(const int w);

// plot line from (x1,y1) to (x2,y2) in normalized (x,y) coordinates
// with the origin at the bottom left corner
void plot_line(const float x1,const float y1,
               const float x2,const float y2);

// plot line from actual position (x,y)
void plot_from(const float x,const float y);

// plot line from last position to (x,y)
void plot_to(const float x,const float y);

// plot line to last position plus delta
void plot_delta(const float dx,const float dy);

// plot point
void plot_point(const float x,const float y);

// plot circle
void plot_circle(float x,float y,float r);

#endif
