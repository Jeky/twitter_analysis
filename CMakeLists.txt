cmake_minimum_required(VERSION 3.0)
project(twitter_analysis)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++11")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(SOURCE_FILES
# utils package
        include/utils/io.h include/utils/log.h include/utils.h include/utils/hash.h include/utils/string_view.h
        src/utils/io.cc src/utils/log.cc
# ml package
        include/ml/ml.h include/ml/classifier.h include/ml/porter2_stemmer.h include/ml/text.h include/ml/evaluator.h include/ml/features.h
        src/ml/ml.cc src/ml/classifier.cc src/ml/porter2_stemmer.cc src/ml/text.cc src/ml/evaluator.cc src/ml/features.cc
# analysis pacakge
        include/data.h
        src/data.cc)

include_directories("${PROJECT_INCLUDE_DIR}")
if (test)
    enable_testing()
    ##############
    # Unit Tests
    ##############
    add_executable(runUnitTest ${SOURCE_FILES} test/unittest.cc)

    # Standard linking to gtest stuff.
    target_link_libraries(runUnitTest ${CMAKE_SOURCE_DIR}/lib/libgtest.a ${CMAKE_SOURCE_DIR}/lib/libgtest_main.a)

    # This is so you can do 'make test' to see all your tests run, instead of
    # manually running the executable runUnitTests to see those specific tests.
    add_test(runUnitTest ${EXECUTABLE_OUTPUT_PATH}/runUnitTest)
else ()
    add_executable(analysis ${SOURCE_FILES} src/analysis.cc)
endif ()
