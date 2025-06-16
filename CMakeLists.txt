cmake_minimum_required(VERSION 3.29)
project(Eda_Proyecto)

set(CMAKE_CXX_STANDARD 20)

add_executable(Eda_Proyecto main.cpp
        Rtree/Mbb.h
        Rtree/Mbb.cpp
        Rtree/Entry.cpp
        Rtree/Entry.h
        Rtree/Node.cpp
        Rtree/Node.h
        Rtree/Rtree.cpp
        Rtree/Rtree.h)
