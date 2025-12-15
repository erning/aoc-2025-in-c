// Day 10: Factory - Gaussian elimination over GF(2) and integer linear programming

#include "common.h"

typedef struct {
    bool* lights;
    int num_lights;
    int** buttons;
    int* button_sizes;
    int num_buttons;
    int64_t* joltage;
    int num_joltage;
} PuzzleLine;

static PuzzleLine parse_line(const char* line) {
    PuzzleLine p = {0};

    // Find brackets for lights [.##.]
    const char* bracket_start = strchr(line, '[');
    const char* bracket_end = strchr(line, ']');

    if (bracket_start && bracket_end) {
        int len = bracket_end - bracket_start - 1;
        p.num_lights = len;
        p.lights = malloc(len * sizeof(bool));
        for (int i = 0; i < len; i++) {
            p.lights[i] = (bracket_start[1 + i] == '#');
        }
    }

    // Parse buttons (x,y,z)
    int btn_capacity = 8;
    p.buttons = malloc(btn_capacity * sizeof(int*));
    p.button_sizes = malloc(btn_capacity * sizeof(int));
    p.num_buttons = 0;

    const char* ptr = bracket_end + 1;
    while (*ptr && *ptr != '{') {
        if (*ptr == '(') {
            const char* paren_end = strchr(ptr, ')');
            if (paren_end) {
                // Parse indices
                int indices[32];
                int idx_count = 0;

                const char* ip = ptr + 1;
                while (ip < paren_end) {
                    while (ip < paren_end && (*ip == ' ' || *ip == ',')) ip++;
                    if (ip >= paren_end) break;

                    int num = 0;
                    while (ip < paren_end && *ip >= '0' && *ip <= '9') {
                        num = num * 10 + (*ip - '0');
                        ip++;
                    }
                    indices[idx_count++] = num;
                }

                if (p.num_buttons >= btn_capacity) {
                    btn_capacity *= 2;
                    p.buttons = realloc(p.buttons, btn_capacity * sizeof(int*));
                    p.button_sizes = realloc(p.button_sizes, btn_capacity * sizeof(int));
                }

                p.buttons[p.num_buttons] = malloc(idx_count * sizeof(int));
                memcpy(p.buttons[p.num_buttons], indices, idx_count * sizeof(int));
                p.button_sizes[p.num_buttons] = idx_count;
                p.num_buttons++;

                ptr = paren_end + 1;
            } else {
                ptr++;
            }
        } else {
            ptr++;
        }
    }

    // Parse joltage {x,y,z}
    const char* brace_start = strchr(line, '{');
    const char* brace_end = strchr(line, '}');

    if (brace_start && brace_end) {
        int jolt_capacity = 8;
        p.joltage = malloc(jolt_capacity * sizeof(int64_t));
        p.num_joltage = 0;

        const char* jp = brace_start + 1;
        while (jp < brace_end) {
            while (jp < brace_end && (*jp == ' ' || *jp == ',')) jp++;
            if (jp >= brace_end) break;

            int64_t num = 0;
            while (jp < brace_end && *jp >= '0' && *jp <= '9') {
                num = num * 10 + (*jp - '0');
                jp++;
            }

            if (p.num_joltage >= jolt_capacity) {
                jolt_capacity *= 2;
                p.joltage = realloc(p.joltage, jolt_capacity * sizeof(int64_t));
            }
            p.joltage[p.num_joltage++] = num;
        }
    }

    return p;
}

static void free_puzzle_line(PuzzleLine* p) {
    free(p->lights);
    for (int i = 0; i < p->num_buttons; i++) {
        free(p->buttons[i]);
    }
    free(p->buttons);
    free(p->button_sizes);
    free(p->joltage);
}

