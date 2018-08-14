
GL_LIBS = `pkg-config --static --libs glfw3` -lGLEW -framework OpenGL
EXT = 
CPPFLAGS = `pkg-config --cflags glfw3`

CC = g++
EXE = GhostRider
OBJS = main.o racingTrack.o Shader.o Vehicle.o external_files/tiny_obj_loader.o Camera.o Lamp.o Barrel.o MainMenu.o gui.o Skybox.o Tetra.o

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(GL_LIBS)

main.o: main.cpp
	$(CC) $(CPPFLAGS) -c -g main.cpp

Shader.o: external_files/Shader.cpp external_files/Shader.hpp
	$(CC) $(CPPFLAGS) -c -g external_files/Shader.cpp

racingTrack.o: racingTrack.cpp racingTrack.h
	$(CC) $(CPPFLAGS}) -c -g racingTrack.cpp

Vehicle.o: Vehicle.cpp Vehicle.h
	$(CC) $(CPPFLAGS) -c -g Vehicle.cpp

Camera.o: Camera.cpp Camera.h
	$(CC) $(CPPFLAGS) -c -g Camera.cpp

Lamp.o: Lamp.cpp Lamp.h
	$(CC) $(CPPFLAGS) -c -g Lamp.cpp

Barrel.o: Barrel.cpp Barrel.h
	$(CC) $(CPPFLAGS) -c -g Barrel.cpp

MainMenu.o: MainMenu.cpp MainMenu.h
	$(CC) $(CPPFLAGS) -c -g MainMenu.cpp

gui.o: gui.cpp gui.h
	$(CC) $(CPPFLAGS) -c -g gui.cpp

Skybox.o: Skybox.cpp Skybox.h
	$(CC) $(CPPFLAGS) -c -g Skybox.cpp

Tetra.o: Tetra.cpp Tetra.h
	$(CC) $(CPPFLAGS) -c -g Tetra.cpp

clean:
	rm -f *.o $(EXE)$(EXT) 
	rm -f external_files/*.o