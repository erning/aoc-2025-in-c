// Day 12: Christmas Tree Farm - Polyomino fitting with backtracking

#include "common.h"

typedef struct {
    int row, col;
} Cell;

typedef struct {
    Cell* cells;
    int num_cells;
} Shape;

typedef struct {
    Shape* orientations;
    int num_orientations;
} ShapeSet;

typedef struct {
    int width;
    int height;
    int* counts;
    int num_counts;
} Region;

// Normalize shape so minimum row and col are 0
static Shape normalize_shape(Shape* s) {
    Shape result;
    result.num_cells = s->num_cells;
    result.cells = malloc(s->num_cells * sizeof(Cell));

    int min_r = INT_MAX, min_c = INT_MAX;
    for (int i = 0; i < s->num_cells; i++) {
        if (s->cells[i].row < min_r) min_r = s->cells[i].row;
        if (s->cells[i].col < min_c) min_c = s->cells[i].col;
    }

    for (int i = 0; i < s->num_cells; i++) {
        result.cells[i].row = s->cells[i].row - min_r;
        result.cells[i].col = s->cells[i].col - min_c;
    }

    // Sort cells
    for (int i = 0; i < result.num_cells - 1; i++) {
        for (int j = i + 1; j < result.num_cells; j++) {
            if (result.cells[j].row < result.cells[i].row ||
                (result.cells[j].row == result.cells[i].row && result.cells[j].col < result.cells[i].col)) {
                Cell tmp = result.cells[i];
                result.cells[i] = result.cells[j];
                result.cells[j] = tmp;
            }
        }
    }

    return result;
}

// Check if two shapes are equal
static bool shapes_equal(Shape* a, Shape* b) {
    if (a->num_cells != b->num_cells) return false;
    for (int i = 0; i < a->num_cells; i++) {
        if (a->cells[i].row != b->cells[i].row || a->cells[i].col != b->cells[i].col) {
            return false;
        }
    }
    return true;
}

// Generate all orientations (rotations and reflections)
static ShapeSet get_orientations(Shape* shape) {
    ShapeSet set;
    set.orientations = malloc(8 * sizeof(Shape));
    set.num_orientations = 0;

    Shape current;
    current.num_cells = shape->num_cells;
    current.cells = malloc(shape->num_cells * sizeof(Cell));
    memcpy(current.cells, shape->cells, shape->num_cells * sizeof(Cell));

    for (int rot = 0; rot < 4; rot++) {
        // Original
        Shape norm1 = normalize_shape(&current);
        bool found1 = false;
        for (int i = 0; i < set.num_orientations; i++) {
            if (shapes_equal(&set.orientations[i], &norm1)) {
                found1 = true;
                break;
            }
        }
        if (!found1) {
            set.orientations[set.num_orientations++] = norm1;
        } else {
            free(norm1.cells);
        }

        // Flip horizontal
        Shape flipped;
        flipped.num_cells = current.num_cells;
        flipped.cells = malloc(current.num_cells * sizeof(Cell));
        for (int i = 0; i < current.num_cells; i++) {
            flipped.cells[i].row = current.cells[i].row;
            flipped.cells[i].col = -current.cells[i].col;
        }

        Shape norm2 = normalize_shape(&flipped);
        free(flipped.cells);

        bool found2 = false;
        for (int i = 0; i < set.num_orientations; i++) {
            if (shapes_equal(&set.orientations[i], &norm2)) {
                found2 = true;
                break;
            }
        }
        if (!found2) {
            set.orientations[set.num_orientations++] = norm2;
        } else {
            free(norm2.cells);
        }

        // Rotate 90 degrees: (r, c) -> (c, -r)
        for (int i = 0; i < current.num_cells; i++) {
            int r = current.cells[i].row;
            int c = current.cells[i].col;
            current.cells[i].row = c;
            current.cells[i].col = -r;
        }
    }

    free(current.cells);
    return set;
}

static bool can_place(bool** grid, Shape* shape, int row, int col, int height, int width) {
    for (int i = 0; i < shape->num_cells; i++) {
        int r = row + shape->cells[i].row;
        int c = col + shape->cells[i].col;
        if (r < 0 || c < 0 || r >= height || c >= width) return false;
        if (grid[r][c]) return false;
    }
    return true;
}

static void place_shape(bool** grid, Shape* shape, int row, int col) {
    for (int i = 0; i < shape->num_cells; i++) {
        grid[row + shape->cells[i].row][col + shape->cells[i].col] = true;
    }
}

static void unplace_shape(bool** grid, Shape* shape, int row, int col) {
    for (int i = 0; i < shape->num_cells; i++) {
        grid[row + shape->cells[i].row][col + shape->cells[i].col] = false;
    }
}

