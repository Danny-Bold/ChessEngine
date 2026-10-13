cmake_minimum_required(VERSION 3.25)

project(ChessEngine
        VERSION 0.1.0
        LANGUAGES CXX)

add_library(chess_engine
        src/Board.cpp
        src/Bitboard.cpp
        src/AttackTables.cpp
        src/MoveGen.cpp
        include/chess/UCI.h
        src/UCI.cpp
        src/search.cpp
        include/chess/search.h
        src/Evaluation.cpp
        include/chess/Evaluation.h
        src/PieceSquareTables.cpp
        src/Zobrist.cpp
        include/chess/Zobrist.h
        include/chess/GoParameters.h
        src/perft.cpp
        include/chess/perft.h
)

target_include_directories(chess_engine
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(
        chess_engine
        PUBLIC
        cxx_std_20
)

set_target_properties(
        chess_engine PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

add_executable(
        ChessEngine
        src/main.cpp
)

target_link_libraries(
        ChessEngine
        PRIVATE chess_engine
)

if(MINGW)
    target_link_options(ChessEngine PRIVATE
            -static
            -static-libgcc
            -static-libstdc++
    )
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(chess_engine PRIVATE /W4 /permissive-)
    target_compile_options(ChessEngine PRIVATE /W4 /permissive-)
else()
    target_compile_options(chess_engine PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
    target_compile_options(ChessEngine PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()


# Testing block
include(CTest)

if(BUILD_TESTING)
    add_executable(chess_perft_tests
            tests/perft_tests.cpp
    )

    target_link_libraries(chess_perft_tests
            PRIVATE chess_engine
    )

    add_test(
            NAME chess_perft
            COMMAND chess_perft_tests
    )

    add_executable(chess_make_move_tests
            tests/make_move_tests.cpp
    )

    target_link_libraries(chess_make_move_tests
            PRIVATE chess_engine
    )

    add_test(
            NAME chess_make_move
            COMMAND chess_make_move_tests
    )
endif()