CC = g++
FLAGS = -Wall -Wpedantic -I ./include/ -L ./lib/ -lraylib -lgdi32 -lopengl32 -lwinmm -g

WEB_CC = em++
WEB_FLAGS = -Wall -Wpedantic -O3 -I ./include/  -L ./web_lib/ -lraylib -s USE_GLFW=3 -s ASYNCIFY -DPLATFORM_WEB -sLLD_REPORT_UNDEFINED

FILES = ../src/Input/Input.cpp ../src/Simulator/Simulator.cpp
OBJECTS = Simulator.o

dev: $(OBJECTS)
	$(CC) ../src/main.cpp $(OBJECTS) -o test.exe $(FLAGS)

web: clean
	$(WEB_CC) ../src/main.cpp $(FILES) -o index.html $(WEB_FLAGS)

clean:
	-rm *.o
	-rm *.exe
	-rm *.html
	-rm *.js
	-rm *.wasm

Simulator.o: ../src/Simulator/Simulator.hpp ../src/Simulator/Simulator.cpp
	$(CC) ../src/Simulator/Simulator.cpp -c $(FLAGS)