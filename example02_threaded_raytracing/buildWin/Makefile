CXX=x86_64-w64-mingw32-g++
CXXFLAGS=-O3 -DNDEBUG

all: Example02.exe

Example02.exe: ../src/main.o ../src/rayTracer.o ../src/argParse.o
	$(CXX) -o Example02.exe -lpthread $^

.PHONY:

clean:
	rm -f Example02.exe
	rm -f ../src/*.o
