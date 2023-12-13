#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct ArrayList{
    int *items;
    size_t size;
    size_t capacity;
} ArrayList;

ArrayList init_array_list(size_t initial_capacity){
    ArrayList new_array_list;
    new_array_list.items = malloc(initial_capacity * sizeof(size_t));
    new_array_list.size = 0;
    new_array_list.capacity = initial_capacity;
    return new_array_list;
}

void push_array_list(ArrayList *array_list, int value){
    array_list->size = array_list->size + 1;
    if(array_list->capacity < array_list->size){
        array_list->capacity = array_list->capacity + 5;
        array_list->items = realloc(array_list->items, array_list->capacity * sizeof(size_t));
    }
    array_list->items[array_list->size-1] = value;
}

void reverse_array_list(ArrayList *array_list){
    for (size_t i = 0; i < array_list->size / 2; ++i) {
        int temp = array_list->items[i];
        array_list->items[i] = array_list->items[array_list->size - i - 1];
        array_list->items[array_list->size - i - 1] = temp;
    }
}

char *pad_str(char *str, int target_length, char ch){
    size_t str_length = strlen(str);
    size_t abs_target_length = abs(target_length);
    if(str_length >= abs_target_length || target_length == 0){
        char *clone_str = malloc(str_length * sizeof(char));
        memcpy(clone_str, str, str_length);
        return clone_str;
    }

    char *target_str = malloc(target_length * sizeof(char));

    size_t pad_length = abs_target_length - str_length;
    if(target_length>0){
        for (size_t i = 0; i < str_length; ++i){
            target_str[i] = str[i];
        }
        for (size_t i = 0; i < pad_length; ++i) {
            target_str[i+str_length] = ch;
        }
    }else if(target_length<0){
        for (size_t i = 0; i < pad_length; ++i) {
            target_str[i] = ch;
        }
        for (size_t i = 0; i < str_length; ++i){
            target_str[i+pad_length] = str[i];
        }
    }

    return target_str;
}

typedef struct DijkstraState {
    size_t curr_visit;
    int *dist;
    int *prev;
} DijkstraState;

int find_min_node_index(size_t nodes_len, bool visit[], int dist[]){
    int min_index = -1;
    for (size_t i = 0; i < nodes_len; ++i) {
        if(visit[i] == false){
            if(min_index == -1){
                min_index = i;
            } else if(dist[i] < dist[min_index]) {
                min_index = i;
            }
        }
    }
    return min_index;
}

DijkstraState *dijkstra(size_t nodes_len, int graph[], size_t src){ 
    // Init visited array
    bool visit[nodes_len];
    
    // Init dijkstra state history
    DijkstraState *dijkstra_state_history = malloc(nodes_len * sizeof(DijkstraState));

    // Set initial states
    int dist[nodes_len];
    dist[src] = 0;
    int prev[nodes_len];
    prev[src] = src;

    for (size_t i = 0; i < nodes_len; ++i) { 
        visit[i] = false;
        if (i != src) {
            dist[i] = INT_MAX;
            prev[i] = -1;
        }
    }

    // Keep track of the number of visited nodes
    size_t visit_count = 0;
    // Get unvisited min distance node
    int min_node_index = find_min_node_index(nodes_len, visit, dist);
    // If there is an unvisited min distance node then loop
    while (min_node_index >= 0) {
        // Mark min distance node as visited
        visit[min_node_index] = true;

        // Store current visited node in history
        dijkstra_state_history[visit_count].curr_visit = min_node_index;
        
        // Allocate dist and prev array in history
        dijkstra_state_history[visit_count].dist = malloc(nodes_len * sizeof(int));
        dijkstra_state_history[visit_count].prev = malloc(nodes_len * sizeof(int));

        // Loop through neighbors
        for (size_t i = min_node_index*nodes_len; i < (min_node_index*nodes_len + nodes_len); ++i) {
            int neighbor_dist = graph[i];
            int node_index = i % nodes_len;
            if (neighbor_dist > 0) {
                int total_dist = dist[min_node_index] + neighbor_dist;
                // Clamp total dist to INT_MAX to avoid overflow
                if(dist[min_node_index]==INT_MAX){
                    total_dist = INT_MAX;
                }
                // Minimize node dist from source
                if (total_dist < dist[node_index]){
                    dist[node_index] = total_dist;
                    prev[node_index] = min_node_index;
                }
            }
            
            // Store current dist and prev in history
            dijkstra_state_history[visit_count].dist[node_index] = dist[node_index];
            dijkstra_state_history[visit_count].prev[node_index] = prev[node_index];
        }

        min_node_index = find_min_node_index(nodes_len, visit, dist);
        visit_count += 1;
    }

    return dijkstra_state_history;
}