static bool solve(bool** grid, int width, int height, ShapeSet* pieces, int num_pieces,
                  int* remaining, int num_remaining, int empty_budget) {
    if (num_remaining == 0) return true;

    // Find first empty cell
    int start_row = -1, start_col = -1;
    for (int r = 0; r < height && start_row < 0; r++) {
        for (int c = 0; c < width; c++) {
            if (!grid[r][c]) {
                start_row = r;
                start_col = c;
                break;
            }
        }
    }

    if (start_row < 0) return num_remaining == 0;

    // Try each remaining piece to cover the first empty cell
    bool any_placed = false;
    for (int i = 0; i < num_remaining; i++) {
        int piece_idx = remaining[i];
        ShapeSet* orientations = &pieces[piece_idx];

        for (int o = 0; o < orientations->num_orientations; o++) {
            Shape* shape = &orientations->orientations[o];

            for (int c = 0; c < shape->num_cells; c++) {
                int row = start_row - shape->cells[c].row;
                int col = start_col - shape->cells[c].col;

                if (can_place(grid, shape, row, col, height, width)) {
                    any_placed = true;
                    place_shape(grid, shape, row, col);

                    // Remove piece from remaining
                    int* new_remaining = malloc((num_remaining - 1) * sizeof(int));
                    for (int j = 0, k = 0; j < num_remaining; j++) {
                        if (j != i) new_remaining[k++] = remaining[j];
                    }

                    if (solve(grid, width, height, pieces, num_pieces, new_remaining, num_remaining - 1, empty_budget)) {
                        free(new_remaining);
                        return true;
                    }

                    free(new_remaining);
                    unplace_shape(grid, shape, row, col);
                }
            }
        }
    }

    // If no piece can cover this cell and we have empty budget
    if (!any_placed && empty_budget > 0) {
        grid[start_row][start_col] = true;
        if (solve(grid, width, height, pieces, num_pieces, remaining, num_remaining, empty_budget - 1)) {
            return true;
        }
        grid[start_row][start_col] = false;
    }

    return false;
}

static bool can_fit(ShapeSet* all_orientations, int num_shapes, int width, int height, int* counts, int num_counts) {
    // Collect all pieces
    int total_pieces = 0;
    int total_cells = 0;

    for (int i = 0; i < num_counts && i < num_shapes; i++) {
        total_pieces += counts[i];
        if (counts[i] > 0) {
            total_cells += counts[i] * all_orientations[i].orientations[0].num_cells;
        }
    }

    if (total_cells > width * height) return false;

    // Build pieces array
    ShapeSet* pieces = malloc(total_pieces * sizeof(ShapeSet));
    int* piece_sizes = malloc(total_pieces * sizeof(int));
    int piece_count = 0;

    for (int i = 0; i < num_counts && i < num_shapes; i++) {
        for (int j = 0; j < counts[i]; j++) {
            pieces[piece_count] = all_orientations[i];
            piece_sizes[piece_count] = all_orientations[i].orientations[0].num_cells;
            piece_count++;
        }
    }

    // Sort by size (largest first) - simple bubble sort
    for (int i = 0; i < piece_count - 1; i++) {
        for (int j = i + 1; j < piece_count; j++) {
            if (piece_sizes[j] > piece_sizes[i]) {
                ShapeSet tmp_s = pieces[i];
                pieces[i] = pieces[j];
                pieces[j] = tmp_s;
                int tmp_i = piece_sizes[i];
                piece_sizes[i] = piece_sizes[j];
                piece_sizes[j] = tmp_i;
            }
        }
    }

    // Create grid
    bool** grid = malloc(height * sizeof(bool*));
    for (int r = 0; r < height; r++) {
        grid[r] = calloc(width, sizeof(bool));
    }

    int* remaining = malloc(piece_count * sizeof(int));
    for (int i = 0; i < piece_count; i++) {
        remaining[i] = i;
    }

    int empty_budget = width * height - total_cells;
    bool result = solve(grid, width, height, pieces, piece_count, remaining, piece_count, empty_budget);

    // Cleanup
    for (int r = 0; r < height; r++) {
        free(grid[r]);
    }
    free(grid);
    free(remaining);
    free(pieces);
    free(piece_sizes);

    return result;
}

