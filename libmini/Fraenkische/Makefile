SHELL	= sh

MAIN	= Fraenkische

CP	= cp -rf
RM	= rm -rf

ZIP	= zip -qry9

trap:

app:
	$(COMPILER) $(OPTS) -o ${MAIN} -I$(INSTALL_INC) ${MAIN}.cpp -L$(INSTALL_LIB) -lMini $(LINK) -lGL -lGLU -lm # -lpthread -lcurl

tools:
	@-(cd tools; make all)

all:	app tools

clean:
	@-$(RM) ii_files Debug* Release* *.ncb *.opt *.plg ${MAIN} ${MAIN}*.zip
	@-(cd tools; make clean)

tidy:	clean
	@-$(RM) FraenkischeTileset/tileset.sav
	@-$(RM) FraenkischeTileset/startup.sav
	@-find . \( -name stub.\* -o -name tile.\*.p\?m\* \) -exec $(RM) {} \;
	@-find . \( -name \*~ -o -name core \) -exec $(RM) {} \;
	@-(cd tools; make tidy)

grid:	app
	@-$(RM) FraenkischeTileset/tileset.sav
	@-$(RM) FraenkischeTileset/startup.sav
	@-find . \( -name stub.\* -o -name tile.\*.p\?m\* \) -exec $(RM) {} \;
	@-${MAIN} -q

zip:	tidy
	@-(cd ..; $(ZIP) ${MAIN}/${MAIN}.zip ${MAIN} -x \*/CVS\*)

demo:	tidy grid
	@-make clean
	@-(cd ..; $(ZIP) ${MAIN}/${MAIN}.zip ${MAIN} -x \*/CVS\* -x \*/SRTM\* -x \*/GTOPO30\* -x \*/LandSat-\* -x \*/Vegetation\* -x \*/bin\* -x \*/Waypoints.txt -x \*/startup.sav)
