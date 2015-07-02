uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq ($(uname_S), Darwin)
	CC = /usr/local/bin/g++-5
else
	CC = g++
endif

all:
		$(CC) -O3 -fopenmp -pthread -std=c++11 bvh.cpp geometry.cpp time_tools.cpp string_tools.cpp math_tools.cpp mesh.cpp camera.cpp main.cpp -o rt
