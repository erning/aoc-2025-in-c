#include "common.h"
#include "days.h"

static DaySolver solvers[] = {
    day01, day02, day03, day04, day05, day06,
    day07, day08, day09, day10, day11, day12
};

static const int NUM_DAYS = sizeof(solvers) / sizeof(solvers[0]);

static void run_day(int day, bool use_example) {
    char* input = use_example ? read_example(day) : read_input(day);
    if (!input) {
        printf("Day %02d: Input not found\n", day);
        return;
    }

    clock_t start = clock();
    DayResult result = solvers[day - 1](input);
    clock_t end = clock();
    double time_ms = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;

    printf("Day %02d: Part 1 = %lld, Part 2 = %lld (%.2f ms)\n",
           day, (long long)result.part1, (long long)result.part2, time_ms);

    free(input);
}

int main(int argc, char* argv[]) {
    bool use_example = false;
    int specific_days[12];
    int num_specific = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--example") == 0) {
            use_example = true;
        } else {
            int day = atoi(argv[i]);
            if (day >= 1 && day <= NUM_DAYS) {
                specific_days[num_specific++] = day;
            }
        }
    }

    printf("Advent of Code 2025 - C23\n");
    printf("=========================\n\n");

    if (num_specific > 0) {
        for (int i = 0; i < num_specific; i++) {
            run_day(specific_days[i], use_example);
        }
    } else {
        for (int day = 1; day <= NUM_DAYS; day++) {
            run_day(day, use_example);
        }
    }

    return 0;
}
