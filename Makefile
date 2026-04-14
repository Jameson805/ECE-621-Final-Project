CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

TARGET = sim

SRCS = main.cpp simulate.cpp lattice.cpp noise.cpp syndrome.cpp decoder.cpp correction.cpp logical.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	