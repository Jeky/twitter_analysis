cmake_minimum_required(VERSION 2.6)
project(twitter)
 
set(PROJECT_INCLUDE_DIR 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/cereal/include
)
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
 
set(TWITTER_SRCS
    ${PROJECT_SOURCE_DIR}/analysis.cc 
    ${PROJECT_SOURCE_DIR}/data.cc 
)
 
include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_INCLUDE_DIR}")
include_directories(../utils/include)
include_directories(../cereal/include)

add_executable(${PROJECT_NAME} ${TWITTER_SRCS})
target_link_libraries(${PROJECT_NAME} utils)