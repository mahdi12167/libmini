trap:

all:
	$(COMPILER) $(OPTS) -I.. -W -Wall -o Stuttgart -I$(INSTALL_INC) Stuttgart.cpp -L$(INSTALL_LIB) -lMini $(LINK) -lGL -lGLU -lm

clean:
	@-rm -f *~ ii_files Debug* Release* *.ncb *.opt *.plg Stuttgart Stuttgart.zip

demo:	clean
	@-(cd ..; zip -qr9 Stuttgart/Stuttgart.zip Stuttgart -x \*/CVS\*)
