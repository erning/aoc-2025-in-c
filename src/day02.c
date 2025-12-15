// Day 2: Gift Shop - Find doubled and repeated pattern numbers
// Doubled: digits are repeated (11, 1010, 222333222333)
// Repeated: any pattern repeated at least twice

#include "common.h"

typedef struct {
    uint64_t start;
    uint64_t end;
} Range;

static int count_digits(uint64_t n) {
    if (n == 0) return 1;
    int count = 0;
    while (n > 0) {
        count++;
        n /= 10;
    }
    return count;
}

static uint64_t power10(int exp) {
    uint64_t result = 1;
    for (int i = 0; i < exp; i++) {
        result *= 10;
    }
    return result;
}

static Range* parse_input(const char* input, int* count) {
    // Count commas to determine number of ranges
    int num_ranges = 1;
    for (const char* p = input; *p; p++) {
        if (*p == ',') num_ranges++;
    }

    Range* ranges = malloc(num_ranges * sizeof(Range));
    *count = 0;

    const char* p = input;
    while (*p) {
        // Skip whitespace and commas
        while (*p && (*p == ' ' || *p == ',' || *p == '\n')) p++;
        if (!*p) break;

        // Parse start
        uint64_t start = 0;
        while (*p >= '0' && *p <= '9') {
            start = start * 10 + (*p - '0');
            p++;
        }

        // Skip dash
        while (*p && *p == '-') p++;

        // Parse end
        uint64_t end = 0;
        while (*p >= '0' && *p <= '9') {
            end = end * 10 + (*p - '0');
            p++;
        }

        ranges[*count].start = start;
        ranges[*count].end = end;
        (*count)++;
    }

    return ranges;
}

// Generate all doubled numbers within a range and sum them
static uint64_t sum_doubled_in_range(uint64_t start, uint64_t end) {
    uint64_t sum = 0;

    int start_digits = count_digits(start);
    int end_digits = count_digits(end);

    for (int total_digits = start_digits; total_digits <= end_digits; total_digits++) {
        if (total_digits % 2 != 0) continue;

        int half_digits = total_digits / 2;
        uint64_t min_half = power10(half_digits - 1);
        uint64_t max_half = power10(half_digits) - 1;
        uint64_t multiplier = power10(half_digits);

        for (uint64_t half = min_half; half <= max_half; half++) {
            uint64_t doubled = half * multiplier + half;
            if (doubled >= start && doubled <= end) {
                sum += doubled;
            }
        }
    }

    return sum;
}

static int64_t part_one(const char* input) {
    int count;
    Range* ranges = parse_input(input, &count);

    uint64_t sum = 0;
    for (int i = 0; i < count; i++) {
        sum += sum_doubled_in_range(ranges[i].start, ranges[i].end);
    }

    free(ranges);
    return (int64_t)sum;
}

// For part 2, we need to track seen numbers to avoid duplicates
#define HASH_SIZE 10007

typedef struct HashNode {
    uint64_t value;
    struct HashNode* next;
} HashNode;

typedef struct {
    HashNode* buckets[HASH_SIZE];
} HashSet;

static void hashset_init(HashSet* set) {
    memset(set->buckets, 0, sizeof(set->buckets));
}

static void hashset_free(HashSet* set) {
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode* node = set->buckets[i];
        while (node) {
            HashNode* next = node->next;
            free(node);
            node = next;
        }
    }
}

static bool hashset_contains(HashSet* set, uint64_t value) {
    int idx = value % HASH_SIZE;
    HashNode* node = set->buckets[idx];
    while (node) {
        if (node->value == value) return true;
        node = node->next;
    }
    return false;
}

static void hashset_insert(HashSet* set, uint64_t value) {
    if (hashset_contains(set, value)) return;
    int idx = value % HASH_SIZE;
    HashNode* node = malloc(sizeof(HashNode));
    node->value = value;
    node->next = set->buckets[idx];
    set->buckets[idx] = node;
}

// Generate all repeated-pattern numbers within a range and sum them
static uint64_t sum_repeated_in_range(uint64_t start, uint64_t end) {
    uint64_t sum = 0;
    HashSet seen;
    hashset_init(&seen);

    int start_digits = count_digits(start);
    int end_digits = count_digits(end);

    for (int total_digits = start_digits; total_digits <= end_digits; total_digits++) {
        // Try each possible pattern length (1 to total_digits/2)
        for (int pattern_len = 1; pattern_len <= total_digits / 2; pattern_len++) {
            if (total_digits % pattern_len != 0) continue;

            int repeat_count = total_digits / pattern_len;
            if (repeat_count < 2) continue;

            // Generate all patterns of this length
            uint64_t min_pattern = (pattern_len == 1) ? 1 : power10(pattern_len - 1);
            uint64_t max_pattern = power10(pattern_len) - 1;
            uint64_t multiplier = power10(pattern_len);

            for (uint64_t pattern = min_pattern; pattern <= max_pattern; pattern++) {
                // Build the repeated number
                uint64_t num = 0;
                for (int i = 0; i < repeat_count; i++) {
                    num = num * multiplier + pattern;
                }

                if (num >= start && num <= end && !hashset_contains(&seen, num)) {
                    hashset_insert(&seen, num);
                    sum += num;
                }
            }
        }
    }

    hashset_free(&seen);
    return sum;
}

static int64_t part_two(const char* input) {
    int count;
    Range* ranges = parse_input(input, &count);

    uint64_t sum = 0;
    for (int i = 0; i < count; i++) {
        sum += sum_repeated_in_range(ranges[i].start, ranges[i].end);
    }

    free(ranges);
    return (int64_t)sum;
}

DayResult day02(const char* input) {
    return (DayResult){
        .part1 = part_one(input),
        .part2 = part_two(input)
    };
}
