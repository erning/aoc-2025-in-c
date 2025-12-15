// Day 4: Printing Department - Grid with '@' rolls
// A roll is accessible if it has fewer than 4 adjacent rolls (8 directions)

#include "common.h"

typedef struct {
    char** grid;
    int rows;
    int cols;
} Grid;

static Grid parse_input(const char* input) {
    int count;
    char** lines = split_lines(input, &count);

    Grid g;
    g.rows = count;
    g.cols = (count > 0) ? strlen(lines[0]) : 0;
    g.grid = lines;

    return g;
}

static void free_grid(Grid* g) {
    free_lines(g->grid, g->rows);
}

static int count_adjacent_rolls(Grid* g, int row, int col) {
    int count = 0;

    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;

            int nr = row + dr;
            int nc = col + dc;

            if (nr >= 0 && nr < g->rows && nc >= 0 && nc < g->cols) {
                if (g->grid[nr][nc] == '@') {
                    count++;
                }
            }
        }
    }

    return count;
}

static int64_t part_one(const char* input) {
    Grid g = parse_input(input);
    int accessible = 0;

    for (int row = 0; row < g.rows; row++) {
        for (int col = 0; col < g.cols; col++) {
            if (g.grid[row][col] == '@' && count_adjacent_rolls(&g, row, col) < 4) {
                accessible++;
            }
        }
    }

    free_grid(&g);
    return accessible;
}

static int64_t part_two(const char* input) {
    Grid g = parse_input(input);
    int total_removed = 0;

    // Dynamic array for positions to remove
    int capacity = 64;
    int* to_remove = malloc(capacity * 2 * sizeof(int));
    int remove_count;

    while (1) {
        remove_count = 0;

        for (int row = 0; row < g.rows; row++) {
            for (int col = 0; col < g.cols; col++) {
                if (g.grid[row][col] == '@' && count_adjacent_rolls(&g, row, col) < 4) {
                    if (remove_count * 2 >= capacity) {
                        capacity *= 2;
                        to_remove = realloc(to_remove, capacity * 2 * sizeof(int));
                    }
                    to_remove[remove_count * 2] = row;
                    to_remove[remove_count * 2 + 1] = col;
                    remove_count++;
                }
            }
        }

        if (remove_count == 0) break;

        for (int i = 0; i < remove_count; i++) {
            int row = to_remove[i * 2];
            int col = to_remove[i * 2 + 1];
            g.grid[row][col] = '.';
        }
        total_removed += remove_count;
    }

    free(to_remove);
    free_grid(&g);
    return total_removed;
}

DayResult day04(const char* input) {
    return (DayResult){
        .part1 = part_one(input),
        .part2 = part_two(input)
    };
}