static int64_t part_one(const char* input) {
    // Parse shapes and regions
    Shape* shapes = malloc(16 * sizeof(Shape));
    int num_shapes = 0;
    int shape_capacity = 16;

    Region* regions = malloc(16 * sizeof(Region));
    int num_regions = 0;
    int region_capacity = 16;

    // Split by double newlines
    const char* p = input;
    while (*p) {
        // Skip leading whitespace
        while (*p && (*p == '\n' || *p == ' ')) p++;
        if (!*p) break;

        // Check if this is a shape definition (starts with digit and colon)
        if (*p >= '0' && *p <= '9') {
            const char* colon = strchr(p, ':');
            const char* x_pos = strchr(p, 'x');

            if (colon && (!x_pos || colon < x_pos)) {
                // This is a shape definition
                const char* line_end = strchr(p, '\n');
                if (!line_end) line_end = p + strlen(p);

                // Skip to next line (the shape pattern)
                p = line_end;
                while (*p == '\n') p++;

                if (num_shapes >= shape_capacity) {
                    shape_capacity *= 2;
                    shapes = realloc(shapes, shape_capacity * sizeof(Shape));
                }

                shapes[num_shapes].cells = malloc(64 * sizeof(Cell));
                shapes[num_shapes].num_cells = 0;

                int row = 0;
                while (*p && *p != '\n' && *p != '\n') {
                    int col = 0;
                    while (*p && *p != '\n') {
                        if (*p == '#') {
                            shapes[num_shapes].cells[shapes[num_shapes].num_cells].row = row;
                            shapes[num_shapes].cells[shapes[num_shapes].num_cells].col = col;
                            shapes[num_shapes].num_cells++;
                        }
                        col++;
                        p++;
                    }
                    if (*p == '\n') p++;
                    row++;

                    // Check if next line is part of shape
                    if (!*p || (*p >= '0' && *p <= '9') || *p == '\n') break;
                }

                num_shapes++;
            } else if (x_pos) {
                // This is a region definition (e.g., "4x4: 0 0 0 0 2 0")
                while (*p) {
                    // Find 'x'
                    while (*p && !(*p >= '0' && *p <= '9')) p++;
                    if (!*p) break;

                    int width = 0;
                    while (*p >= '0' && *p <= '9') {
                        width = width * 10 + (*p - '0');
                        p++;
                    }

                    if (*p != 'x') break;
                    p++;

                    int height = 0;
                    while (*p >= '0' && *p <= '9') {
                        height = height * 10 + (*p - '0');
                        p++;
                    }

                    while (*p && *p == ':') p++;

                    // Parse counts
                    int* counts = malloc(16 * sizeof(int));
                    int count_count = 0;
                    int count_capacity = 16;

                    while (*p && *p != '\n') {
                        while (*p && (*p == ' ' || *p == '\t')) p++;
                        if (!*p || *p == '\n') break;

                        int num = 0;
                        while (*p >= '0' && *p <= '9') {
                            num = num * 10 + (*p - '0');
                            p++;
                        }

                        if (count_count >= count_capacity) {
                            count_capacity *= 2;
                            counts = realloc(counts, count_capacity * sizeof(int));
                        }
                        counts[count_count++] = num;
                    }

                    if (count_count > 0) {
                        if (num_regions >= region_capacity) {
                            region_capacity *= 2;
                            regions = realloc(regions, region_capacity * sizeof(Region));
                        }
                        regions[num_regions].width = width;
                        regions[num_regions].height = height;
                        regions[num_regions].counts = counts;
                        regions[num_regions].num_counts = count_count;
                        num_regions++;
                    } else {
                        free(counts);
                    }

                    while (*p && *p == '\n') p++;
                    if (!*p || !(*p >= '0' && *p <= '9')) break;
                }
            } else {
                // Skip line
                while (*p && *p != '\n') p++;
                if (*p == '\n') p++;
            }
        } else {
            // Skip line
            while (*p && *p != '\n') p++;
            if (*p == '\n') p++;
        }
    }

    // Pre-compute all orientations
    ShapeSet* all_orientations = malloc(num_shapes * sizeof(ShapeSet));
    for (int i = 0; i < num_shapes; i++) {
        all_orientations[i] = get_orientations(&shapes[i]);
    }

    // Count regions that can be filled
    int64_t count = 0;
    for (int i = 0; i < num_regions; i++) {
        if (can_fit(all_orientations, num_shapes, regions[i].width, regions[i].height,
                    regions[i].counts, regions[i].num_counts)) {
            count++;
        }
    }

    // Cleanup
    for (int i = 0; i < num_shapes; i++) {
        free(shapes[i].cells);
        for (int j = 0; j < all_orientations[i].num_orientations; j++) {
            free(all_orientations[i].orientations[j].cells);
        }
        free(all_orientations[i].orientations);
    }
    free(shapes);
    free(all_orientations);

    for (int i = 0; i < num_regions; i++) {
        free(regions[i].counts);
    }
    free(regions);

    return count;
}

static int64_t part_two(const char* input) {
    (void)input;
    return 0;
}

DayResult day12(const char* input) {
    return (DayResult){
        .part1 = part_one(input),
        .part2 = part_two(input)
    };
}
