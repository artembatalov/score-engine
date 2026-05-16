cmake_minimum_required(VERSION 3.24)

project(
    score-engine
    VERSION 1.0
    DESCRIPTION "score-engine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(lib)
add_subdirectory(bin)
