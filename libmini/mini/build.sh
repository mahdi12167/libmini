#!/bin/csh -f

# available rules:
# no rule  -> same as lib
# lib      -> build main library
# vlib     -> build viewer library
# stub     -> build without OpenGL refs
# wall     -> build with all warnings
# example  -> build example
# stubtest -> build stubtest
# viewer   -> build viewer
# deps     -> make main dependencies
# vdeps    -> make viewer dependencies
# install  -> make install
# clean    -> remove object files
# tidy     -> clean up all temporary files

# installation directory
set INSTALL="/usr/local"

set rule=$1
if ($rule == "") set rule="lib"

set stub=""
if ($rule == "stub") then
   set rule="lib"
   set stub=" -DNOOGL"
endif

set wall=""
if ($rule == "wall") then
   set rule="lib"
   set wall=" -Wall"
endif

set option=$2
if ($option == "wall") set wall=" -Wall"

set depend="depend"
if ($rule == "vdeps") then
   set rule="deps"
   set depend="vdepend"
endif

if ($rule == "install") then
   if ($option != "") set INSTALL=$option
   exec make INSTALL="$INSTALL" install
endif

if ($rule == "deps") then
   if ($HOSTTYPE == "iris4d") make MAKEDEPEND="CC -M$stub" TARGET=IRIX $depend
   if ($HOSTTYPE == "i386") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$stub" TARGET=LINUX $depend
   if ($HOSTTYPE == "i386-linux") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$stub" TARGET=LINUX $depend
   if ($HOSTTYPE == "i386-cygwin") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$stub" TARGET=LINUX $depend
   if ($HOSTTYPE == "i486") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$stub" TARGET=LINUX $depend
   if ($HOSTTYPE == "i486-linux") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$stub" TARGET=LINUX $depend
   if ($HOSTTYPE == "i586") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$stub" TARGET=LINUX $depend
   if ($HOSTTYPE == "i586-linux") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$stub" TARGET=LINUX $depend
   if ($HOSTTYPE == "i686") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$stub" TARGET=LINUX $depend
   if ($HOSTTYPE == "i686-linux") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$stub" TARGET=LINUX $depend
   if ($HOSTTYPE == "x86_64-linux") make MAKEDEPEND="c++ -M -m64 -I/usr/X11R6/include$stub" TARGET=LINUX $depend
   if ($HOSTTYPE == "powerpc") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$stub" TARGET=LINUX $depend
   if ($HOSTTYPE == "powermac") make MAKEDEPEND="c++ -M$stub" TARGET=MACOSX $depend
   if ($HOSTTYPE == "intel-pc") make MAKEDEPEND="c++ -M$stub" TARGET=MACOSX $depend
else
   if ($HOSTTYPE == "iris4d") make COMPILER="CC" OPTS="-O3 -mips3 -OPT:Olimit=0 -Wl,-woff84$stub$wall" LINK="-lglut -lX11 -lXm -lXt -lXmu" $rule
   if ($HOSTTYPE == "i386") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$stub$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
   if ($HOSTTYPE == "i386-linux") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$stub$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
   if ($HOSTTYPE == "i386-cygwin") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$stub$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
   if ($HOSTTYPE == "i486") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$stub$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
   if ($HOSTTYPE == "i486-linux") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$stub$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
   if ($HOSTTYPE == "i586") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$stub$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
   if ($HOSTTYPE == "i586-linux") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$stub$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
   if ($HOSTTYPE == "i686") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$stub$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
   if ($HOSTTYPE == "i686-linux") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$stub$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
   if ($HOSTTYPE == "x86_64-linux") make COMPILER="c++" OPTS="-m64 -O3 -I/usr/X11R6/include$stub$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib64 -lX11" $rule
   if ($HOSTTYPE == "powerpc") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$stub$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11" $rule
   if ($HOSTTYPE == "powermac") make COMPILER="c++" OPTS="-O3$stub$wall" LINK="-Wl,-w -L/System/Library/Frameworks/OpenGL.framework/Libraries -framework GLUT -lobjc" $rule
   if ($HOSTTYPE == "intel-pc") make COMPILER="c++" OPTS="-O3$stub$wall" LINK="-Wl,-w -L/System/Library/Frameworks/OpenGL.framework/Libraries -framework GLUT -lobjc" $rule
endif
