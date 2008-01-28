SHELL	= sh

PRGS	= db2db

SRCS	= $(PRGS:=.cpp)
OBJS	= $(PRGS:=.o)

RM	= rm -rf

.SUFFIXES: .cpp

.cpp.o:
	$(COMPILER) $(OPTS) -c -I.. $*.cpp
	$(COMPILER) $(OPTS) -o $* $*.o -L.. -lMini -lMiniSFX $(LINK) -lGL -lGLU -lm

trap:

all:	$(OBJS)

clean:
	@-$(RM) $(OBJS) ii_files

tidy:	clean
	@-$(RM) *~
	@-$(RM) $(PRGS)
