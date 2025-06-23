cmake_minimum_required(VERSION 3.29)
project(Eda_Proyecto)

set(CMAKE_CXX_STANDARD 20)

add_executable(Eda_Proyecto main.cpp
        RStarTree/Point.h
        RStarTree/Point.cpp
        RStarTree/Mbb.h
        RStarTree/Mbb.cpp
        RStarTree/Entry.cpp
        RStarTree/Entry.h
        RStarTree/Node.cpp
        RStarTree/Node.h
        RStarTree/Rtree.cpp
        RStarTree/Rtree.h
        Knn_Interface/knn_strategy.h
        Knn_Interface/knn_strategy.cpp
        SMKNN/Splitt.h
        SMKNN/Splitt.cpp
        SMKNN/Merge.cpp
        SMKNN/Merge.h
        Frontend/ExportVisualizationFiles.h
)