ArrayList construct_path(size_t nodes_len, DijkstraState *dijkstra_state, size_t dest){
    // Init path list
    ArrayList path = init_array_list(nodes_len);

    // Return empty path if untraceable
    if(dijkstra_state->prev[dest]==-1){
        return path;
    }

    // Trace path from destination to source
    push_array_list(&path, dest);
    int prev_node_index = dijkstra_state->prev[dest];
    while(prev_node_index != dijkstra_state->prev[prev_node_index]){
        push_array_list(&path, prev_node_index);
        prev_node_index = dijkstra_state->prev[prev_node_index];
    }
    push_array_list(&path, prev_node_index);

    // Reverse the path -> source to destination
    reverse_array_list(&path);

    return path;
}

void print_dijkstra_state_history(size_t nodes_len, char **nodes, DijkstraState *dijkstra_state_history){
    // Print table headers top border
    printf("+-%s-", pad_str("", 10, '-'));
    for (size_t i = 0; i < nodes_len; ++i){
        printf("+-%s-", pad_str("", 10, '-'));
    }
    printf("+\n");

    // Print table headers
    printf("| %s", pad_str("V", 10, ' '));
    for (size_t i = 0; i < nodes_len; ++i) {
        printf(" | %s", pad_str(nodes[i], 10, ' '));
    }
    printf(" |\n");

    // Print table headers bottom border
    printf("+-%s-", pad_str("", 10, '-'));
    for (size_t i = 0; i < nodes_len; ++i){
        printf("+-%s-", pad_str("", 10, '-'));
    }
    printf("+\n");

    // Print table body 
    for (size_t i = 0; i < nodes_len; ++i) {
        printf("| %s", pad_str(nodes[dijkstra_state_history[i].curr_visit], 10, ' '));
        for (size_t j = 0; j < nodes_len; ++j) {
            int curr_dist = dijkstra_state_history[i].dist[j];
            int curr_prev = dijkstra_state_history[i].prev[j];
            if (curr_prev>-1){
                size_t dist_digit = floor(log10(curr_dist)) + 1;
                size_t prev_length = strlen(nodes[curr_prev]);
                if (curr_dist==0){
                    dist_digit = 1;
                }
                char *dist_prev = malloc((dist_digit+prev_length+1) * sizeof(char));
                sprintf(dist_prev, "%d_%s", curr_dist, nodes[curr_prev]);
                printf(" | %s", pad_str(dist_prev, 10, ' '));
            } else {
                if (curr_dist==INT_MAX){
                    printf(" | %s", pad_str("inf", 10, ' '));
                }else{
                    printf(" | %s", pad_str("-", 10, ' '));
                }
            }
        }
        printf(" |\n");
    }

    // Print table bottom border
    printf("+-%s-", pad_str("", 10, '-'));
    for (size_t i = 0; i < nodes_len; ++i){
        printf("+-%s-", pad_str("", 10, '-'));
    }
    printf("+\n");
}

int main(void){
    // Init nodes
    char *nodes[] = {"Monaire", "Poirott", "Milis", "Bouche", "Tempest", "Ranoa", "Jura", "Asura"};

    // Get nodes length
    size_t nodes_len = sizeof(nodes) / sizeof(nodes[0]);

    // Init graph as adjacency matrix
    int adj_mat[] = {
        0, 4, 13, 0, 0, 0, 0, 0,
        0, 0, 5, 8, 0, 0, 0, 0,
        0, 0, 0, 0, 5, 10, 0, 0,
        0, 0, 0, 0, 3, 0, 3, 14,
        0, 0, 0, 0, 0, 6, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 5,
        0, 0, 0, 0, 0, 0, 0, 12,
        0, 0, 0, 0, 0, 0, 0, 0
    };

    // Init source (index)
    size_t src = 0;

    // Init destination (index)
    size_t dest = 7;

    // Run Dijkstra
    DijkstraState *dijkstra_state_history = dijkstra(nodes_len, adj_mat, src);

    // Print table
    print_dijkstra_state_history(nodes_len, nodes, dijkstra_state_history);
    
    // Path construction to destination
    ArrayList path = construct_path(nodes_len, &dijkstra_state_history[nodes_len - 1], dest);

    // If path doesn't exist
    if(path.size==0){
        printf("There exists no path from %s to %s\n", nodes[src], nodes[dest]);

        goto cleanup;
    }

    // If path does exist
    printf("Shortest path from %s to %s:\n", nodes[src], nodes[dest]);
    for (size_t i = 0; i < path.size-1; ++i){
        printf("%d", adj_mat[path.items[i]*nodes_len + path.items[i+1]]);
        if (i<path.size-2){
            printf(" + ");
        }else{
            printf(" = %d \n", dijkstra_state_history[nodes_len - 1].dist[dest]);
        }
    }

    for (size_t i = 0; i < path.size; ++i) {
        printf("%s", nodes[path.items[i]]);
        if (i<path.size-1){
            printf(" -> ");
        }else{
            printf("\n");
        }
    }

cleanup:
    free(path.items);
    free(dijkstra_state_history);

    return 0;
}
