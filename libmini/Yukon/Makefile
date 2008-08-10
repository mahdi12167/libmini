trap:

all:
	$(COMPILER) $(OPTS) -I.. -I$(INSTALL_INC) -W -Wall -o Yukon Yukon.cpp -L$(INSTALL_LIB) -lMini $(LINK) -lGL -lGLU -lm

clean:
	@-rm -f *~ ii_files Debug* Release* *.ncb *.opt *.plg Yukon Yukon.zip

demo:	clean
	@-(cd ..; zip -qr9 Yukon/Yukon.zip Yukon -x \*/CVS\*)
