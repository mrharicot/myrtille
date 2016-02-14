cmake_minimum_required(VERSION 2.8.12)
project(myrtille)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_CXX_COMPILER "/usr/local/bin/g++-5")
    set(CMAKE_C_COMPILER   "/usr/local/bin/gcc-5")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")



set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS}         -std=c++11 -fopenmp -pthread")
set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG}     -Wall -g3 -O0 -fopenmp -pthread")
set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE}   -Wall -O3 -DNDEBUG -fopenmp -pthread")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -Wall -std=c++11 -g3 -O0 -fopenmp -pthread")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -std=c++11 -O3 -DNDEBUG -fopenmp -pthread")

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
  bsdf.cpp
  scene.cpp)

add_library(rt_libs STATIC
  ${RT_SRCS}
)
link_libraries(rt_libs)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

add_executable(myrtille main.cpp)
