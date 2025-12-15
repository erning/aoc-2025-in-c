# Advent of Code 2025 - C23

Solutions for [Advent of Code 2025](https://adventofcode.com/2025/) implemented in modern C (C23 standard).

## Requirements

- GCC 13+ (with C23/C2x support)
- GNU Make

## Build & Run

```bash
# Build
make

# Run all days
make run

# Run with example inputs
make example

# Run specific days
./build/aoc2025 1 5 10

# Clean build
make clean
```

## Project Structure

```
c23/
├── Makefile
├── README.md
├── inputs/
│   ├── 01-example.txt
│   ├── 01-input.txt
│   └── ...
└── src/
    ├── common.h      # Common utilities and types
    ├── common.c      # File I/O, string helpers
    ├── days.h        # Day solver declarations
    ├── main.c        # Entry point
    ├── day01.c       # Day 1 solution
    └── ...
```

## Puzzles Overview

| Day | Title | Algorithm |
|-----|-------|-----------|
| 1 | Secret Entrance | Circular dial, modular arithmetic |
| 2 | Gift Shop | Number pattern generation (doubled, repeated) |
| 3 | Lobby | Greedy digit selection |
| 4 | Printing Department | Grid neighbor counting, cascade removal |
| 5 | Cafeteria | Range merging, interval queries |
| 6 | Trash Compactor | Column-wise text parsing |
| 7 | Laboratories | Beam simulation, timeline counting |
| 8 | Playground | Union-Find, minimum spanning tree |
| 9 | Movie Theater | Point-in-polygon, rectangle fitting |
| 10 | Factory | Gaussian elimination (GF(2) and integer) |
| 11 | Reactor | Graph path counting, memoized DFS |
| 12 | Christmas Tree Farm | Polyomino fitting, backtracking |

## Results

| Day | Part 1 | Part 2 |
|-----|--------|--------|
| 1 | 1123 | 6695 |
| 2 | 19386344315 | 34421651192 |
| 3 | 16927 | 167384358365132 |
| 4 | 1527 | 8690 |
| 5 | 558 | 344813017450467 |
| 6 | 5977759036837 | 9630000828442 |
| 7 | 1516 | 1393669447690 |
| 8 | 62186 | 8420405530 |
| 9 | 4729332959 | 1474477524 |
| 10 | 524 | 21696 |
| 11 | 607 | 506264456238938 |
| 12 | 599 | 0 |

## License

MIT
