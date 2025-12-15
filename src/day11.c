// Day 11: Reactor - Directed graph path counting with memoization

#include "common.h"

#define MAX_NAME_LEN 32
#define MAX_NEIGHBORS 32

typedef struct {
    char name[MAX_NAME_LEN];
    int* neighbors;
    int num_neighbors;
    int neighbor_capacity;
} Node;

typedef struct {
    Node* nodes;
    int num_nodes;
    int capacity;
} Graph;

static void graph_init(Graph* g) {
    g->capacity = 1024;
    g->nodes = calloc(g->capacity, sizeof(Node));
    g->num_nodes = 0;
}

static void graph_free(Graph* g) {
    for (int i = 0; i < g->num_nodes; i++) {
        free(g->nodes[i].neighbors);
    }
    free(g->nodes);
}

static int find_or_add_node(Graph* g, const char* name) {
    for (int i = 0; i < g->num_nodes; i++) {
        if (strcmp(g->nodes[i].name, name) == 0) {
            return i;
        }
    }

    if (g->num_nodes >= g->capacity) {
        g->capacity *= 2;
        g->nodes = realloc(g->nodes, g->capacity * sizeof(Node));
    }

    int idx = g->num_nodes++;
    strncpy(g->nodes[idx].name, name, MAX_NAME_LEN - 1);
    g->nodes[idx].name[MAX_NAME_LEN - 1] = '\0';
    g->nodes[idx].num_neighbors = 0;
    g->nodes[idx].neighbor_capacity = MAX_NEIGHBORS;
    g->nodes[idx].neighbors = malloc(MAX_NEIGHBORS * sizeof(int));
    return idx;
}

static void add_neighbor(Graph* g, int from_idx, int to_idx) {
    Node* node = &g->nodes[from_idx];
    if (node->num_neighbors >= node->neighbor_capacity) {
        node->neighbor_capacity *= 2;
        node->neighbors = realloc(node->neighbors, node->neighbor_capacity * sizeof(int));
    }
    node->neighbors[node->num_neighbors++] = to_idx;
}

static void parse_input(Graph* g, const char* input) {
    graph_init(g);

    int line_count;
    char** lines = split_lines(input, &line_count);

    for (int i = 0; i < line_count; i++) {
        if (!lines[i][0] || lines[i][0] == '#') continue;

        char* colon = strchr(lines[i], ':');
        if (!colon) continue;

        // Parse "from" node
        char from_name[MAX_NAME_LEN];
        int from_len = colon - lines[i];
        if (from_len >= MAX_NAME_LEN) from_len = MAX_NAME_LEN - 1;
        strncpy(from_name, lines[i], from_len);
        from_name[from_len] = '\0';

        // Trim trailing spaces
        while (from_len > 0 && from_name[from_len - 1] == ' ') {
            from_name[--from_len] = '\0';
        }

        int from_idx = find_or_add_node(g, from_name);

        // Parse neighbors
        char* ptr = colon + 1;
        while (*ptr) {
            while (*ptr && (*ptr == ' ' || *ptr == '\t')) ptr++;
            if (!*ptr) break;

            char to_name[MAX_NAME_LEN];
            int to_len = 0;
            while (*ptr && *ptr != ' ' && *ptr != '\t' && *ptr != '\n' && to_len < MAX_NAME_LEN - 1) {
                to_name[to_len++] = *ptr++;
            }
            to_name[to_len] = '\0';

            if (to_len > 0) {
                int to_idx = find_or_add_node(g, to_name);
                add_neighbor(g, from_idx, to_idx);
            }
        }
    }

    free_lines(lines, line_count);
}

static int64_t count_paths_dfs(Graph* g, int current, int target, int64_t* memo) {
    if (current == target) return 1;

    if (memo[current] >= 0) return memo[current];

    int64_t total = 0;
    Node* node = &g->nodes[current];

    for (int i = 0; i < node->num_neighbors; i++) {
        total += count_paths_dfs(g, node->neighbors[i], target, memo);
    }

    memo[current] = total;
    return total;
}

static int64_t count_paths(Graph* g, const char* from, const char* to) {
    int from_idx = -1, to_idx = -1;

    for (int i = 0; i < g->num_nodes; i++) {
        if (strcmp(g->nodes[i].name, from) == 0) from_idx = i;
        if (strcmp(g->nodes[i].name, to) == 0) to_idx = i;
    }

    if (from_idx < 0 || to_idx < 0) return 0;

    int64_t* memo = malloc(g->num_nodes * sizeof(int64_t));

    for (int i = 0; i < g->num_nodes; i++) {
        memo[i] = -1;
    }

    int64_t result = count_paths_dfs(g, from_idx, to_idx, memo);

    free(memo);
    return result;
}

// Count paths that visit both via1 and via2
static int64_t count_paths_via_two(Graph* g, const char* from, const char* to,
                                   const char* via1, const char* via2) {
    // Path: from -> via1 -> via2 -> to
    int64_t p1 = count_paths(g, from, via1) *
                 count_paths(g, via1, via2) *
                 count_paths(g, via2, to);

    // Path: from -> via2 -> via1 -> to
    int64_t p2 = count_paths(g, from, via2) *
                 count_paths(g, via2, via1) *
                 count_paths(g, via1, to);

    return p1 + p2;
}

static int64_t part_one(const char* input) {
    Graph g;
    parse_input(&g, input);
    int64_t result = count_paths(&g, "you", "out");
    graph_free(&g);
    return result;
}

static int64_t part_two(const char* input) {
    Graph g;
    parse_input(&g, input);
    int64_t result = count_paths_via_two(&g, "svr", "out", "dac", "fft");
    graph_free(&g);
    return result;
}

DayResult day11(const char* input) {
    return (DayResult){
        .part1 = part_one(input),
        .part2 = part_two(input)
    };
}
