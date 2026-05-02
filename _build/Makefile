CC = g++
FLAGS = -L ./lib/ -lraylib -lgdi32 -lopengl32 -lwinmm -g

WEB_CC = em++
WEB_FLAGS = -O3 -L ./web_lib/ -lraylib -s USE_GLFW=3 -s ASYNCIFY -DPLATFORM_WEB -sLLD_REPORT_UNDEFINED

ALL_FLAGS = -I ./include/ -std=c++20

FILES = ../src/Simulator/Simulator.cpp
OBJECTS = Simulator.o

dev: $(OBJECTS)
	$(CC) ../src/main.cpp $(OBJECTS) -o test.exe $(ALL_FLAGS) $(FLAGS) 

web: clean
	$(WEB_CC) ../src/main.cpp $(FILES) -o index.html $(ALL_FLAGS) $(WEB_FLAGS)

clean:
	-rm *.o
	-rm *.exe
	-rm *.html
	-rm *.js
	-rm *.wasm

Simulator.o: ../src/Simulator/Simulator.hpp ../src/Simulator/Simulator.cpp
	$(CC) ../src/Simulator/Simulator.cpp -c $(FLAGS) $(ALL_FLAGS)