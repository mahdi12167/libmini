ALG	= 4
MAP	= Yukon

CDBGS	= #-DVERBOSE
CDEFS	= -DIMPACT -DLOWFOG
CFLAGS	= $(CDEFS) $(CDBGS) -Wall -Wno-parentheses -O3

MAIN	= CAR
MODS	= XWin OGL menu jpeg driver landscape data

SRCS	= $(MAIN).c $(MODS:=.c)
OBJS	= $(MAIN).o $(MODS:=.o)
LIBS	= -lm -lX11 -lXm -lXt -lGL -lGLU -ljpeg

.c.o:
	cc $(CFLAGS) -c $*.c

$(MAIN): $(OBJS)
	cc $(CFLAGS) -o $(MAIN) $(OBJS) $(LIBS)
	@strip $(MAIN)

depend:
	@makedepend $(CDEFS) $(SRCS)
	@rm -f Makefile.bak

clean:
	@rm -f $(MAIN) $(OBJS) core

pack:	clean
	@rm -f landscape.c map.h
	@zip -q9 -m $(MAP).zip $(MAP).*

unpack:
	@unzip $(MAP).zip >/dev/null
	@ln -s landscape$(ALG).c landscape.c
	@ln -s $(MAP).h map.h

# DO NOT DELETE THIS LINE -- make depend depends on it.
