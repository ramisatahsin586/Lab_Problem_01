
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

typedef struct {
    int source, destination, weight, order;
} Edge;

int FindMinCost(int vertices, long long minCost[], bool included[]) {
    long long minimum = LLONG_MAX / 4;
    int minIndex = -1;

    for (int i = 0; i < vertices; i++) {
        if (!included[i] && minCost[i] < minimum) {
            minimum = minCost[i];
            minIndex = i;
        }
    }

    return minIndex;
}

int FindParent(int parent[], int vertex) {
    if (parent[vertex] != vertex) {
        parent[vertex] = FindParent(parent, parent[vertex]);
    }

    return parent[vertex];
}

bool FindTreePath(int vertices, int tree[vertices][vertices],
                  int source, int destination, int path[],
                  int *pathLength, long long *cost) {

    int *parent = malloc(vertices * sizeof(int));
    int *queue = malloc(vertices * sizeof(int));
    bool *visited = malloc(vertices * sizeof(bool));

    if (parent == NULL || queue == NULL || visited == NULL) {
        free(parent);
        free(queue);
        free(visited);
        return false;
    }

    for (int i = 0; i < vertices; i++) {
        parent[i] = -1;
        visited[i] = false;
    }

    int front = 0, rear = 0;

    queue[rear++] = source;
    visited[source] = true;

    while (front < rear) {
        int current = queue[front++];

        if (current == destination) {
            break;
        }

        for (int i = 0; i < vertices; i++) {
            if (tree[current][i] != -1 && !visited[i]) {
                visited[i] = true;
                parent[i] = current;
                queue[rear++] = i;
            }
        }
    }

    if (!visited[destination]) {
        free(parent);
        free(queue);
        free(visited);
        return false;
    }

    *pathLength = 0;
    *cost = 0;

    int current = destination;

    while (current != -1) {
        path[(*pathLength)++] = current;

        if (parent[current] != -1) {
            *cost += tree[current][parent[current]];
        }

        current = parent[current];
    }

    free(parent);
    free(queue);
    free(visited);

    return true;
}

void PrintPath(int path[], int pathLength, long long cost) {
    for (int i = pathLength - 1; i >= 0; i--) {
        printf("%d", path[i]);

        if (i > 0) {
            printf(" -> ");
        }
    }

    printf(" (Cost: %lld)\n", cost);
}

void Dijkstra(int vertices, int graph[vertices][vertices],
              int source, int destination) {

    long long *distance = malloc(vertices * sizeof(long long));
    int *parent = malloc(vertices * sizeof(int));
    bool *visited = malloc(vertices * sizeof(bool));
    int *path = malloc(vertices * sizeof(int));

    if (distance == NULL || parent == NULL ||
        visited == NULL || path == NULL) {

        printf("Memory allocation failed!\n");

        free(distance);
        free(parent);
        free(visited);
        free(path);
        return;
    }

    for (int i = 0; i < vertices; i++) {
        distance[i] = LLONG_MAX / 4;
        parent[i] = -1;
        visited[i] = false;
    }

    distance[source] = 0;

    for (int count = 0; count < vertices; count++) {

        int current = FindMinCost(vertices, distance, visited);

        if (current == -1) {
            break;
        }

        visited[current] = true;

        for (int i = 0; i < vertices; i++) {
            if (graph[current][i] != -1 &&
                !visited[i] &&
                distance[current] + graph[current][i] < distance[i]) {

                distance[i] = distance[current] + graph[current][i];
                parent[i] = current;
            }
        }
    }

    if (distance[destination] == LLONG_MAX / 4) {
        printf("Not Possible\n");
    }
    else {
        int pathLength = 0;
        int current = destination;

        while (current != -1) {
            path[pathLength++] = current;
            current = parent[current];
        }

        PrintPath(path, pathLength, distance[destination]);
    }

    free(distance);
    free(parent);
    free(visited);
    free(path);
}

