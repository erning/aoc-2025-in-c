// Day 1: Secret Entrance - Circular dial puzzle
// A 100-position circular dial, starting at position 50

#include "common.h"

#define DIAL_SIZE 100

typedef struct {
    int sign;   // +1 for R, -1 for L
    int steps;
} Instruction;

// Proper modulo for negative numbers
static int mod(int n, int m) {
    return ((n % m) + m) % m;
}

static Instruction* parse_input(const char* input, int* count) {
    int line_count;
    char** lines = split_lines(input, &line_count);

    Instruction* instructions = malloc(line_count * sizeof(Instruction));
    *count = 0;

    for (int i = 0; i < line_count; i++) {
        if (lines[i][0] == '\0') continue;

        instructions[*count].sign = (lines[i][0] == 'R') ? 1 : -1;
        instructions[*count].steps = atoi(lines[i] + 1);
        (*count)++;
    }

    free_lines(lines, line_count);
    return instructions;
}

static int64_t part_one(const char* input) {
    int count;
    Instruction* instructions = parse_input(input, &count);

    int position = 50;
    int result = 0;

    for (int i = 0; i < count; i++) {
        position = mod(position + instructions[i].sign * instructions[i].steps, DIAL_SIZE);
        if (position == 0) {
            result++;
        }
    }

    free(instructions);
    return result;
}

static int64_t part_two(const char* input) {
    int count;
    Instruction* instructions = parse_input(input, &count);

    int position = 50;
    int result = 0;

    for (int i = 0; i < count; i++) {
        int sign = instructions[i].sign;
        int steps = instructions[i].steps;

        // Each full rotation crosses 0 once
        result += steps / DIAL_SIZE;

        int remainder = steps % DIAL_SIZE;
        int new_position = mod(position + sign * remainder, DIAL_SIZE);

        // Check if we crossed 0 (not counting starting from 0)
        bool crossed = false;
        if (position != 0) {
            if (sign > 0) {
                crossed = (position + remainder >= DIAL_SIZE);
            } else {
                crossed = (position < remainder);
            }
        }

        // Landing on 0 counts (but not if we started from 0 and didn't move)
        bool landed_on_zero = (new_position == 0) && (position != 0 || remainder > 0);

        if (crossed || landed_on_zero) {
            result++;
        }

        position = new_position;
    }

    free(instructions);
    return result;
}

DayResult day01(const char* input) {
    return (DayResult){
        .part1 = part_one(input),
        .part2 = part_two(input)
    };
}
