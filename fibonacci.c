#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct ThreadData {
    int n;
    int *arr;
};

void* FibonacciWorker(void* arg) {
    struct ThreadData* data = (struct ThreadData*)arg;
    int n = data->n;
    int *arr = data->arr;

    if (n > 0) arr[0] = 0;
    if (n > 1) arr[1] = 1;

    for (int i = 2; i < n; i++) {
        arr[i] = arr[i-1] + arr[i-2];
    }

    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <number of elements>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        printf("Please provide a positive integer.\n");
        return 1;
    }

    int *arr = (int*)malloc(n * sizeof(int));
    if (arr == NULL) {
        perror("Failed to allocate memory");
        return 1;
    }

    pthread_t thread;
    struct ThreadData data;
    data.n = n;
    data.arr = arr;

    if (pthread_create(&thread, NULL, FibonacciWorker, &data) != 0) {
        perror("Failed to create thread");
        free(arr);
        return 1;
    }

    if (pthread_join(thread, NULL) != 0) {
        perror("Failed to join thread");
        free(arr);
        return 1;
    }

    printf("Fibonacci sequence of %d numbers:\n", n);
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    free(arr);
    return 0;
}
