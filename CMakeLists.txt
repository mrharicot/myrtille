cmake_minimum_required(VERSION 2.8.12)
project(rt)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp -pthread")
set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG}     -Wall -g3 -O0 -fopenmp -pthread")
set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE}   -Wall -O2 -DNDEBUG -fopenmp -pthread")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -Wall -std=c++11 -g3 -O0 -fopenmp -pthread")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -std=c++11 -O2 -DNDEBUG -fopenmp -pthread")

SET(RT_SRCS
  renderer.cpp
  sampler.cpp
  material.cpp
  bvh.cpp
  geometry.cpp
  time_tools.cpp
  string_tools.cpp
  math_tools.cpp
  mesh.cpp
  camera.cpp
  file_tools.cpp
)

add_library(rt_libs STATIC
  ${RT_SRCS}
)
link_libraries(rt_libs)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

add_executable(rt main.cpp)
