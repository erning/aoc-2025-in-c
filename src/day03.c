// Day 3: Lobby - Extract maximum value by selecting digits in order
// Greedy algorithm: pick largest digit that leaves enough remaining

#include "common.h"

static int max_two_digit(const char* bank) {
    int len = strlen(bank);
    int max_val = 0;

    for (int i = 0; i < len; i++) {
        for (int j = i + 1; j < len; j++) {
            int val = (bank[i] - '0') * 10 + (bank[j] - '0');
            if (val > max_val) max_val = val;
        }
    }

    return max_val;
}

static int64_t part_one(const char* input) {
    int count;
    char** lines = split_lines(input, &count);

    int64_t sum = 0;
    for (int i = 0; i < count; i++) {
        if (lines[i][0]) {
            sum += max_two_digit(lines[i]);
        }
    }

    free_lines(lines, count);
    return sum;
}

// Greedy: for each position, pick the largest available digit
// that still leaves enough digits remaining for the rest
static int64_t max_n_digits(const char* bank, int n) {
    int len = strlen(bank);
    if (n > len) return 0;

    int64_t result = 0;
    int start = 0;

    for (int i = 0; i < n; i++) {
        int remaining_picks = n - i - 1;
        int end = len - remaining_picks;

        // Find the largest digit in range [start, end)
        int best_idx = start;
        for (int j = start; j < end; j++) {
            if (bank[j] > bank[best_idx]) {
                best_idx = j;
            }
        }

        result = result * 10 + (bank[best_idx] - '0');
        start = best_idx + 1;
    }

    return result;
}

static int64_t part_two(const char* input) {
    int count;
    char** lines = split_lines(input, &count);

    int64_t sum = 0;
    for (int i = 0; i < count; i++) {
        if (lines[i][0]) {
            sum += max_n_digits(lines[i], 12);
        }
    }

    free_lines(lines, count);
    return sum;
}

DayResult day03(const char* input) {
    return (DayResult){
        .part1 = part_one(input),
        .part2 = part_two(input)
    };
}
