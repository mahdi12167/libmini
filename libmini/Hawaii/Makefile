SHELL	= sh

MAIN	= Hawaii

RM	= rm -rf
ZIP	= zip -qry9

trap:

app:
	$(COMPILER) $(OPTS) -I.. -I$(INSTALL_INC) -W -Wall -o ${MAIN} ${MAIN}.cpp -L$(INSTALL_LIB) -lMini $(LINK) -lGL -lGLU -lm # -lpthread

tools:
	@-(cd tools; make all)

all:	app tools

clean:
	@-$(RM) ii_files Debug* Release* *.ncb *.opt *.plg ${MAIN} ${MAIN}*.zip
	@-(cd tools; make clean)

tidy:	clean
	@-find . -name \*.tileset.sav -exec $(RM) {} \;
	@-find . \( -name startup.sav -o -name settings.sav \) -exec $(RM) {} \;
	@-find . \( -name stub.\* -o -name tile.\*.p\?m\* \) -exec $(RM) {} \;
	@-find . \( -name normalized.\* \) -exec $(RM) {} \;
	@-find . \( -name \*~ -o -name core \) -exec $(RM) {} \;
	@-(cd tools; make tidy)

gridlo:	app
	@-find . \( -name stub.\* -o -name tile.\*.p\?m\* \) -exec $(RM) {} \;
	@-${MAIN} -n -q

gridhi:	app
	@-find . \( -name stub.\* -o -name tile.\*.p\?m\* \) -exec $(RM) {} \;
	@-${MAIN} -n -h -q

zip:	tidy
	@-(cd ..; $(ZIP) ${MAIN}/${MAIN}-all.zip ${MAIN} -x \*/CVS\*)

demo:	tidy gridlo
	@-make clean
	@-(cd ..; $(ZIP) ${MAIN}/${MAIN}.zip ${MAIN} -x \*/CVS\* -x \*/USGS-DEM\* -x \*/LandSat-ETM\* -x \*/Bathymetry-DEM\* -x \*/normalized/normalized.\* -x \*/Waypoints\*.txt -x \*/startup.sav -x \*/settings.sav -x \*/Screens\*)
