CC = g++
CPPFLAGS = -g -std=c++11
LIBS = -lm

raytracer:  raytracer.o util.o light_source.o scene_object.o bmp_io.o obj_io.o
	$(CC) $(CFLAGS) -std=c++11 -o raytracer \
	raytracer.o util.o light_source.o scene_object.o bmp_io.o obj_io.o  $(LIBS)

clean:
	-rm -f core *.o
	-rm raytracer
