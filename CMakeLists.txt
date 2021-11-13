cmake_minimum_required(VERSION 3.20)
project(CQF)

set(CMAKE_CXX_STANDARD 14)

add_executable(CQF main.cpp CqiMatrix.cpp CqiMatrix.h Core.h ConnectionMatrix.cpp ConnectionMatrix.h CompQuality.cpp CompQuality.h Matrix.cpp Matrix.h)
