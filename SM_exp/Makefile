CXX = g++ -std=c++11
EXEFLAGS = -O2
CXXFLAGS = -O2 -c
BOOSTFLAGS = -lboost_system -lboost_filesystem

SOURCES = $(shell find . | grep -e ".hpp")
OBJECTS = $(SOURCES:%.hpp=%.o)

getData: getData.cpp
	$(CXX) $(EXEFLAGS) -o getData getData.cpp $(BOOSTFLAGS)

testSM: $(OBJECTS) testSM.cpp
	$(CXX) $^ $(EXEFLAGS) -o $@ $(BOOSTFLAGS)

%.o: %.cpp %.hpp util.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf getData testSM $(OBJECTS)
