cmake_minimum_required(VERSION 3.12)
project(MyProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(my_program
  src/main.cpp
  src/LR.h
)

add_executable(test1
  src/Test10.cpp
  src/LR.h
)

add_executable(test2
  src/Final_tests.cpp
  src/LR.h
)
