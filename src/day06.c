// Day 6: Trash Compactor - Parse column-aligned arithmetic problems

#include "common.h"

typedef struct {
    char op;        // '+' or '*'
    int64_t* numbers;
    int num_count;
} Problem;

static int64_t solve_problem(Problem* p) {
    if (p->num_count == 0) return 0;

    int64_t result = p->numbers[0];
    for (int i = 1; i < p->num_count; i++) {
        if (p->op == '+') {
            result += p->numbers[i];
        } else {
            result *= p->numbers[i];
        }
    }
    return result;
}

static Problem* parse_problems_v1(const char* input, int* count) {
    int line_count;
    char** lines = split_lines(input, &line_count);
    if (line_count == 0) {
        *count = 0;
        return NULL;
    }

    // Find max line length and pad all lines
    int max_len = 0;
    for (int i = 0; i < line_count; i++) {
        int len = strlen(lines[i]);
        if (len > max_len) max_len = len;
    }

    char** padded = malloc(line_count * sizeof(char*));
    for (int i = 0; i < line_count; i++) {
        padded[i] = malloc(max_len + 1);
        int len = strlen(lines[i]);
        memcpy(padded[i], lines[i], len);
        memset(padded[i] + len, ' ', max_len - len);
        padded[i][max_len] = '\0';
    }

    // Parse problems
    int capacity = 16;
    Problem* problems = malloc(capacity * sizeof(Problem));
    *count = 0;

    int col = 0;
    while (col < max_len) {
        // Skip separator columns (all spaces)
        bool all_space = true;
        for (int i = 0; i < line_count; i++) {
            if (padded[i][col] != ' ') {
                all_space = false;
                break;
            }
        }
        if (all_space) {
            col++;
            continue;
        }

        // Find end of this problem
        int end_col = col + 1;
        while (end_col < max_len) {
            all_space = true;
            for (int i = 0; i < line_count; i++) {
                if (padded[i][end_col] != ' ') {
                    all_space = false;
                    break;
                }
            }
            if (all_space) break;
            end_col++;
        }

        // Extract this problem
        if (*count >= capacity) {
            capacity *= 2;
            problems = realloc(problems, capacity * sizeof(Problem));
        }

        Problem* p = &problems[*count];
        p->op = '+';
        p->numbers = malloc(line_count * sizeof(int64_t));
        p->num_count = 0;

        for (int i = 0; i < line_count; i++) {
            char segment[256];
            int seg_len = end_col - col;
            memcpy(segment, padded[i] + col, seg_len);
            segment[seg_len] = '\0';

            // Trim
            char* start = segment;
            while (*start == ' ') start++;
            char* end = start + strlen(start) - 1;
            while (end > start && *end == ' ') end--;
            *(end + 1) = '\0';

            if (strcmp(start, "+") == 0) {
                p->op = '+';
            } else if (strcmp(start, "*") == 0) {
                p->op = '*';
            } else if (*start) {
                // Try to parse as number
                char* endptr;
                int64_t num = strtoll(start, &endptr, 10);
                if (*endptr == '\0') {
                    p->numbers[p->num_count++] = num;
                }
            }
        }

        if (p->num_count > 0) {
            (*count)++;
        } else {
            free(p->numbers);
        }

        col = end_col;
    }

    // Cleanup
    for (int i = 0; i < line_count; i++) {
        free(padded[i]);
    }
    free(padded);
    free_lines(lines, line_count);

    return problems;
}

static int64_t part_one(const char* input) {
    int count;
    Problem* problems = parse_problems_v1(input, &count);

    int64_t sum = 0;
    for (int i = 0; i < count; i++) {
        sum += solve_problem(&problems[i]);
        free(problems[i].numbers);
    }
    free(problems);

    return sum;
}

static Problem* parse_problems_v2(const char* input, int* count) {
    int line_count;
    char** lines = split_lines(input, &line_count);
    if (line_count == 0) {
        *count = 0;
        return NULL;
    }

    // Find max line length and pad all lines
    int max_len = 0;
    for (int i = 0; i < line_count; i++) {
        int len = strlen(lines[i]);
        if (len > max_len) max_len = len;
    }

    char** padded = malloc(line_count * sizeof(char*));
    for (int i = 0; i < line_count; i++) {
        padded[i] = malloc(max_len + 1);
        int len = strlen(lines[i]);
        memcpy(padded[i], lines[i], len);
        memset(padded[i] + len, ' ', max_len - len);
        padded[i][max_len] = '\0';
    }

    int num_rows = line_count;

    // Parse problems right to left
    int capacity = 16;
    Problem* problems = malloc(capacity * sizeof(Problem));
    *count = 0;

    int col = max_len;
    while (col > 0) {
        col--;

        // Skip separator columns (all spaces)
        bool all_space = true;
        for (int i = 0; i < num_rows; i++) {
            if (padded[i][col] != ' ') {
                all_space = false;
                break;
            }
        }
        if (all_space) continue;

        // Find start of this problem
        int start_col = col;
        while (start_col > 0) {
            all_space = true;
            for (int i = 0; i < num_rows; i++) {
                if (padded[i][start_col - 1] != ' ') {
                    all_space = false;
                    break;
                }
            }
            if (all_space) break;
            start_col--;
        }

        // Get operator from last row
        char op = '+';
        for (int c = start_col; c <= col; c++) {
            char ch = padded[num_rows - 1][c];
            if (ch == '+' || ch == '*') {
                op = ch;
                break;
            }
        }

        // Read numbers column by column from right to left
        if (*count >= capacity) {
            capacity *= 2;
            problems = realloc(problems, capacity * sizeof(Problem));
        }

        Problem* p = &problems[*count];
        p->op = op;
        p->numbers = malloc((col - start_col + 1) * sizeof(int64_t));
        p->num_count = 0;

        for (int c = col; c >= start_col; c--) {
            // Read digits vertically (top to bottom) from all rows except last
            char num_str[64];
            int num_len = 0;

            for (int r = 0; r < num_rows - 1; r++) {
                char ch = padded[r][c];
                if (ch >= '0' && ch <= '9') {
                    num_str[num_len++] = ch;
                }
            }

            if (num_len > 0) {
                num_str[num_len] = '\0';
                p->numbers[p->num_count++] = strtoll(num_str, NULL, 10);
            }
        }

        if (p->num_count > 0) {
            (*count)++;
        } else {
            free(p->numbers);
        }

        col = start_col;
    }

    // Cleanup
    for (int i = 0; i < line_count; i++) {
        free(padded[i]);
    }
    free(padded);
    free_lines(lines, line_count);

    return problems;
}

static int64_t part_two(const char* input) {
    int count;
    Problem* problems = parse_problems_v2(input, &count);

    int64_t sum = 0;
    for (int i = 0; i < count; i++) {
        sum += solve_problem(&problems[i]);
        free(problems[i].numbers);
    }
    free(problems);

    return sum;
}

DayResult day06(const char* input) {
    return (DayResult){
        .part1 = part_one(input),
        .part2 = part_two(input)
    };
}
