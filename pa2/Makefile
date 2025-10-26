CXX := g++
CXXFLAGS := -std=c++17 -O3 -Wall -Wextra -Wshadow -Wpedantic
LDFLAGS :=

SRC := main.cpp parse.cpp pack.cpp tree.cpp output.cpp
OBJ := $(SRC:.cpp=.o)

pa2: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp pa2.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

debug: CXXFLAGS := -std=c++17 -g -O0 -Wall -Wextra -Wshadow -Wpedantic
debug: clean pa2

clean:
	rm -f $(OBJ) pa2

.PHONY: clean debug
