// Day 7: Laboratories - Beam simulation through manifold
// Beams start at 'S', move down, split at '^'

#include "common.h"

typedef struct {
    char** grid;
    int rows;
    int cols;
    int start_col;
} Grid;

static Grid parse_input(const char* input) {
    int count;
    char** lines = split_lines(input, &count);

    Grid g;
    g.grid = lines;
    g.rows = count;
    g.cols = (count > 0) ? strlen(lines[0]) : 0;

    // Find S position
    g.start_col = 0;
    for (int c = 0; c < g.cols; c++) {
        if (g.grid[0][c] == 'S') {
            g.start_col = c;
            break;
        }
    }

    return g;
}

// Simple hash set for (row, col) pairs
#define BEAM_HASH_SIZE 4096

typedef struct BeamNode {
    int row;
    int col;
    struct BeamNode* next;
} BeamNode;

typedef struct {
    BeamNode* buckets[BEAM_HASH_SIZE];
    int count;
} BeamSet;

static void beamset_init(BeamSet* set) {
    memset(set->buckets, 0, sizeof(set->buckets));
    set->count = 0;
}

static void beamset_free(BeamSet* set) {
    for (int i = 0; i < BEAM_HASH_SIZE; i++) {
        BeamNode* node = set->buckets[i];
        while (node) {
            BeamNode* next = node->next;
            free(node);
            node = next;
        }
    }
}

static unsigned int beam_hash(int row, int col) {
    return ((unsigned int)row * 31 + (unsigned int)col) % BEAM_HASH_SIZE;
}

static bool beamset_contains(BeamSet* set, int row, int col) {
    unsigned int idx = beam_hash(row, col);
    BeamNode* node = set->buckets[idx];
    while (node) {
        if (node->row == row && node->col == col) return true;
        node = node->next;
    }
    return false;
}

static void beamset_insert(BeamSet* set, int row, int col) {
    if (beamset_contains(set, row, col)) return;
    unsigned int idx = beam_hash(row, col);
    BeamNode* node = malloc(sizeof(BeamNode));
    node->row = row;
    node->col = col;
    node->next = set->buckets[idx];
    set->buckets[idx] = node;
    set->count++;
}

// Get all beams as array
static void beamset_to_array(BeamSet* set, int** rows, int** cols, int* count) {
    *rows = malloc(set->count * sizeof(int));
    *cols = malloc(set->count * sizeof(int));
    *count = 0;

    for (int i = 0; i < BEAM_HASH_SIZE; i++) {
        BeamNode* node = set->buckets[i];
        while (node) {
            (*rows)[*count] = node->row;
            (*cols)[*count] = node->col;
            (*count)++;
            node = node->next;
        }
    }
}

static int64_t part_one(const char* input) {
    Grid g = parse_input(input);

    BeamSet beams;
    beamset_init(&beams);
    beamset_insert(&beams, 0, g.start_col);

    int split_count = 0;

    while (beams.count > 0) {
        int* rows;
        int* cols;
        int count;
        beamset_to_array(&beams, &rows, &cols, &count);

        BeamSet new_beams;
        beamset_init(&new_beams);

        for (int i = 0; i < count; i++) {
            int row = rows[i];
            int col = cols[i];
            int next_row = row + 1;

            if (next_row >= g.rows) continue;

            char cell = g.grid[next_row][col];
            if (cell == '^') {
                split_count++;
                if (col > 0) beamset_insert(&new_beams, next_row, col - 1);
                if (col + 1 < g.cols) beamset_insert(&new_beams, next_row, col + 1);
            } else {
                beamset_insert(&new_beams, next_row, col);
            }
        }

        free(rows);
        free(cols);
        beamset_free(&beams);
        beams = new_beams;
    }

    beamset_free(&beams);
    free_lines(g.grid, g.rows);
    return split_count;
}

// For part 2, track timeline counts per position
#define TIMELINE_HASH_SIZE 4096

typedef struct TimelineNode {
    int row;
    int col;
    int64_t count;
    struct TimelineNode* next;
} TimelineNode;

typedef struct {
    TimelineNode* buckets[TIMELINE_HASH_SIZE];
} TimelineMap;

static void timelinemap_init(TimelineMap* map) {
    memset(map->buckets, 0, sizeof(map->buckets));
}

static void timelinemap_free(TimelineMap* map) {
    for (int i = 0; i < TIMELINE_HASH_SIZE; i++) {
        TimelineNode* node = map->buckets[i];
        while (node) {
            TimelineNode* next = node->next;
            free(node);
            node = next;
        }
    }
}

static void timelinemap_add(TimelineMap* map, int row, int col, int64_t count) {
    unsigned int idx = beam_hash(row, col);
    TimelineNode* node = map->buckets[idx];
    while (node) {
        if (node->row == row && node->col == col) {
            node->count += count;
            return;
        }
        node = node->next;
    }

    node = malloc(sizeof(TimelineNode));
    node->row = row;
    node->col = col;
    node->count = count;
    node->next = map->buckets[idx];
    map->buckets[idx] = node;
}

static bool timelinemap_empty(TimelineMap* map) {
    for (int i = 0; i < TIMELINE_HASH_SIZE; i++) {
        if (map->buckets[i]) return false;
    }
    return true;
}

static int64_t part_two(const char* input) {
    Grid g = parse_input(input);

    TimelineMap beams;
    timelinemap_init(&beams);
    timelinemap_add(&beams, 0, g.start_col, 1);

    int64_t total_timelines = 0;

    while (!timelinemap_empty(&beams)) {
        TimelineMap new_beams;
        timelinemap_init(&new_beams);

        for (int i = 0; i < TIMELINE_HASH_SIZE; i++) {
            TimelineNode* node = beams.buckets[i];
            while (node) {
                int row = node->row;
                int col = node->col;
                int64_t count = node->count;
                int next_row = row + 1;

                if (next_row >= g.rows) {
                    total_timelines += count;
                } else {
                    char cell = g.grid[next_row][col];
                    if (cell == '^') {
                        if (col > 0) timelinemap_add(&new_beams, next_row, col - 1, count);
                        if (col + 1 < g.cols) timelinemap_add(&new_beams, next_row, col + 1, count);
                    } else {
                        timelinemap_add(&new_beams, next_row, col, count);
                    }
                }

                node = node->next;
            }
        }

        timelinemap_free(&beams);
        beams = new_beams;
    }

    timelinemap_free(&beams);
    free_lines(g.grid, g.rows);
    return total_timelines;
}

DayResult day07(const char* input) {
    return (DayResult){
        .part1 = part_one(input),
        .part2 = part_two(input)
    };
}
