// Day 5: Cafeteria - Range checking and merging

#include "common.h"

typedef struct {
    int64_t start;
    int64_t end;
} Range;

static void parse_input(const char* input, Range** ranges, int* range_count,
                        int64_t** ids, int* id_count) {
    // Find the blank line separator
    const char* blank = strstr(input, "\n\n");
    if (!blank) {
        *range_count = 0;
        *id_count = 0;
        return;
    }

    // Parse ranges (first part)
    int r_capacity = 16;
    *ranges = malloc(r_capacity * sizeof(Range));
    *range_count = 0;

    const char* p = input;
    while (p < blank) {
        while (p < blank && (*p == ' ' || *p == '\n')) p++;
        if (p >= blank) break;

        int64_t start = 0;
        while (p < blank && *p >= '0' && *p <= '9') {
            start = start * 10 + (*p - '0');
            p++;
        }

        while (p < blank && *p == '-') p++;

        int64_t end = 0;
        while (p < blank && *p >= '0' && *p <= '9') {
            end = end * 10 + (*p - '0');
            p++;
        }

        if (*range_count >= r_capacity) {
            r_capacity *= 2;
            *ranges = realloc(*ranges, r_capacity * sizeof(Range));
        }
        (*ranges)[*range_count].start = start;
        (*ranges)[*range_count].end = end;
        (*range_count)++;
    }

    // Parse IDs (second part)
    int i_capacity = 16;
    *ids = malloc(i_capacity * sizeof(int64_t));
    *id_count = 0;

    p = blank + 2;  // Skip "\n\n"
    while (*p) {
        while (*p && (*p == ' ' || *p == '\n')) p++;
        if (!*p) break;

        int64_t id = 0;
        while (*p >= '0' && *p <= '9') {
            id = id * 10 + (*p - '0');
            p++;
        }

        if (*id_count >= i_capacity) {
            i_capacity *= 2;
            *ids = realloc(*ids, i_capacity * sizeof(int64_t));
        }
        (*ids)[(*id_count)++] = id;
    }
}

static bool is_fresh(int64_t id, Range* ranges, int range_count) {
    for (int i = 0; i < range_count; i++) {
        if (id >= ranges[i].start && id <= ranges[i].end) {
            return true;
        }
    }
    return false;
}

static int64_t part_one(const char* input) {
    Range* ranges;
    int range_count;
    int64_t* ids;
    int id_count;

    parse_input(input, &ranges, &range_count, &ids, &id_count);

    int64_t count = 0;
    for (int i = 0; i < id_count; i++) {
        if (is_fresh(ids[i], ranges, range_count)) {
            count++;
        }
    }

    free(ranges);
    free(ids);
    return count;
}

static int compare_ranges(const void* a, const void* b) {
    const Range* ra = (const Range*)a;
    const Range* rb = (const Range*)b;
    if (ra->start < rb->start) return -1;
    if (ra->start > rb->start) return 1;
    return 0;
}

static int64_t part_two(const char* input) {
    Range* ranges;
    int range_count;
    int64_t* ids;
    int id_count;

    parse_input(input, &ranges, &range_count, &ids, &id_count);

    // Sort ranges by start
    qsort(ranges, range_count, sizeof(Range), compare_ranges);

    // Merge overlapping ranges
    Range* merged = malloc(range_count * sizeof(Range));
    int merged_count = 0;

    for (int i = 0; i < range_count; i++) {
        if (merged_count > 0 && ranges[i].start <= merged[merged_count - 1].end + 1) {
            // Merge with previous
            if (ranges[i].end > merged[merged_count - 1].end) {
                merged[merged_count - 1].end = ranges[i].end;
            }
        } else {
            merged[merged_count++] = ranges[i];
        }
    }

    // Count total IDs
    int64_t total = 0;
    for (int i = 0; i < merged_count; i++) {
        total += merged[i].end - merged[i].start + 1;
    }

    free(ranges);
    free(ids);
    free(merged);
    return total;
}

DayResult day05(const char* input) {
    return (DayResult){
        .part1 = part_one(input),
        .part2 = part_two(input)
    };
}
