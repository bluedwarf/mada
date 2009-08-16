all: test.exe

test.exe: main.cpp DoubleArray.hpp MappedArray.hpp
	g++ -pg -o test.exe main.cpp

clean:
	rm -f test.exe