bool Kruskal(int vertices, int edges, Edge edgeList[],
             int tree[vertices][vertices]) {

    Edge *sortedEdges = malloc((edges > 0 ? edges : 1) * sizeof(Edge));
    int *parent = malloc(vertices * sizeof(int));
    int *rank = malloc(vertices * sizeof(int));

    if (sortedEdges == NULL || parent == NULL || rank == NULL) {
        free(sortedEdges);
        free(parent);
        free(rank);
        return false;
    }

    for (int i = 0; i < vertices; i++) {
        parent[i] = i;
        rank[i] = 0;

        for (int j = 0; j < vertices; j++) {
            tree[i][j] = -1;
        }
    }

    for (int i = 0; i < edges; i++) {
        sortedEdges[i] = edgeList[i];
    }

    // Bubble Sort
    for (int i = 0; i < edges - 1; i++) {
        for (int j = 0; j < edges - i - 1; j++) {

            if (sortedEdges[j].weight > sortedEdges[j + 1].weight ||
                (sortedEdges[j].weight == sortedEdges[j + 1].weight &&
                 sortedEdges[j].order > sortedEdges[j + 1].order)) {

                Edge temp = sortedEdges[j];
                sortedEdges[j] = sortedEdges[j + 1];
                sortedEdges[j + 1] = temp;
            }
        }
    }

    int selectedEdges = 0;

    for (int i = 0; i < edges && selectedEdges < vertices - 1; i++) {

        int sourceRoot = FindParent(parent, sortedEdges[i].source);
        int destinationRoot = FindParent(parent, sortedEdges[i].destination);

        if (sourceRoot != destinationRoot) {

            if (rank[sourceRoot] < rank[destinationRoot]) {
                parent[sourceRoot] = destinationRoot;
            }
            else if (rank[sourceRoot] > rank[destinationRoot]) {
                parent[destinationRoot] = sourceRoot;
            }
            else {
                parent[destinationRoot] = sourceRoot;
                rank[sourceRoot]++;
            }

            int u = sortedEdges[i].source;
            int v = sortedEdges[i].destination;
            int w = sortedEdges[i].weight;

            tree[u][v] = w;
            tree[v][u] = w;

            selectedEdges++;
        }
    }

    free(sortedEdges);
    free(parent);
    free(rank);

    return selectedEdges == vertices - 1;
}

bool Prim(int vertices, int graph[vertices][vertices],
          int source, int tree[vertices][vertices]) {

    int *parent = malloc(vertices * sizeof(int));
    long long *minCost = malloc(vertices * sizeof(long long));
    bool *included = malloc(vertices * sizeof(bool));

    if (parent == NULL || minCost == NULL || included == NULL) {
        free(parent);
        free(minCost);
        free(included);
        return false;
    }

    for (int i = 0; i < vertices; i++) {
        minCost[i] = LLONG_MAX / 4;
        parent[i] = -1;
        included[i] = false;

        for (int j = 0; j < vertices; j++) {
            tree[i][j] = -1;
        }
    }

    minCost[source] = 0;

    for (int count = 0; count < vertices; count++) {

        int current = FindMinCost(vertices, minCost, included);

        if (current == -1) {
            free(parent);
            free(minCost);
            free(included);
            return false;
        }

        included[current] = true;

        for (int i = 0; i < vertices; i++) {
            if (graph[current][i] != -1 &&
                !included[i] &&
                graph[current][i] < minCost[i]) {

                minCost[i] = graph[current][i];
                parent[i] = current;
            }
        }
    }

    for (int i = 0; i < vertices; i++) {
        if (parent[i] != -1) {
            tree[i][parent[i]] = graph[i][parent[i]];
            tree[parent[i]][i] = graph[i][parent[i]];
        }
    }

    free(parent);
    free(minCost);
    free(included);

    return true;
}

