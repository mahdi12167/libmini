MINI	= ../mini
IRR	= ../../irrlicht-1.4.2

IRRINC	= $(IRR)/include
IRRLIB	= $(IRR)/lib/Linux

all:
	c++ -c faceofmars.cpp -I$(MINI)/.. -I$(IRRINC)
	c++ faceofmars.o -L$(MINI) -lMini -L$(IRRLIB) -lIrrlicht -lGL -lGLU -lm -o faceofmars

clean:
tidy:
	@-rm -f *~ *.o faceofmars

demo:   clean
	@-(cd ..; zip -qr9 FaceOfMars/FaceOfMars.zip FaceOfMars -x \*/CVS\*)
