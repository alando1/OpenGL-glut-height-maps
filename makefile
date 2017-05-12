COMPILEFLAGS = -std=c++11 -I"./include" -O3 -Wall
COMPILECMD = g++ -c $(COMPILEFLAGS)
LINKFLAGS =   
LINKCMD = g++ $(LINKFLAGS) $(OBJS)  $(LIBRARIES) -o $(TARGET)
LIBRARIES = -lGLU -lGL -lSOIL -lglut
#LIBRARIES = -lSDL2 -lSDL2main -lSDL2_mixer -lfreeglut -lSOIL -lopengl32 -lglu32 
OBJS = Terrain.o SuppliedGlutFuncs.o DrawFunc.o 256main.o 
TARGET = run

main:   $(OBJS)
		$(LINKCMD)

clean:
	rm *.o

Terrain.o: Terrain.h Terrain.cpp
	$(COMPILECMD) Terrain.cpp

SuppliedGlutFuncs.o: SuppliedGlutFuncs.h SuppliedGlutFuncs.cpp
	$(COMPILECMD) SuppliedGlutFuncs.cpp

DrawFunc.o: DrawFunc.h DrawFunc.cpp
	$(COMPILECMD) DrawFunc.cpp

256main.o: 256main.cpp
	$(COMPILECMD) 256main.cpp