void ShowKruskal(int vertices, int tree[vertices][vertices],
                 int source, int destination) {

    int *path = malloc(vertices * sizeof(int));
    int pathLength;
    long long cost;

    if (path == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    if (FindTreePath(vertices, tree, source, destination, path, &pathLength, &cost)) {

        PrintPath(path, pathLength, cost);
    }
    else {
        printf("Not Possible\n");
    }

    free(path);
}

void ShowPrim(int vertices, int tree[vertices][vertices],
              int source, int destination) {

    int *path = malloc(vertices * sizeof(int));
    int pathLength;
    long long cost;

    if (path == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    if (FindTreePath(vertices, tree, source, destination,
                     path, &pathLength, &cost)) {

        PrintPath(path, pathLength, cost);
    }
    else {
        printf("Not Possible\n");
    }

    free(path);
}

int main(void) {

    FILE *file = fopen("graph.txt", "r");

    int vertices, edges;

    if (fscanf(file, "%d %d", &vertices, &edges) != 2 || vertices < 1 || edges < 0 || edges > vertices * (vertices - 1) / 2) {

        printf("Invalid graph input!\n");
        fclose(file);
        return 1;
    }

    Edge *edgeList = malloc((edges > 0 ? edges : 1) * sizeof(Edge));
    int (*graph)[vertices] = malloc((size_t)vertices * sizeof(*graph));
    int (*kruskalTree)[vertices] = malloc((size_t)vertices * sizeof(*kruskalTree));
    int (*primTree)[vertices] = malloc((size_t)vertices * sizeof(*primTree));

    if (edgeList == NULL || graph == NULL || kruskalTree == NULL || primTree == NULL) {

        printf("Memory allocation failed!\n");

        free(edgeList);
        free(graph);
        free(kruskalTree);
        free(primTree);
        fclose(file);
        return 1;
    }

    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            graph[i][j] = -1;
        }
    }

    for (int i = 0; i < edges; i++) {

        int u, v, w;

        if (fscanf(file, "%d %d %d", &u, &v, &w) != 3 ||
            u < 0 || u >= vertices ||
            v < 0 || v >= vertices ||
            u == v || w < 0 ||
            graph[u][v] != -1) {

            printf("Invalid edge input!\n");

            free(edgeList);
            free(graph);
            free(kruskalTree);
            free(primTree);
            fclose(file);
            return 1;
        }

        edgeList[i].source = u;
        edgeList[i].destination = v;
        edgeList[i].weight = w;
        edgeList[i].order = i;

        graph[u][v] = w;
        graph[v][u] = w;
    }

    int source, destination;
    int removeSource, removeDestination;

    if (fscanf(file, "%d %d", &source, &destination) != 2 ||
        fscanf(file, "%d %d", &removeSource, &removeDestination) != 2 || source < 0 || source >= vertices ||
        destination < 0 || destination >= vertices || removeSource < 0 || removeSource >= vertices ||
        removeDestination < 0 || removeDestination >= vertices) {

        printf("Invalid source, destination or removal edge!\n");

        free(edgeList);
        free(graph);
        free(kruskalTree);
        free(primTree);
        fclose(file);
        return 1;
    }

    fclose(file);

    int removeIndex = -1;

    for (int i = 0; i < edges; i++) {
        if ((edgeList[i].source == removeSource &&
             edgeList[i].destination == removeDestination) ||
            (edgeList[i].source == removeDestination &&
             edgeList[i].destination == removeSource)) {

            removeIndex = i;
            break;
        }
    }

    if (removeIndex == -1) {
        printf("Edge to remove not found!\n");

        free(edgeList);
        free(graph);
        free(kruskalTree);
        free(primTree);
        return 1;
    }

    printf("Shortest Path: ");
    Dijkstra(vertices, graph, source, destination);

    printf("Kruskal Path:  ");

    if (Kruskal(vertices, edges, edgeList, kruskalTree)) {
        ShowKruskal(vertices, kruskalTree, source, destination);
    }
    else {
        printf("Not Possible\n");
    }

    printf("Prim Path:     ");

    if (Prim(vertices, graph, source, primTree)) {
        ShowPrim(vertices, primTree, source, destination);
    }
    else {
        printf("Not Possible\n");
    }

    printf("After Removing Edge (%d,%d):\n",
           removeSource, removeDestination);

    graph[removeSource][removeDestination] = -1;
    graph[removeDestination][removeSource] = -1;

    Edge *newEdgeList = malloc((edges > 0 ? edges : 1) * sizeof(Edge));

    if (newEdgeList == NULL) {
        printf("Memory allocation failed!\n");

        free(edgeList);
        free(graph);
        free(kruskalTree);
        free(primTree);
        return 1;
    }

    int newEdges = 0;

    for (int i = 0; i < edges; i++) {
        if (i != removeIndex) {
            newEdgeList[newEdges] = edgeList[i];
            newEdges++;
        }
    }

    printf("Updated Shortest Path: ");
    Dijkstra(vertices, graph, source, destination);

    printf("Updated Kruskal Path:  ");

    if (Kruskal(vertices, newEdges, newEdgeList, kruskalTree)) {
        ShowKruskal(vertices, kruskalTree, source, destination);
    }
    else {
        printf("Not Possible\n");
    }

    printf("Updated Prim Path:     ");

    if (Prim(vertices, graph, source, primTree)) {
        ShowPrim(vertices, primTree, source, destination);
    }
    else {
        printf("Not Possible\n");
    }

    free(edgeList);
    free(newEdgeList);
    free(graph);
    free(kruskalTree);
    free(primTree);

    return 0;
}
