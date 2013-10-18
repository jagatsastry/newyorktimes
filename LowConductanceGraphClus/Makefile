CXX=g++
CXXFLAGS=-Wall -pedantic-errors -g
SOURCES=boostGraph.cpp approximatePageRank.cpp storeloadMethods.cpp
OBJECTS=$(SOURCES:.cpp=.o)


BIN = run

all: $(BIN)

run: $(OBJECTS) main.cpp 
	$(CXX) $(CXXFLAGS) $(OBJECTS) main.cpp -o $@ 

matlabMethods.o: matlabMethods.cpp 
	$(CXX) $(CXXFLAGS) $(INCL) -c matlabMethods.cpp
	
ourMethodsMatrix.o: ourMethodsMatrix.cpp 
	$(CXX) $(CXXFLAGS) $(INCL) -c ourMethodsMatrix.cpp
	
boostGraph.o: boostGraph.cpp 
	$(CXX) $(CXXFLAGS) $(INCL) -c boostGraph.cpp
	
clean:
	rm -f $(OBJECTS) $(BIN)