# libraries must be in parent directory
# hint: compile libmini with "build.sh stub"

demo:
	gcc -c faceofmars.cpp -I../mini -I../irrlicht-0.7/include
	gcc faceofmars.o -L. -lIrrlicht -lMini -lGL -lGLU -L/usr/X11R6/lib -lX11 -lXm -lXt -lXmu -lXxf86vm -ljpeg -lz -lm -o faceofmars

clean:
	@-rm -f *~ faceofmars.o faceofmars
