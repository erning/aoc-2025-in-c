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
    int count = 0;
    for (const char* p = str; *p; p++) {
        if (*p == '\n') count++;
    }
    // Count last line if it doesn't end with newline
    if (str[0] && str[strlen(str) - 1] != '\n') count++;
    return count;
}

char** split_lines(const char* str, int* count) {
    *count = count_lines(str);
    if (*count == 0) return NULL;

    char** lines = malloc(*count * sizeof(char*));
    if (!lines) return NULL;

    const char* start = str;
    int idx = 0;

    for (const char* p = str; ; p++) {
        if (*p == '\n' || *p == '\0') {
            size_t len = p - start;
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

    // Adjust count if we have trailing newlines
    while (*count > 0 && lines[*count - 1][0] == '\0') {
        free(lines[*count - 1]);
        (*count)--;
    }

    return lines;
}

void free_lines(char** lines, int count) {
    if (!lines) return;
    for (int i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
}
