.PHONY: clean

#FIXME change -g to -O2 for final tests
CXXFLAGS=$(CFLAGS) -std=c++11 -Wall -pedantic -g

all: main
	./main

main: mojagrubaryba.o grubaryba_example.o
	g++ $(CXXFLAGS) mojagrubaryba.o grubaryba_example.o -o main

mojagrubaryba.o: ../mojagrubaryba.cc ../mojagrubaryba.h
	g++ $(CXXFLAGS) -c ../mojagrubaryba.cc -o mojagrubaryba.o

grubaryba_example.o: grubaryba_example.cc grubaryba.h ../mojagrubaryba.h
	g++ $(CXXFLAGS) -c grubaryba_example.cc -o grubaryba_example.o
clean:
	rm -rf *.o
