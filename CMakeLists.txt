cmake_minimum_required(VERSION 3.3)
project(twitter_analysis)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(SOURCE_FILES
    # utils package
    include/utils/io.h include/utils/log.h include/utils.h
    src/utils/io.cc src/utils/log.cc
    # ml package
    include/ml/ml.h include/ml/classifier.h include/ml/porter2_stemmer.h include/ml/text.h
    src/ml/ml.cc src/ml/classifier.cc src/ml/porter2_stemmer.cc src/ml/text.cc
    # analysis pacakge
    include/data.h
    src/data.cc
    # executable file
    src/analysis.cc)

include_directories("${PROJECT_INCLUDE_DIR}")

add_executable(analysis ${SOURCE_FILES})