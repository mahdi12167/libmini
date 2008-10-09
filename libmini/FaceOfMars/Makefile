MINI	= ../mini
IRR	= ../irrlicht

demo:
	c++ -c faceofmars.cpp -I$(MINI)/.. -I$(IRR)/include
	c++ faceofmars.o -L$(MINI) -lMini -L$(IRR) -lIrrlicht -lGL -lGLU -L/usr/X11R6/lib -lX11 -lXm -lXt -lXmu -lXxf86vm -ljpeg -lz -lm -o faceofmars

clean:
	@-rm -f *~ faceofmars.o faceofmars
