CXX = g++ -O2 -std=c++11

all: globalAssign localAssign

globalAssign : globalAssign.cpp
        $(CXX) -o globalAssign globalAssign.cpp

localAssign : localAssign.cpp
        $(CXX) -o localAssign localAssign.cpp

clean:
        rm -rf globalAssign localAssign