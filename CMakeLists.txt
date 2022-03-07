cmake_minimum_required(VERSION 3.21)
project(foam27)

set(CMAKE_CXX_STANDARD 14)

add_executable(foam27 [Semi_Dynamic]TC_cell_small_angle_higherOrder_polyLine2.cpp)

add_executable(foam27_linear linear.cpp)

add_executable(foam27_denticle
        denticle.cpp
        TEdgeSpace.cpp
        TEdgeSpace.h)
