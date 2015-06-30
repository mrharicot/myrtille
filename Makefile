all:
		g++ -g3 -std=c++11 bvh.cpp geometry.cpp time_tools.cpp string_tools.cpp math_tools.cpp mesh.cpp camera.cpp main.cpp -o rt
