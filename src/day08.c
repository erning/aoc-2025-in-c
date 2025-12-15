// Day 8: Playground - 3D points, Union-Find, minimum spanning tree

#include "common.h"

typedef struct {
    int64_t x, y, z;
} Point3D;

// Union-Find data structure
typedef struct {
    int* parent;
    int* rank;
    int* size;
    int n;
} UnionFind;

static UnionFind* uf_create(int n) {
    UnionFind* uf = malloc(sizeof(UnionFind));
    uf->parent = malloc(n * sizeof(int));
    uf->rank = malloc(n * sizeof(int));
    uf->size = malloc(n * sizeof(int));
    uf->n = n;

    for (int i = 0; i < n; i++) {
        uf->parent[i] = i;
        uf->rank[i] = 0;
        uf->size[i] = 1;
    }

    return uf;
}

static void uf_free(UnionFind* uf) {
    free(uf->parent);
    free(uf->rank);
    free(uf->size);
    free(uf);
}

static int uf_find(UnionFind* uf, int x) {
    if (uf->parent[x] != x) {
        uf->parent[x] = uf_find(uf, uf->parent[x]);
    }
    return uf->parent[x];
}

static bool uf_union(UnionFind* uf, int x, int y) {
    int px = uf_find(uf, x);
    int py = uf_find(uf, y);
    if (px == py) return false;

    if (uf->rank[px] < uf->rank[py]) {
        uf->parent[px] = py;
        uf->size[py] += uf->size[px];
    } else if (uf->rank[px] > uf->rank[py]) {
        uf->parent[py] = px;
        uf->size[px] += uf->size[py];
    } else {
        uf->parent[py] = px;
        uf->size[px] += uf->size[py];
        uf->rank[px]++;
    }

    return true;
}

typedef struct {
    int64_t dist;
    int i, j;
} Edge;

static int compare_edges(const void* a, const void* b) {
    const Edge* ea = (const Edge*)a;
    const Edge* eb = (const Edge*)b;
    if (ea->dist < eb->dist) return -1;
    if (ea->dist > eb->dist) return 1;
    return 0;
}

static Point3D* parse_input(const char* input, int* count) {
    int line_count;
    char** lines = split_lines(input, &line_count);

    Point3D* points = malloc(line_count * sizeof(Point3D));
    *count = 0;

    for (int i = 0; i < line_count; i++) {
        if (!lines[i][0]) continue;

        int64_t x, y, z;
        if (sscanf(lines[i], "%ld,%ld,%ld", &x, &y, &z) == 3) {
            points[*count].x = x;
            points[*count].y = y;
            points[*count].z = z;
            (*count)++;
        }
    }

    free_lines(lines, line_count);
    return points;
}

static int64_t distance_squared(Point3D* a, Point3D* b) {
    int64_t dx = a->x - b->x;
    int64_t dy = a->y - b->y;
    int64_t dz = a->z - b->z;
    return dx * dx + dy * dy + dz * dz;
}

static int64_t solve(const char* input, int connections) {
    int n;
    Point3D* points = parse_input(input, &n);

    // Calculate all pairwise distances
    int num_edges = n * (n - 1) / 2;
    Edge* edges = malloc(num_edges * sizeof(Edge));
    int edge_count = 0;

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            edges[edge_count].dist = distance_squared(&points[i], &points[j]);
            edges[edge_count].i = i;
            edges[edge_count].j = j;
            edge_count++;
        }
    }

    // Sort by distance
    qsort(edges, edge_count, sizeof(Edge), compare_edges);

    // Connect the closest pairs
    UnionFind* uf = uf_create(n);
    int connected = 0;

    for (int e = 0; e < edge_count && connected < connections; e++) {
        uf_union(uf, edges[e].i, edges[e].j);
        connected++;
    }

    // Get circuit sizes and multiply top 3
    int* sizes = malloc(n * sizeof(int));
    int size_count = 0;

    for (int i = 0; i < n; i++) {
        if (uf_find(uf, i) == i) {
            sizes[size_count++] = uf->size[i];
        }
    }

    // Sort descending
    for (int i = 0; i < size_count - 1; i++) {
        for (int j = i + 1; j < size_count; j++) {
            if (sizes[j] > sizes[i]) {
                int tmp = sizes[i];
                sizes[i] = sizes[j];
                sizes[j] = tmp;
            }
        }
    }

    int64_t result = 1;
    for (int i = 0; i < 3 && i < size_count; i++) {
        result *= sizes[i];
    }

    free(sizes);
    uf_free(uf);
    free(edges);
    free(points);

    return result;
}

static int64_t part_one(const char* input) {
    return solve(input, 1000);
}

static int64_t part_two(const char* input) {
    int n;
    Point3D* points = parse_input(input, &n);

    // Calculate all pairwise distances
    int num_edges = n * (n - 1) / 2;
    Edge* edges = malloc(num_edges * sizeof(Edge));
    int edge_count = 0;

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            edges[edge_count].dist = distance_squared(&points[i], &points[j]);
            edges[edge_count].i = i;
            edges[edge_count].j = j;
            edge_count++;
        }
    }

    // Sort by distance
    qsort(edges, edge_count, sizeof(Edge), compare_edges);

    // Connect until all in one circuit
    UnionFind* uf = uf_create(n);
    int num_circuits = n;
    int64_t result = 0;

    for (int e = 0; e < edge_count; e++) {
        if (uf_union(uf, edges[e].i, edges[e].j)) {
            num_circuits--;
            if (num_circuits == 1) {
                result = points[edges[e].i].x * points[edges[e].j].x;
                break;
            }
        }
    }

    uf_free(uf);
    free(edges);
    free(points);

    return result;
}

DayResult day08(const char* input) {
    return (DayResult){
        .part1 = part_one(input),
        .part2 = part_two(input)
    };
}
