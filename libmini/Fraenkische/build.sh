#!/bin/tcsh -f

set INSTALL_INC=".."
set INSTALL_LIB="../mini"

set rule=$1
if ($rule == "") set rule="app"

if ($HOSTTYPE == "iris4d") make INSTALL_INC="$INSTALL_INC" INSTALL_LIB="$INSTALL_LIB" COMPILER="CC" OPTS="-O3 -mips3 -OPT:Olimit=0 -Wl,-woff84" LINK="-lglut -lX11 -lXm -lXt -lXmu" $rule
if ($HOSTTYPE == "i386") make INSTALL_INC="$INSTALL_INC" INSTALL_LIB="$INSTALL_LIB" COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
if ($HOSTTYPE == "i386-linux") make INSTALL_INC="$INSTALL_INC" INSTALL_LIB="$INSTALL_LIB" COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
if ($HOSTTYPE == "i386-cygwin") make INSTALL_INC="$INSTALL_INC" INSTALL_LIB="$INSTALL_LIB" COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
if ($HOSTTYPE == "i486") make INSTALL_INC="$INSTALL_INC" INSTALL_LIB="$INSTALL_LIB" COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
if ($HOSTTYPE == "i486-linux") make INSTALL_INC="$INSTALL_INC" INSTALL_LIB="$INSTALL_LIB" COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
if ($HOSTTYPE == "i586") make INSTALL_INC="$INSTALL_INC" INSTALL_LIB="$INSTALL_LIB" COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
if ($HOSTTYPE == "i586-linux") make INSTALL_INC="$INSTALL_INC" INSTALL_LIB="$INSTALL_LIB" COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
if ($HOSTTYPE == "i686") make INSTALL_INC="$INSTALL_INC" INSTALL_LIB="$INSTALL_LIB" COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
if ($HOSTTYPE == "i686-linux") make INSTALL_INC="$INSTALL_INC" INSTALL_LIB="$INSTALL_LIB" COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
if ($HOSTTYPE == "powerpc") make INSTALL_INC="$INSTALL_INC" INSTALL_LIB="$INSTALL_LIB" COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
if ($HOSTTYPE == "powermac") make INSTALL_INC="$INSTALL_INC" INSTALL_LIB="$INSTALL_LIB" COMPILER="c++" OPTS="-O3" LINK="-Wl,-w -L/System/Library/Frameworks/OpenGL.framework/Libraries -framework GLUT -lobjc" $rule
if ($HOSTTYPE == "intel-pc") make INSTALL_INC="$INSTALL_INC" INSTALL_LIB="$INSTALL_LIB" COMPILER="c++" OPTS="-O3" LINK="-Wl,-w -L/System/Library/Frameworks/OpenGL.framework/Libraries -framework GLUT -lobjc" $rule
