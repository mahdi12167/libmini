#!/bin/tcsh -f

# available rules:
# no rule  -> same as lib
# lib      -> build main library
# vlib     -> build viewer library
# stub     -> build without OpenGL
# nosquish -> build without squish
# usegreyc -> build with greyc
# wall     -> build with all warnings
# example  -> build example
# stubtest -> build stubtest
# viewer   -> build viewer
# deps     -> make main dependencies
# vdeps    -> make viewer dependencies
# install  -> make install
# clean    -> remove object files
# tidy     -> clean up all temporary files

# additional include directories
set INCLUDE="-I/usr/local/include -I../deps/freeglut -I../deps/squish -I../deps/greycstoration"

# additional link directories
set LINK="-L/usr/local/lib -L../deps/freeglut -L../deps/squish"

# default installation directory
set INSTALL="/usr/local"

set rule=$1
if ($rule == "") set rule="lib"

set incl=""
if ("$INCLUDE" != "") set incl=" $INCLUDE"

set link=""
if ("$LINK" != "") set link=" $LINK"

set stub=""
if ($rule == "stub") then
   set rule="lib"
   set stub=" -DNOOGL"
endif

set defs=""
if ($rule == "nosquish") then
   set rule="lib"
   set defs=" -DNOSQUISH"
endif
if ($rule == "usegreyc") then
   set rule="lib"
   set defs=" -DUSEGREYC"
endif

set wall=""
if ($rule == "wall") then
   set rule="lib"
   set wall=" -W -Wall"
endif

set option1=$2
if ($option1 == "nosquish") set defs=$defs" -DNOSQUISH"
if ($option1 == "usegreyc") set defs=$defs" -DUSEGREYC"
if ($option1 == "wall") set wall=" -W -Wall"

set option2=$3
if ($option2 == "nosquish") set defs=$defs" -DNOSQUISH"
if ($option2 == "usegreyc") set defs=$defs" -DUSEGREYC"
if ($option2 == "wall") set wall=" -W -Wall"

set depend="depend"
if ($rule == "vdeps") then
   set rule="deps"
   set depend="vdepend"
endif

if ($rule == "install") then
   if ($option1 != "") set INSTALL=$option1
   exec make INSTALL="$INSTALL" install
endif

if ($rule == "deps") then
   if ($HOSTTYPE == "iris4d") make MAKEDEPEND="CC -M$incl$stub$defs" TARGET=IRIX $depend
   if ($HOSTTYPE == "i386") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$incl$stub$defs" TARGET=LINUX $depend
   if ($HOSTTYPE == "i386-linux") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$incl$stub$defs" TARGET=LINUX $depend
   if ($HOSTTYPE == "i386-cygwin") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$incl$stub$defs" TARGET=LINUX $depend
   if ($HOSTTYPE == "i486") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$incl$stub$defs" TARGET=LINUX $depend
   if ($HOSTTYPE == "i486-linux") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$incl$stub$defs" TARGET=LINUX $depend
   if ($HOSTTYPE == "i586") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$incl$stub$defs" TARGET=LINUX $depend
   if ($HOSTTYPE == "i586-linux") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$incl$stub$defs" TARGET=LINUX $depend
   if ($HOSTTYPE == "i686") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$incl$stub$defs" TARGET=LINUX $depend
   if ($HOSTTYPE == "i686-linux") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$incl$stub$defs" TARGET=LINUX $depend
   if ($HOSTTYPE == "x86_64-linux") make MAKEDEPEND="c++ -M -m64 -I/usr/X11R6/include$incl$stub$defs" TARGET=LINUX $depend
   if ($HOSTTYPE == "powerpc") make MAKEDEPEND="c++ -M -I/usr/X11R6/include$incl$stub$defs" TARGET=LINUX $depend
   if ($HOSTTYPE == "powermac") make MAKEDEPEND="c++ -M$incl$stub$defs" TARGET=MACOSX $depend
   if ($HOSTTYPE == "intel-pc") make MAKEDEPEND="c++ -M$incl$stub$defs" TARGET=MACOSX $depend
else
   if ($HOSTTYPE == "iris4d") make COMPILER="CC" OPTS="-O3 -mips3 -OPT:Olimit=0 -Wl,-woff84$incl$stub$defs$wall" LINK="-lglut -lX11 -lXm -lXt -lXmu$link" $rule
   if ($HOSTTYPE == "i386") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$incl$stub$defs$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11$link" $rule
   if ($HOSTTYPE == "i386-linux") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$incl$stub$defs$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11$link" $rule
   if ($HOSTTYPE == "i386-cygwin") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$incl$stub$defs$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11$link" $rule
   if ($HOSTTYPE == "i486") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$incl$stub$defs$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11$link" $rule
   if ($HOSTTYPE == "i486-linux") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$incl$stub$defs$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11$link" $rule
   if ($HOSTTYPE == "i586") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$incl$stub$defs$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11$link" $rule
   if ($HOSTTYPE == "i586-linux") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$incl$stub$defs$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11$link" $rule
   if ($HOSTTYPE == "i686") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$incl$stub$defs$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11$link" $rule
   if ($HOSTTYPE == "i686-linux") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$incl$stub$defs$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11$link" $rule
   if ($HOSTTYPE == "x86_64-linux") make COMPILER="c++" OPTS="-m64 -O3 -I/usr/X11R6/include$incl$stub$defs$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib64 -lX11$link" $rule
   if ($HOSTTYPE == "powerpc") make COMPILER="c++" OPTS="-O3 -I/usr/X11R6/include$incl$stub$defs$wall" LINK="-lglut -lGLU -L/usr/X11R6/lib -lX11$link" $rule
   if ($HOSTTYPE == "powermac") make COMPILER="c++" OPTS="-O3$incl$stub$defs$wall" LINK="-Wl,-w -L/System/Library/Frameworks/OpenGL.framework/Libraries -framework GLUT -lobjc$link" $rule
   if ($HOSTTYPE == "intel-pc") make COMPILER="c++" OPTS="-O3$incl$stub$defs$wall" LINK="-Wl,-w -L/System/Library/Frameworks/OpenGL.framework/Libraries -framework GLUT -lobjc$link" $rule
endif
