#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>

// Read entire file into a string (caller must free)
char* read_file(const char* filename);

// Read input file for a specific day
char* read_input(int day);

// Read example file for a specific day
char* read_example(int day);

// Read custom file for a specific day
char* read_as_string(int day, const char* name);

// Utility: count lines in a string
int count_lines(const char* str);

// Utility: split string by delimiter, returns array of strings (caller must free)
char** split_lines(const char* str, int* count);

// Utility: free array of strings
void free_lines(char** lines, int count);

// Day solver function type
typedef struct {
    int64_t part1;
    int64_t part2;
} DayResult;

// Function pointer type for day solvers
typedef DayResult (*DaySolver)(const char* input);

#endif // COMMON_H
