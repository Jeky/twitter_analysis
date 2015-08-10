cmake_minimum_required(VERSION 2.6)
project(ml)
 
set(PROJECT_INCLUDE_DIR 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/cereal/include
)
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
 
set(ML_SRCS 
    ${PROJECT_SOURCE_DIR}/ml.cc
)
 
include_directories("${PROJECT_BINARY_DIR}")
include_directories(../cereal/include)
include_directories(../utils/include)
include_directories("${PROJECT_INCLUDE_DIR}")
 
add_library(${PROJECT_NAME} STATIC ${ML_SRCS})