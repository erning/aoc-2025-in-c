#include "common.h"

char* read_file(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Cannot open file: %s\n", filename);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = malloc(size + 1);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    size_t read = fread(buffer, 1, size, f);
    buffer[read] = '\0';
    fclose(f);

    return buffer;
}

char* read_input(int day) {
    char filename[64];
    snprintf(filename, sizeof(filename), "inputs/%02d-input.txt", day);
    return read_file(filename);
}

char* read_example(int day) {
    char filename[64];
    snprintf(filename, sizeof(filename), "inputs/%02d-example.txt", day);
    return read_file(filename);
}

char* read_as_string(int day, const char* name) {
    char filename[64];
    snprintf(filename, sizeof(filename), "inputs/%02d-%s.txt", day, name);
    return read_file(filename);
}

int count_lines(const char* str) {
    if (!str || !*str) return 0;

    int count = 0;
    for (const char* p = str; *p; p++) {
        if (*p == '\n') count++;
    }
    // Count last line if it doesn't end with newline
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] != '\n') count++;
    return count;
}

char** split_lines(const char* str, int* count) {
    if (!str || !*str) {
        *count = 0;
        return NULL;
    }

    // First pass: count actual lines
    int capacity = 16;
    char** lines = malloc(capacity * sizeof(char*));
    if (!lines) {
        *count = 0;
        return NULL;
    }

    const char* start = str;
    int idx = 0;

    for (const char* p = str; ; p++) {
        if (*p == '\n' || *p == '\0') {
            size_t len = p - start;

            // Skip empty lines at the end
            if (len == 0 && *p == '\0') break;

            // Grow array if needed
            if (idx >= capacity) {
                capacity *= 2;
                char** new_lines = realloc(lines, capacity * sizeof(char*));
                if (!new_lines) {
                    free_lines(lines, idx);
                    *count = 0;
                    return NULL;
                }
                lines = new_lines;
            }

            lines[idx] = malloc(len + 1);
            if (lines[idx]) {
                memcpy(lines[idx], start, len);
                lines[idx][len] = '\0';
            }
            idx++;
            start = p + 1;
            if (*p == '\0') break;
        }
    }

    // Remove trailing empty lines
    while (idx > 0 && lines[idx - 1][0] == '\0') {
        free(lines[idx - 1]);
        idx--;
    }

    *count = idx;
    return lines;
}

void free_lines(char** lines, int count) {
    if (!lines) return;
    for (int i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
}