// Solve lights puzzle using brute force
static int solve_machine(PuzzleLine* p) {
    int n_lights = p->num_lights;
    int n_buttons = p->num_buttons;

    if (n_buttons > 20) return 0;  // Too many for brute force

    int min_presses = INT_MAX;

    for (int mask = 0; mask < (1 << n_buttons); mask++) {
        bool* state = calloc(n_lights, sizeof(bool));
        int presses = 0;

        for (int i = 0; i < n_buttons; i++) {
            if (mask & (1 << i)) {
                presses++;
                for (int j = 0; j < p->button_sizes[i]; j++) {
                    int idx = p->buttons[i][j];
                    if (idx < n_lights) {
                        state[idx] = !state[idx];
                    }
                }
            }
        }

        bool matches = true;
        for (int i = 0; i < n_lights; i++) {
            if (state[i] != p->lights[i]) {
                matches = false;
                break;
            }
        }

        if (matches && presses < min_presses) {
            min_presses = presses;
        }

        free(state);
    }

    return min_presses == INT_MAX ? 0 : min_presses;
}

static int64_t part_one(const char* input) {
    int line_count;
    char** lines = split_lines(input, &line_count);

    int64_t sum = 0;
    for (int i = 0; i < line_count; i++) {
        if (!lines[i][0]) continue;

        PuzzleLine p = parse_line(lines[i]);
        sum += solve_machine(&p);
        free_puzzle_line(&p);
    }

    free_lines(lines, line_count);
    return sum;
}

static int64_t gcd(int64_t a, int64_t b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b != 0) {
        int64_t t = b;
        b = a % b;
        a = t;
    }
    return a;
}

// Recursive search with pruning
static void search_solution(int64_t** matrix, int* pivot_cols, int pivot_count,
                           int* free_cols, int free_count, int n_buttons,
                           int free_idx, int64_t* solution, int64_t max_val,
                           int64_t* min_presses, int64_t current_sum) {
    // Pruning: if current sum of free variables already >= best solution, skip
    if (current_sum >= *min_presses) {
        return;
    }

    if (free_idx == free_count) {
        // All free variables set, compute pivot variables
        int64_t pivot_sum = 0;

        for (int r = pivot_count - 1; r >= 0; r--) {
            int col = pivot_cols[r];
            int64_t sum = matrix[r][n_buttons];

            for (int c = col + 1; c < n_buttons; c++) {
                sum -= matrix[r][c] * solution[c];
            }

            if (matrix[r][col] == 0 || sum % matrix[r][col] != 0) {
                return;
            }

            int64_t val = sum / matrix[r][col];
            if (val < 0) {
                return; // Early exit if negative
            }
            solution[col] = val;
            pivot_sum += val;

            // Early pruning during back-substitution
            if (current_sum + pivot_sum >= *min_presses) {
                return;
            }
        }

        int64_t total = current_sum + pivot_sum;
        if (total < *min_presses) {
            *min_presses = total;
        }
        return;
    }

    int col = free_cols[free_idx];
    // Limit max_val based on remaining budget
    int64_t remaining_budget = *min_presses - current_sum - 1;
    int64_t effective_max = max_val;
    if (remaining_budget >= 0 && remaining_budget < effective_max) {
        effective_max = remaining_budget;
    }
    if (remaining_budget < 0) {
        return;
    }

    for (int64_t val = 0; val <= effective_max; val++) {
        solution[col] = val;
        search_solution(matrix, pivot_cols, pivot_count, free_cols, free_count,
                       n_buttons, free_idx + 1, solution, max_val, min_presses,
                       current_sum + val);
    }
    solution[col] = 0;
}

