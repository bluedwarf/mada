all: test.exe

test.exe: main.cpp
	g++ -g -o test.exe main.cpp

clean:
	rm -f test.exe
