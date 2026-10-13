# Chess Engine project

## Project overview

This is a UCI-compliant chess engine build as a C++20 learning project.
It uses a hybrid bitboard-mailbox approach to track board state.
To find the best legal move from a position, a negamax algorithm with alpha-beta pruning is used. Through the UCI interface, iterative deepening modes with automatic time-budgeting can be selected.

## Features

- Hybrid board representation via bitboards and mailbox board to allow for both fast legal move generation and fast piece lookup.
- Make/undo move functionality that tracks board state including castling rights, en passant rights and repetition history.
- Negamax search algorithm with alpha-beta pruning respecting external termination commands, time budgets/deadlines and iterative deepening move reordering.
- Lightweight static evaluation function calculating per-piece material and positional contributions.
- Repetition detection via Zobrist hashing.
- Full UCI interface, enabling GUI use.

## Requirements and build instructions

- GCC (`g++`) via MinGW-w64.
- CMake 3.25 or later.

## Testing and validation

## Search and evaluation design

Searching through nodes uses a standard negamax algorithm with alpha-beta pruning.

## Performance and limitations

## Project structure

