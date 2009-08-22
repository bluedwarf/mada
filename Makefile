all: test.exe

test.exe: main.cpp DoubleArray.hpp MappedArray.hpp KeySet.hpp
#	g++ -pg -o test.exe main.cpp
	g++ -O3 -o test.exe main.cpp

clean:
	rm -f test.exe
