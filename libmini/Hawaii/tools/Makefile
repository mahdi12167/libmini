SHELL	= sh

PRGS	= raw2pnm dem2pgm pnminfo

SRCS	= $(PRGS:=.cpp)
OBJS	= $(PRGS:=.o)

RM	= rm -rf

.SUFFIXES: .cpp

.cpp.o:
	$(COMPILER) $(OPTS) -I../../mini -c $*.cpp
	$(COMPILER) $(OPTS) -o $* $*.o -L../../mini -lMini $(LINK) -lGL -lGLU -lm
	@-strip $*

trap:

all:	$(OBJS)
	@-(cd SDTS; make prg)

clean:
	@-$(RM) $(OBJS) ii_files
	@-(cd SDTS; make clean)

tidy:	clean
	@-$(RM) $(PRGS)
	@-(cd SDTS; make tidy)
