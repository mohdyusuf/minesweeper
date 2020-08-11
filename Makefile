LIBRARIES = -framework OpenGL -framework GLUT -lpng

EXE = minesweeper

all:
	clang++ -arch x86_64 -std=c++17 -mmacosx-version-min=10.15 -Wall $(LIBRARIES) main.cpp -o $(EXE)
clean:
	-rm $(EXE)