add_library(score_engine_lib
    parser.cpp
    composition.cpp
    effects.cpp
    instruments.cpp
    renderer.cpp
    event.cpp
    pattern.cpp
    itmo_loops.cpp
)

target_include_directories(score_engine_lib
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)