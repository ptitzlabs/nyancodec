IDIR=
CXX=g++
CXXFLAGS=--std=c++14 -Wall -O2

all: main.cpp
	$(CXX) main.cpp $(CXXFLAGS) -o main $(IDIR)
clean:
	rm -f main
