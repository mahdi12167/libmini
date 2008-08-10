trap:

all:
	$(COMPILER) $(OPTS) -W -Wall -o Yukon -I$(INSTALL_INC) Yukon.cpp -L$(INSTALL_LIB) -lMini $(LINK) -lGL -lGLU -lm

clean:
	@-rm -f *~ ii_files Debug* Release* *.ncb *.opt *.plg Yukon Yukon.zip

demo:	clean
	@-(cd ..; zip -qr9 Yukon/Yukon.zip Yukon -x \*/CVS\*)
