all:
		g++ -fopenmp -O3 -std=c++11 geometry.cpp time_tools.cpp string_tools.cpp math_tools.cpp mesh.cpp camera.cpp main.cpp -o rt
