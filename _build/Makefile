BASE_FLAGS = -Wall -Wpedantic -I ./include/ -lraylib

CC = g++
FLAGS = $(BASE_FLAGS) -L ./lib/ -lgdi32 -lopengl32 -lwinmm -g

WEB_CC = em++
WEB_FLAGS = $(BASE_FLAGS) -O3 -L ./web_lib/ -s USE_GLFW=3 -s ASYNCIFY -DPLATFORM_WEB

dev:
	$(CC) ../src/main.cpp -o test.exe $(FLAGS)

web: clean
	$(WEB_CC) ../src/main.cpp -o index.html $(WEB_FLAGS)

clean:
	-rm *.o
	-rm *.exe
	-rm *.html
	-rm *.js
	-rm *.wasm