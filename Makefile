uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq ($(uname_S), Darwin)
	CC = /usr/local/bin/g++-5
else
	CC = g++
endif

CFLAGS = -c -Wall -pedantic -g3 -std=c++11 -fopenmp -pthread
LDFLAGS = -fopenmp

SOURCES = material.cpp bvh.cpp geometry.cpp time_tools.cpp string_tools.cpp math_tools.cpp mesh.cpp camera.cpp main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

EXECUTABLE = rt

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
 
.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
 
clean:
	rm $(OBJECTS) $(EXECUTABLE)