// Solve joltage puzzle (integer linear programming)
static int64_t solve_joltage(PuzzleLine* p) {
    int n_counters = p->num_joltage;
    int n_buttons = p->num_buttons;

    if (n_counters == 0 || n_buttons == 0) return 0;

    // Build coefficient matrix
    int64_t** a = malloc(n_counters * sizeof(int64_t*));
    for (int j = 0; j < n_counters; j++) {
        a[j] = calloc(n_buttons, sizeof(int64_t));
    }

    for (int i = 0; i < n_buttons; i++) {
        for (int k = 0; k < p->button_sizes[i]; k++) {
            int j = p->buttons[i][k];
            if (j < n_counters) {
                a[j][i] = 1;
            }
        }
    }

    // Build augmented matrix [A | b]
    int64_t** matrix = malloc(n_counters * sizeof(int64_t*));
    for (int j = 0; j < n_counters; j++) {
        matrix[j] = malloc((n_buttons + 1) * sizeof(int64_t));
        for (int i = 0; i < n_buttons; i++) {
            matrix[j][i] = a[j][i];
        }
        matrix[j][n_buttons] = p->joltage[j];
    }

    // Gaussian elimination
    int* pivot_cols = malloc(n_buttons * sizeof(int));
    int pivot_count = 0;
    int pivot_row = 0;

    for (int col = 0; col < n_buttons && pivot_row < n_counters; col++) {
        // Find non-zero pivot
        int found = -1;
        for (int row = pivot_row; row < n_counters; row++) {
            if (matrix[row][col] != 0) {
                found = row;
                break;
            }
        }

        if (found < 0) continue;

        // Swap rows
        int64_t* tmp = matrix[pivot_row];
        matrix[pivot_row] = matrix[found];
        matrix[found] = tmp;

        pivot_cols[pivot_count++] = col;

        // Eliminate
        for (int row = 0; row < n_counters; row++) {
            if (row != pivot_row && matrix[row][col] != 0) {
                int64_t g = gcd(matrix[pivot_row][col], matrix[row][col]);
                int64_t mult_pivot = matrix[row][col] / g;
                int64_t mult_row = matrix[pivot_row][col] / g;

                for (int c = 0; c <= n_buttons; c++) {
                    matrix[row][c] = matrix[row][c] * mult_row - matrix[pivot_row][c] * mult_pivot;
                }
            }
        }

        pivot_row++;
    }

    // Check for inconsistency
    for (int row = pivot_row; row < n_counters; row++) {
        if (matrix[row][n_buttons] != 0) {
            // No solution
            for (int j = 0; j < n_counters; j++) {
                free(matrix[j]);
                free(a[j]);
            }
            free(matrix);
            free(a);
            free(pivot_cols);
            return 0;
        }
    }

    // Free variables
    bool* is_pivot = calloc(n_buttons, sizeof(bool));
    for (int i = 0; i < pivot_count; i++) {
        is_pivot[pivot_cols[i]] = true;
    }

    int* free_cols = malloc(n_buttons * sizeof(int));
    int free_count = 0;
    for (int c = 0; c < n_buttons; c++) {
        if (!is_pivot[c]) {
            free_cols[free_count++] = c;
        }
    }

    // Search for minimum solution
    int64_t min_presses = INT64_MAX;
    int64_t max_search = 0;
    for (int i = 0; i < n_counters; i++) {
        if (p->joltage[i] > max_search) max_search = p->joltage[i];
    }
    if (max_search < 50) max_search = 50;

    int64_t* solution = calloc(n_buttons, sizeof(int64_t));

    // Use recursive search with pruning
    search_solution(matrix, pivot_cols, pivot_count, free_cols, free_count,
                   n_buttons, 0, solution, max_search, &min_presses, 0);

    // Cleanup
    free(solution);
    free(free_cols);
    free(is_pivot);
    free(pivot_cols);
    for (int j = 0; j < n_counters; j++) {
        free(matrix[j]);
        free(a[j]);
    }
    free(matrix);
    free(a);

    return min_presses == INT64_MAX ? 0 : min_presses;
}

static int64_t part_two(const char* input) {
    int line_count;
    char** lines = split_lines(input, &line_count);

    int64_t sum = 0;
    for (int i = 0; i < line_count; i++) {
        if (!lines[i][0]) continue;

        PuzzleLine p = parse_line(lines[i]);
        sum += solve_joltage(&p);
        free_puzzle_line(&p);
    }

    free_lines(lines, line_count);
    return sum;
}

DayResult day10(const char* input) {
    return (DayResult){
        .part1 = part_one(input),
        .part2 = part_two(input)
    };
}
