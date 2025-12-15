// Day 9: Movie Theater - Rectangle fitting in polygon

#include "common.h"

typedef struct {
    int64_t x, y;
} Point;

static Point* parse_input(const char* input, int* count) {
    int line_count;
    char** lines = split_lines(input, &line_count);

    Point* points = malloc(line_count * sizeof(Point));
    *count = 0;

    for (int i = 0; i < line_count; i++) {
        if (!lines[i][0]) continue;

        int64_t x, y;
        if (sscanf(lines[i], "%ld,%ld", &x, &y) == 2) {
            points[*count].x = x;
            points[*count].y = y;
            (*count)++;
        }
    }

    free_lines(lines, line_count);
    return points;
}

static int64_t i64_abs(int64_t x) {
    return x < 0 ? -x : x;
}

static int64_t i64_max(int64_t a, int64_t b) {
    return a > b ? a : b;
}

static int64_t i64_min(int64_t a, int64_t b) {
    return a < b ? a : b;
}

static int64_t part_one(const char* input) {
    int n;
    Point* tiles = parse_input(input, &n);

    int64_t max_area = 0;

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int64_t width = i64_abs(tiles[j].x - tiles[i].x) + 1;
            int64_t height = i64_abs(tiles[j].y - tiles[i].y) + 1;
            int64_t area = width * height;
            max_area = i64_max(max_area, area);
        }
    }

    free(tiles);
    return max_area;
}

// Check if a point is inside or on the boundary of the polygon
static bool point_in_polygon(Point* tiles, int n, int64_t x, int64_t y) {
    // Check if on boundary
    for (int i = 0; i < n; i++) {
        int64_t x1 = tiles[i].x, y1 = tiles[i].y;
        int64_t x2 = tiles[(i + 1) % n].x, y2 = tiles[(i + 1) % n].y;

        if (x1 == x2) {
            // Vertical segment
            int64_t miny = i64_min(y1, y2);
            int64_t maxy = i64_max(y1, y2);
            if (x == x1 && y >= miny && y <= maxy) {
                return true;
            }
        } else {
            // Horizontal segment
            int64_t minx = i64_min(x1, x2);
            int64_t maxx = i64_max(x1, x2);
            if (y == y1 && x >= minx && x <= maxx) {
                return true;
            }
        }
    }

    // Ray casting for interior
    int crossings = 0;
    for (int i = 0; i < n; i++) {
        int64_t x1 = tiles[i].x, y1 = tiles[i].y;
        int64_t x2 = tiles[(i + 1) % n].x, y2 = tiles[(i + 1) % n].y;

        if (x1 == x2 && x1 < x) {
            // Vertical segment to the left of point
            if ((y1 < y && y2 >= y) || (y2 < y && y1 >= y)) {
                crossings++;
            }
        }
    }

    return crossings % 2 == 1;
}

static bool is_rect_fully_inside(Point* tiles, int n, int64_t lx, int64_t rx, int64_t ly, int64_t ry) {
    // Check four corners
    if (!point_in_polygon(tiles, n, lx, ly)) return false;
    if (!point_in_polygon(tiles, n, rx, ly)) return false;
    if (!point_in_polygon(tiles, n, lx, ry)) return false;
    if (!point_in_polygon(tiles, n, rx, ry)) return false;

    // Check all points on edges at key y values
    for (int i = 0; i < n; i++) {
        int64_t y = tiles[i].y;
        if (y > ly && y < ry) {
            if (!point_in_polygon(tiles, n, lx, y)) return false;
            if (!point_in_polygon(tiles, n, rx, y)) return false;
        }
    }

    // Check that no polygon edge crosses through the rectangle interior
    for (int i = 0; i < n; i++) {
        int64_t x1 = tiles[i].x, y1 = tiles[i].y;
        int64_t x2 = tiles[(i + 1) % n].x, y2 = tiles[(i + 1) % n].y;

        if (x1 == x2) {
            // Vertical segment
            int64_t miny = i64_min(y1, y2);
            int64_t maxy = i64_max(y1, y2);
            if (x1 > lx && x1 < rx) {
                if (miny < ry && maxy > ly) {
                    if (!(maxy >= ry && miny <= ly)) {
                        return false;
                    }
                }
            }
        } else {
            // Horizontal segment
            int64_t minx = i64_min(x1, x2);
            int64_t maxx = i64_max(x1, x2);
            if (y1 > ly && y1 < ry) {
                if (minx < rx && maxx > lx) {
                    if (!(maxx >= rx && minx <= lx)) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

static int64_t part_two(const char* input) {
    int n;
    Point* tiles = parse_input(input, &n);

    int64_t max_area = 0;

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int64_t lx = i64_min(tiles[i].x, tiles[j].x);
            int64_t rx = i64_max(tiles[i].x, tiles[j].x);
            int64_t ly = i64_min(tiles[i].y, tiles[j].y);
            int64_t ry = i64_max(tiles[i].y, tiles[j].y);

            if (is_rect_fully_inside(tiles, n, lx, rx, ly, ry)) {
                int64_t area = (rx - lx + 1) * (ry - ly + 1);
                max_area = i64_max(max_area, area);
            }
        }
    }

    free(tiles);
    return max_area;
}

DayResult day09(const char* input) {
    return (DayResult){
        .part1 = part_one(input),
        .part2 = part_two(input)
    };
}
