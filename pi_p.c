#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

double GetTime() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1000000.0;
}

struct ThreadData {
    int thread_id;
    int num_threads;
    int n;
};

double f(double a)
{
    return (4.0 / (1.0 + a*a));
}

void* ThreadFunc(void* arg) {
    struct ThreadData* data = (struct ThreadData*)arg;
    int n = data->n;
    int thread_id = data->thread_id;
    int num_threads = data->num_threads;
    
    double fH = 1.0 / (double) n;
    double fSum = 0.0;
    double fX;
    
    int chunk_size = n / num_threads;
    int start = thread_id * chunk_size;
    int end = (thread_id == num_threads - 1) ? n : start + chunk_size;
    
    for (int i = start; i < end; i++) {
        fX = fH * ((double)i + 0.5);
        fSum += f(fX);
    }
    
    // Return partial sum dynamically allocated
    double* result = (double*)malloc(sizeof(double));
    *result = fSum;
    return (void*)result;
}

int main(int argc, char **argv)
{
    int n = 2000000000;
    int num_threads = 1;
    const double fPi25DT = 3.141592653589793238462643;
    double fPi = 0.0;
    double fTimeStart, fTimeEnd;
    
    if (argc > 1) {
        num_threads = atoi(argv[1]);
    }
    
    if (num_threads <= 0) num_threads = 1;

    printf("Calculating Pi with %d threads and %d intervals.\n", num_threads, n);

    // get initial time 
    fTimeStart = GetTime();

    pthread_t* threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    struct ThreadData* thread_data = (struct ThreadData*)malloc(num_threads * sizeof(struct ThreadData));

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].num_threads = num_threads;
        thread_data[i].n = n;
        pthread_create(&threads[i], NULL, ThreadFunc, &thread_data[i]);
    }

    double total_sum = 0.0;
    for (int i = 0; i < num_threads; i++) {
        void* retval;
        pthread_join(threads[i], &retval);
        double* partial_sum = (double*)retval;
        total_sum += *partial_sum;
        free(partial_sum);
    }
    
    double fH = 1.0 / (double) n;
    fPi = total_sum * fH;

    //get final fime
    fTimeEnd = GetTime();
    
    printf("\npi is approximately = %.20f \nError               = %.20f\n",
           fPi, fabs(fPi - fPi25DT));
    
    // report time
    printf("Time elapsed: %.6f seconds\n", fTimeEnd - fTimeStart);

    free(threads);
    free(thread_data);

    return 0;
}
