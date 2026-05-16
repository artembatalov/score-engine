set(CMAKE_CXX_STANDARD 20)

add_library(itmoloops_lib itmo_loops.cpp parser.cpp composition.cpp effects.cpp instruments.cpp renderer.cpp event.cpp pattern.cpp)

target_include_directories(itmoloops_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)