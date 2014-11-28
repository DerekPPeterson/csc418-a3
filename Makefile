CC = g++
CPPFLAGS = -g -O2 -std=c++11
LIBS = -lm

raytracer:  raytracer.o util.o light_source.o scene_object.o bmp_io.o obj_io.o \
			scenes.o
	$(CC) $(CFLAGS) -std=c++11 -o raytracer \
	raytracer.o util.o light_source.o scene_object.o bmp_io.o obj_io.o scenes.o $(LIBS)

clean:
	-rm -f core *.o
	-rm raytracer
