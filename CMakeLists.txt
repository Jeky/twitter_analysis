cmake_minimum_required (VERSION 2.6)
project (Twitter_Analysis)
add_definitions(-std=c++11)
aux_source_directory(src/. src_list)
include_directories("include")
set(CMAKE_BUILD_TYPE Release)

add_executable(analysis ${src_list}
    include/cereal/types/unordered_map.hpp
    include/cereal/types/memory.hpp
    include/cereal/types/string.hpp
    include/cereal/types/vector.hpp
    include/cereal/archives/binary.hpp
)
