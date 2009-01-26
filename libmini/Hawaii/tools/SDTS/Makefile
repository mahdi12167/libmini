SHELL	= sh

CP	= cp -rfp
RM	= rm -rf

trap:

prg:	lib
	@-cc sdts2dem.c fips123.a -lm -o sdts2dem >/dev/null 2>/dev/null

lib:
	@-if [ ! -e fips123.a ]; then make all; fi

all:
	@-if [ ! -e f123 ]; then unzip -q f123.zip; fi
	@-(cd f123; $(CP) f123inc/stc123.h ..)
	@-(cd f123; make >/dev/null 2>/dev/null; $(CP) fips123.a ..)

clean:
	@-$(RM) sdts2dem fips123.a

tidy:	clean
	@-$(RM) f123 stc123.h
