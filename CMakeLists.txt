cmake_minimum_required(VERSION 3.0.0)
project(NewRay VERSION 0.1.0)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(SDL2 REQUIRED)
include_directories(SDL2Test ${SDL2_INCLUDE_DIRS})
find_package ( OpenMP )
if ( OpenMP_FOUND )
	message ( STATUS "OpenMP_VERSION = ${OpenMP_VERSION}" )
endif ()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(STB_INCLUDE_PATH /home/number/tools/stb)
include_directories(${STB_INCLUDE_PATH})

add_executable(NewRay main.cpp)
target_compile_options(NewRay PUBLIC -fopenmp)
target_link_libraries(NewRay -lpthread ${SDL2_LIBRARIES})

